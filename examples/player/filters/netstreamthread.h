#ifndef NETSTREAMTHREAD_H
#define NETSTREAMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class NetStreamThread : public QThread
{
    Q_OBJECT

public:
    NetStreamThread(int socketDescriptor, QByteArray * _buffer, QObject *parent=0);

    void run() Q_DECL_OVERRIDE;

signals:
    void error(QTcpSocket::SocketError socketError);
    void bufferSent();

public slots:
    void startTransfer();


private:
    int socketDescriptor;
    QByteArray *buffer=0;
    bool bufferReady;
    QTcpSocket *tcpSocket=0;
    qint64 PreviosWritten,numBytes,TotalBytes,bytesWritten=0,bytesToWrite;

private slots:
    //void readyRead();
    void updateServerProgress(qint64);
    void disconnected();
    void tcpSocketDestroyed(QObject*);
};

#endif // NETSTREAMTHREAD_H
