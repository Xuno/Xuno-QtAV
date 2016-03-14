#include "netstreamfilter.h"
#include <QTime>
#include <QDebug>
#include <QImage>
#include <QObject>


NetStreamFilter::NetStreamFilter(QObject *parent)
    : VideoFilter(parent)
    ,firstFrame(false)
    ,m_parent(parent)
{
    qDebug()<<"NetStreamFilter constructor";
    setObjectName("NetStreamFilter");
}

NetStreamFilter::~NetStreamFilter()
{
    qDebug()<<"NetStreamFilter destructor";
    if (Nss) delete Nss;
}

void NetStreamFilter::setMpvPipe(runmpvpipe *value)
{
    mpvpipe=value;
    if (mpvpipe){
        mpvpipe->setBuffer(&buffer);
    }
}

void NetStreamFilter::customEvent(QEvent *event)
{
    qDebug()<<"NetStreamFilter::customEvent"<<event->type();
    QtAV::VideoFilter::customEvent(event);
}

bool NetStreamFilter::event(QEvent *e)
{
    qDebug()<<"NetStreamFilter::event"<<e->type();
    return QtAV::VideoFilter::event(e);
}

void NetStreamFilter::setPlayer(QtAV::AVPlayer *player)
{
    m_player=player;
}

void NetStreamFilter::onStarted()
{
    qDebug()<<"NetStreamFilter::onStarted()";
}

void NetStreamFilter::process(QtAV::Statistics *statistics, QtAV::VideoFrame *frame)
{
    //    QTime duration=statistics->duration;
    //    qDebug()<<"NetStreamFilter::process - duration"<< duration;
    //    qDebug()<<"NetStreamFilter::process - video WxH"<<statistics->video_only.width<<statistics->video_only.height;
    //    qDebug()<<"NetStreamFilter::process - pts()"<< statistics->video_only.pts();
    //    QElapsedTimer timer;
    //    timer.start();
    Q_UNUSED(statistics);
    Q_UNUSED(frame);

    if (m_player){
        uchar *pixels=nullptr;
        int w=0;
        int h=0;
        int bpp=4;
        bool readypixels=m_player->renderer()->getPixels(pixels, w, h, bpp);
        if (readypixels && pixels && w>0 && h>0){
            if (mpvpipe){
                qint64 byteCount=w*h*bpp;
                //for rotate OpenGL image vertically ised QImage
                QImage qi= QImage(pixels,w,h,QImage::Format_ARGB32).copy();
                if (!qi.isNull()){
                    QMatrix matrix = QMatrix().scale(1,-1);
                    buffer=QByteArray((const char*) qi.transformed(matrix).bits(),byteCount);
                    mpvpipe->sendFrame(w,h);
                }
            }
        }
    }
}

void NetStreamFilter::onPostDraw()
{
    qDebug()<<"NetStreamFilter::onPostDraw";
}


