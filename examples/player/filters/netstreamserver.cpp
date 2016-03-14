#include "netstreamserver.h"
#include <QDebug>

static const int PayloadSize = 64 * 1024; // 64 KB
static const int UDPDatagramSize = 512; // bytes

NetStreamServer::NetStreamServer(QObject *parent) : QObject(parent)
{
    qDebug()<<"NetStreamServer constructor";
    connect(&tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection()));

    connect(&tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

}

NetStreamServer::~NetStreamServer()
{

    if (TcpNetStreamThread){
        TcpNetStreamThread->exit();
    }

    if (tcpServerConnection && tcpServerConnection->isOpen()){
        tcpServerConnection->abort();
        delete tcpServerConnection;
    }
    tcpServer.close();
}

void NetStreamServer::start()
{
    //startButton->setEnabled(false);
    tcpServer.listen(QHostAddress::LocalHost,8888);
    //udpServer->bind(QHostAddress::LocalHost,8888);

    bytesWritten = 0;
    bytesReceived = 0;

    qDebug()<<"2. tcpServer.serverPort()"<<tcpServer.serverPort();

    while (!tcpServer.isListening() && !tcpServer.listen()) {
        return;
    }

    qDebug()<<"3. tcpServer.serverPort()"<<tcpServer.serverPort();
}

void NetStreamServer::acceptConnection()
{
    qDebug()<<"NetStreamServer acceptConnection";
    tcpServerConnection = tcpServer.nextPendingConnection();

    tcpServerConnectionDescriptor=tcpServerConnection->socketDescriptor();

    if (TcpNetStreamThread) {
        delete(TcpNetStreamThread);
        TcpNetStreamThread=0;
    }

    TcpNetStreamThread = new NetStreamThread(tcpServerConnectionDescriptor,&buffer);

    connect(TcpNetStreamThread, SIGNAL(bufferSent()), this, SLOT(bufferSent()));
    connect(this, SIGNAL(startThreadTransfer()), TcpNetStreamThread, SLOT(startTransfer()),Qt::QueuedConnection);

    // once a thread is not needed, it will be beleted later
    connect(TcpNetStreamThread, SIGNAL(finished()), TcpNetStreamThread, SLOT(deleteLater()));
    connect(TcpNetStreamThread, SIGNAL(destroyed(QObject*)), this, SLOT(finishTcpNetStreamThread(QObject*)));
}

void NetStreamServer::startTransfer()
{

    qDebug()<<"startTransfer";
    qDebug()<<"buffer"<<buffer.size();

    if (buffer.size() && bufferReady && tcpServerConnection && tcpServerConnection->isValid()){
        qDebug()<<"tcpServerConnection"<<tcpServerConnection->isOpen();
        TotalBytes=buffer.size();
        bytesToWrite = TotalBytes - tcpServerConnection->write(buffer);
        qDebug()<<"after";
        //bufferReady=false;
        qDebug()<<tr("Connected, sent rest")<<bytesToWrite;
        if (bytesToWrite==0) {
            bufferReady=false;
        }
    }

}

void NetStreamServer::updateServerProgress(qint64 numBytes)
{
    qint64 deltaWritten=numBytes-PreviosWritten;
    PreviosWritten=numBytes;
    //qDebug()<<"updateServerProgress"<<numBytes<<bytesWritten;
    if (numBytes){
        bytesWritten += deltaWritten;
    }

    if (bytesWritten == TotalBytes) {
        bufferReady=false;
    }




}

bool NetStreamServer::sendTCPDataBuffer()
{
    // qDebug()<<"sendTCPDataBuffer";
    if (!bufferReady && buffer.isEmpty()) return false;

    if (buffer.size() && bufferReady && tcpServerConnection && tcpServerConnection->isValid()){
        TotalBytes=buffer.size();
        tcpServerConnection->waitForBytesWritten();
        bytesToWrite = TotalBytes - tcpServerConnection->write(buffer.data(),buffer.size());
        if (bytesToWrite==0) {
            bufferReady=false;
            return true;
        }
    }

    // QThread::msleep(1000);
    return false;
}

void NetStreamServer::bufferSent()
{
    bufferReady=false;
}

void NetStreamServer::finishTcpNetStreamThread(QObject*)
{
    qDebug()<<"finishTcpNetStreamThread";
    TcpNetStreamThread=0;
}

void NetStreamServer::processPendingDatagrams()
{
    QByteArray datagram;

    do {
        datagram.resize(udpServer->pendingDatagramSize());
        udpServer->readDatagram(datagram.data(), datagram.size());
    } while (udpServer->hasPendingDatagrams());

    qDebug()<<"NetStreamServer processPendingDatagrams"<<datagram;

}

void NetStreamServer::setTotalBytes(const qint64 &value)
{
    qDebug()<<"setTotalBytes"<<value;
    TotalBytes = value;
}

bool NetStreamServer::sentUDPDataBuffer()
{
    if (udpServer && !buffer.isEmpty()){
        TotalBytes=buffer.size();
        qint64 datagrams=TotalBytes/UDPDatagramSize;
        //int restBytes=TotalBytes%UDPDatagramSize;
        bytesWritten=0;
        //qDebug()<<"datagrams"<<datagrams<<"TotalBytes"<<TotalBytes<<"restBytes"<<restBytes;
        QElapsedTimer timer;
        timer.start();
        for (int windowid=0;windowid<=datagrams;windowid++){
            QByteArray datagram = buffer.mid(UDPDatagramSize*windowid,UDPDatagramSize);
            if (!datagram.isEmpty()){

                qint64 s=udpServer->writeDatagram(datagram,UDPHostAdress,UDPport);
                bytesWritten+=s;
            }
        }
        return bytesWritten==TotalBytes;
    }
    return false;
}

void NetStreamServer::setBuffer(const QByteArray &value)
{
    //buffer = value;
    if (!bufferReady){
        buffer = value;
        bufferReady=true;
    }
    if (bufferReady && TcpNetStreamThread) {
        if (!TcpNetStreamThread->isRunning()) {
            TcpNetStreamThread->start();
        }else{
            emit startThreadTransfer();
        }
    }
}

void NetStreamServer::updateServerBytesAviable()
{
    bytesReceived += (int)tcpServerConnection->bytesAvailable();
    QByteArray readdata=tcpServerConnection->readAll();

    qDebug()<<"updateServerBytesAviable readdata"<<readdata;

}

void NetStreamServer::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug()<<"socketError"<<socketError;
    if (udpServer){
        qDebug()<< tr("Network error udpServer")<<tr("The following error occurred: %1.").arg(udpServer->errorString());
    }

}




