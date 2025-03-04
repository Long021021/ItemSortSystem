#ifndef THREADEDTCPSERVER_H
#define THREADEDTCPSERVER_H

#include <QTcpServer>
#include <QThread>
#include "ClientHandler.h"
#include "environment.h"

class ThreadedTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ThreadedTcpServer(Environment *envi,QObject *parent = nullptr);
    ClientHandler *handler;
    Environment *env;

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // THREADEDTCPSERVER_H
