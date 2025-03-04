#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

public slots:
    void processConnection();

signals:
    void finished();
    void dataReady(const QString &data); // 定义一个信号，用于发送数据

private slots:
    void readData();
    void onDisconnected();

private:
    qintptr m_socketDescriptor;
    QTcpSocket *m_socket = nullptr;
};

#endif // CLIENTHANDLER_H
