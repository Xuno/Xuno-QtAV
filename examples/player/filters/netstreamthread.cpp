#include "netstreamthread.h"
#include <QtNetwork>
#define useSleep 0

NetStreamThread::NetStreamThread(int socketDescriptor, QByteArray *_buffer, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor), buffer(_buffer)
{
    qDebug()<<"NetStreamThread::constructor";
    bytesWritten=0;
}

void NetStreamThread::run()
{
    qDebug()<<"NetStreamThread::run";
    if (buffer && buffer->isEmpty()) return;

    tcpSocket=new QTcpSocket();
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket->error());
        return;
    }

    tcpSocket->setParent(0);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(startTransfer()),Qt::DirectConnection);
    //connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)) , this, SLOT(updateServerProgress(qint64)), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(tcpSocket, SIGNAL(destroyed(QObject*)), this, SLOT(tcpSocketDestroyed(QObject*)));
    PreviosWritten=0;
#if (!useSleep)
    TotalBytes=buffer->size();
    tcpSocket->write(*buffer);
    buffer->clear();
    exec();
#else
    forever{
        if (isRunning()){
            while (buffer->isEmpty()) {
                QThread::msleep(5);
            }
            TotalBytes=buffer->size();
            if (TotalBytes && tcpSocket && tcpSocket->isOpen()){
                tcpSocket->waitForBytesWritten();
                bytesToWrite = TotalBytes - tcpSocket->write(*buffer);
            }
        }
    }
#endif
}

void NetStreamThread::updateServerProgress(qint64 numBytes)
{
    qint64 deltaWritten=numBytes-PreviosWritten;
    PreviosWritten=numBytes;
    if (numBytes){
        bytesWritten += deltaWritten;
    }
    //qDebug()<<"NetStreamThread::updateServerProgress"<<numBytes<<bytesWritten<<TotalBytes;

    if (bytesWritten >= TotalBytes) {
        //bufferReady=false;
        emit bufferSent();
        buffer->clear();
    }
}

void NetStreamThread::disconnected()
{
    qDebug() << "NetStreamThread" << socketDescriptor << " Disconnected";
    tcpSocket->deleteLater();

}

void NetStreamThread::tcpSocketDestroyed(QObject *)
{
    qDebug() << "NetStreamThread" << socketDescriptor << " tcpSocketDestroyed";
    tcpSocket=0;
    exit(0);
}

void NetStreamThread::startTransfer()
{
//    qDebug()<<"NetStreamThread::startTransfer";
//    qDebug()<<"buffer"<<buffer->size();

    if (buffer->size() && tcpSocket && tcpSocket->isOpen()){
//        qDebug()<<"tcpServerConnection"<<tcpSocket->isOpen();
//        qDebug()<<"before";
        TotalBytes=buffer->size();
        bytesToWrite = TotalBytes - tcpSocket->write(*buffer);
        //tcpSocket->waitForBytesWritten();
        tcpSocket->flush();
        //buffer->clear();
//        qDebug()<<"after";
        //bufferReady=false;
//        qDebug()<<tr("Connected, sent rest")<<bytesToWrite;
        if (bytesToWrite==0) {
            bufferReady=false;
            emit bufferSent();
            buffer->clear();
        }
    }
}


