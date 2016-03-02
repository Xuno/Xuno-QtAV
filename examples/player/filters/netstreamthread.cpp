#include "netstreamthread.h"

#include <QtNetwork>

NetStreamThread::NetStreamThread(int socketDescriptor, const QByteArray &_block, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor), block(_block)
{
}

void NetStreamThread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    tcpSocket.write(block);
//    tcpSocket.disconnectFromHost();
//    tcpSocket.waitForDisconnected();
}


