#include "netstreamserver.h"
#include <QDebug>

static const int PayloadSize = 64 * 1024; // 64 KB
static const int UDPDatagramSize = 512; // bytes

NetStreamServer::NetStreamServer(QObject *parent) : QObject(parent)
{
    qDebug()<<"NetStreamServer constructor";
    //    connect(&tcpServer, SIGNAL(newConnection()),
    //            this, SLOT(acceptConnection()));
    //    connect(&tcpClient, SIGNAL(connected()), this, SLOT(startTransfer()));
    //    connect(&tcpClient, SIGNAL(bytesWritten(qint64)),
    //            this, SLOT(updateClientProgress(qint64)));
    //    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),
    //            this, SLOT(displayError(QAbstractSocket::SocketError)));

    udpServer = new QUdpSocket(this);
    connect(udpServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    //udpServer->connectToHost(UDPHostAdress,UDPport,QIODevice::WriteOnly);


}

void NetStreamServer::start()
{
    //startButton->setEnabled(false);
    //tcpServer.listen(QHostAddress::LocalHost,8888);
    //udpServer->bind(QHostAddress::LocalHost,8888);

    bytesWritten = 0;
    bytesReceived = 0;

    qDebug()<<"2. tcpServer.serverPort()"<<tcpServer.serverPort();

    while (!tcpServer.isListening() && !tcpServer.listen()) {
        //        QMessageBox::StandardButton ret = QMessageBox::critical(this,
        //                                                                tr("Loopback"),
        //                                                                tr("Unable to start the test: %1.")
        //                                                                .arg(tcpServer.errorString()),
        //                                                                QMessageBox::Retry
        //                                                                | QMessageBox::Cancel);
        //        if (ret == QMessageBox::Cancel)
        return;
    }

    //serverStatusLabel->setText(tr("Listening"));
    //clientStatusLabel->setText(tr("Connecting"));
    qDebug()<<"3. tcpServer.serverPort()"<<tcpServer.serverPort();
    //tcpClient.connectToHost(QHostAddress::LocalHost, tcpServer.serverPort());

    QString mpvparam;//="--vo-defaults=opengl:scale=ewa_lanczossharp:cscale=haasnsoft:dscale=mitchell:target-prim=bt.709:target-trc=srgb:scaler-resizes-only:no-deband:prescale-passes=2:prescale-downscaling-threshold=1.6:prescale=superxbr:superxbr-sharpness=0.7";

    //    if (QApplication::arguments().size()>2){
    //        QString arg=QApplication::arguments().at(2);
    //        if (!arg.isEmpty()) mpvparam=arg;
    //    }

    qDebug()<<"Start mpv.com"<<QString("tcp://localhost:%1 %2").arg(tcpServer.serverPort()).arg(mpvparam);
    //1228800

    //runprocess.start("mpv.com", QStringList() << QString("tcp://localhost:%1 %2").arg(tcpServer.serverPort()).arg(mpvparam));

}

void NetStreamServer::acceptConnection()
{
    tcpServerConnection = tcpServer.nextPendingConnection();
    //    connect(tcpServerConnection, SIGNAL(readyRead()),
    //            this, SLOT(updateServerProgress()));

    connect(tcpServerConnection, SIGNAL(connected()), this, SLOT(startTransfer()));

    connect(tcpServerConnection, SIGNAL(bytesWritten(qint64)), this, SLOT(updateServerProgress(qint64)));


    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    qDebug()<<tr("Accepted connection");
    startTransfer();
    tcpServer.close();
}

void NetStreamServer::startTransfer()
{

    qDebug()<<"startTransfer";
    //    QString name="RED_1920x822_yuv444p10le.mp4";
    //    if (QApplication::arguments().size()>1){
    //        QString arg=QApplication::arguments().at(1);
    //        if (!arg.isEmpty()) name=arg;
    //    }
    //    qDebug()<<"Open file:"<<name;
    //    file = new QFile(name);
    //    if (!file->open(QIODevice::ReadOnly))
    //    {
    //        qDebug()<<QString("Couldn't open the file:%1").arg(name);
    //        return;
    //    }
    //    TotalBytes = file->size();

    //    QByteArray buffer = file->read(PayloadSize);

    qDebug()<<"buffer"<<buffer.size();

    if (buffer.size() && bufferReady && tcpServerConnection && tcpServerConnection->isValid()){
        qDebug()<<"tcpServerConnection"<<tcpServerConnection->isOpen();
        //        char * imageData = buffer.data();
        //        for (int i=0;i<16;i=i+4){
        //            qDebug()<<QString("sT: %1 - R:%2 G:%3 B:%4 A:%5").arg(i/4).arg(QString::number(imageData[i],16).toUpper()).arg(QString::number(imageData[i+1],16).toUpper()).arg(QString::number(imageData[i+2],16).toUpper()).arg(QString::number(imageData[i+3],16).toUpper());
        //        }
        // called when the TCP client connected to the loopback server
        qDebug()<<"before";
        //char * imageData = buffer.data();
        bytesToWrite = TotalBytes - tcpServerConnection->write(buffer);
        //buffer.clear();
        qDebug()<<"after";
        bufferReady=false;
    }
    qDebug()<<tr("Connected")<<bytesToWrite;
}

void NetStreamServer::updateServerProgress(qint64 numBytes)
{
    qDebug()<<"updateServerProgress"<<numBytes<<bytesWritten;
    if (!bufferReady && buffer.isEmpty()) return;

    //char * imageData = buffer.data();
    bytesWritten += numBytes;

    // only write more if not finished and when the Qt write buffer is below a certain size.
    if (bytesToWrite > 0 && tcpServerConnection->bytesToWrite() <= 4*PayloadSize){
        try {
            tcpServerConnection->waitForBytesWritten();
            qint64 w=tcpServerConnection->write(buffer);
            bytesToWrite -= w;
        }catch(...){
            qDebug()<<"Error in  write NetStreamServer::updateServerProgress";
        }


        //buffer.clear();
        bufferReady=false;
    }

    //    serverProgressBar->setMaximum(TotalBytes);
    //    serverProgressBar->setValue(bytesWritten);
    //    if (bytesWritten<1024*1024){
    //        //        serverStatusLabel->setText(tr("Sent %1KB").arg(bytesWritten / (1024 )));
    //    }else{
    //        //        serverStatusLabel->setText(tr("Sent %1MB").arg(bytesWritten / (1024 * 1024)));
    //    }

    if (bytesWritten == TotalBytes) {
        tcpServerConnection->close();
        qDebug()<<tr("Closed connection");
        //        startButton->setEnabled(true);
    }

    //` QThread::msleep(1000);
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
        int restBytes=TotalBytes%UDPDatagramSize;
        bytesWritten=0;
        //qDebug()<<"datagrams"<<datagrams<<"TotalBytes"<<TotalBytes<<"restBytes"<<restBytes;
        QElapsedTimer timer;
        timer.start();
        for (int windowid=0;windowid<datagrams;windowid++){
            QByteArray datagram = buffer.mid(UDPDatagramSize*windowid,UDPDatagramSize);
            if (!datagram.isEmpty()){
                bytesWritten+=udpServer->writeDatagram(datagram,UDPHostAdress,UDPport);
            }
        }
        if (restBytes){
            //qDebug()<<"restBytes"<<restBytes;
            QByteArray datagram = buffer.right(restBytes);
            bytesWritten+=udpServer->writeDatagram(datagram,UDPHostAdress,UDPport);
        }
        qDebug()<<"bytesWritten"<<bytesWritten<<timer.elapsed()<<"milliseconds";
        return bytesWritten==TotalBytes;
    }
    return false;
}

void NetStreamServer::setBuffer(const QByteArray &value)
{
     buffer = value;
//    if (!bufferReady){
//        buffer = value;
//        bufferReady=true;

//        //qDebug()<<"setBuffer"<<buffer.toHex().at(20)<<buffer.toHex().at(21);
//        //char * imageData = buffer.data();
//        //    for (int i=0;i<16;i=i+4){
//        //        qDebug()<<QString("%1 - R:%2 G:%3 B:%4 A:%5").arg(i/4).arg(QString::number(imageData[i],16).toUpper()).arg(QString::number(imageData[i+1],16).toUpper()).arg(QString::number(imageData[i+2],16).toUpper()).arg(QString::number(imageData[i+3],16).toUpper());
//        //    }
//    }else{
//        qDebug()<<"Not Redy setBuffer";
//    }
}

//void NetStreamServer::updateServerProgress()
//{
//    bytesReceived += (int)tcpServerConnection->bytesAvailable();
//    tcpServerConnection->readAll();

//    //    serverProgressBar->setMaximum(TotalBytes);
//    //    serverProgressBar->setValue(bytesReceived);
//    //    serverStatusLabel->setText(tr("Received %1MB")
//    //                               .arg(bytesReceived / (1024 * 1024)));

//    if (bytesReceived == TotalBytes) {
//        tcpServerConnection->close();
//        //startButton->setEnabled(true);
//    }
//}

void NetStreamServer::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError)
        return;

    //qDebug()<< tr("Network error")<<tr("The following error occurred: %1.").arg(tcpClient.errorString());

    //tcpClient.close();
    //tcpServer.close();
    //    clientProgressBar->reset();
    //    serverProgressBar->reset();
    //    clientStatusLabel->setText(tr("Client ready"));
    //    serverStatusLabel->setText(tr("Server ready"));
    //    startButton->setEnabled(true);

    if (udpServer){
    qDebug()<< tr("Network error udpServer")<<tr("The following error occurred: %1.").arg(udpServer->errorString());
    }

}




