#ifndef NETSTREAMSERVER_H
#define NETSTREAMSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDialog>
#include <QProcess>
#include <QMessageBox>
#include <QFile>

class NetStreamServer : public QObject
{
    Q_OBJECT
public:
    explicit NetStreamServer(QObject *parent = 0);

    void setBuffer(const QByteArray &value);

    void setTotalBytes(const qint64 &value);

public slots:
    void start();
    void acceptConnection();
    void startTransfer();
//    void updateServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);
    void updateServerProgress(qint64 numBytes);


signals:


private:
    QByteArray buffer;
    bool bufferReady=false;
    QTcpServer tcpServer;
    QTcpSocket tcpClient;
    QTcpSocket *tcpServerConnection=0;
    qint64 bytesToWrite;
    qint64 bytesWritten;
    qint64 bytesReceived;
    QProcess runprocess;
    QFile *file;
    qint64 TotalBytes;

};

#endif // NETSTREAMSERVER_H
