#include "ClientHandler.h"
#include <QDebug>
#include <QThread>
#include <QString>


ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent)
    : QObject(parent), m_socketDescriptor(socketDescriptor)
{
}

void ClientHandler::processConnection()
{
    m_socket = new QTcpSocket();

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << "Error setting socket descriptor:" << m_socket->errorString();
        delete m_socket;
        emit finished();
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::readData);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

}

void ClientHandler::readData()
{
    QByteArray data = m_socket->readAll();
    // qDebug() << "Received data:" << data;
    // m_socket->write(QString("Echo: %1").arg(QString(data)).toUtf8());  //back write
    QString text = QString::fromUtf8(data); // UTF-8 编码

    if(text.contains("th:")){
        emit dataReady(text); // 发出data
    }
}

void ClientHandler::onDisconnected()
{
    // qDebug() << "Client disconnected";
    m_socket->deleteLater();
    emit finished();
}
