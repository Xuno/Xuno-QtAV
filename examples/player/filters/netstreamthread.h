#ifndef NETSTREAMTHREAD_H
#define NETSTREAMTHREAD_H

#include <QThread>
#include <QTcpSocket>

class NetStreamThread : public QThread
{
    Q_OBJECT

public:
    NetStreamThread(int socketDescriptor, const QByteArray &_block, QObject *parent);

    void run() Q_DECL_OVERRIDE;

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    int socketDescriptor;
    QByteArray block;
};

#endif // NETSTREAMTHREAD_H
