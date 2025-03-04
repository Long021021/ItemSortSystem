#include "ThreadedTcpServer.h"
#include "ClientHandler.h"


ThreadedTcpServer::ThreadedTcpServer(Environment *envi, QObject *parent)
    : QTcpServer(parent)
{
    this->env = envi;
}

void ThreadedTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QThread *thread = new QThread();

    handler = new ClientHandler(socketDescriptor);

    if (env) {
        connect(handler, &ClientHandler::dataReady, env, &Environment::updateTemp);
    }

    handler->moveToThread(thread);

    connect(thread, &QThread::started, handler, &ClientHandler::processConnection);
    connect(handler, &ClientHandler::finished, thread, &QThread::quit);
    connect(handler, &ClientHandler::finished, handler, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}
