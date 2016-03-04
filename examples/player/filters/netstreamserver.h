#ifndef NETSTREAMSERVER_H
#define NETSTREAMSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDialog>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QElapsedTimer>
#include "netstreamthread.h"

class NetStreamServer : public QObject
{
    Q_OBJECT
public:
    explicit NetStreamServer(QObject *parent = 0);

    void setBuffer(const QByteArray &value);
    void setTotalBytes(const qint64 &value);
    bool sentUDPDataBuffer();
public slots:
    void start();
    void acceptConnection();
    void startTransfer();
    //void updateServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);
    void updateServerProgress(qint64 numBytes);
    void processPendingDatagrams();
    void updateServerBytesAviable();
    bool sendTCPDataBuffer();
    void bufferSent();

signals:
    void startThreadTransfer();


private:
    QByteArray buffer;
    bool bufferReady=false;
    QTcpServer tcpServer;
    QTcpSocket tcpClient;
    QTcpSocket *tcpServerConnection=0;
    QUdpSocket *udpServer=0;
    qint64 bytesToWrite;
    qint64 bytesWritten;
    qint64 bytesReceived;
    QProcess runprocess;
    QFile *file;
    qint64 TotalBytes;
    qint64 PreviosWritten=0;
    qint64 UDPport=8888;
    QHostAddress UDPHostAdress=QHostAddress::Broadcast;
    int tcpServerConnectionDescriptor;
    NetStreamThread *TcpNetStreamThread=0;

};

#endif // NETSTREAMSERVER_H
