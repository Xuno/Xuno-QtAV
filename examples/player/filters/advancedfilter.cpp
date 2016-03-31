#include "advancedfilter.h"
#include <QTime>
#include <QDebug>
#include <QImage>
#include <QObject>


AdvancedFilter::AdvancedFilter(QObject *parent)
    : VideoFilter(parent)
    ,firstFrame(false)
    ,m_parent(parent)
{
    qDebug()<<"AdvancedFilter constructor";
    setObjectName("AdvancedFilter");
}

AdvancedFilter::~AdvancedFilter()
{
    qDebug()<<"AdvancedFilter destructor";
#ifdef NETSTREAMSERVER_H
    if (Nss) delete Nss;
#endif
}

void AdvancedFilter::setMpvPipe(runmpvpipe *value)
{
    mpvpipe=value;
    if (mpvpipe){
        mpvpipe->setBuffer(&buffer);
    }
}

void AdvancedFilter::customEvent(QEvent *event)
{
    qDebug()<<"AdvancedFilter::customEvent"<<event->type();
    QtAV::VideoFilter::customEvent(event);
}

bool AdvancedFilter::event(QEvent *e)
{
    qDebug()<<"AdvancedFilter::event"<<e->type();
    return QtAV::VideoFilter::event(e);
}

void AdvancedFilter::setPlayer(QtAV::AVPlayer *player)
{
    m_player=player;
}

void AdvancedFilter::onStarted()
{
    qDebug()<<"AdvancedFilter::onStarted()";
}

void AdvancedFilter::process(QtAV::Statistics *statistics, QtAV::VideoFrame *frame)
{
    //    QTime duration=statistics->duration;
    //    qDebug()<<"AdvancedFilter::process - duration"<< duration;
    //    qDebug()<<"AdvancedFilter::process - video WxH"<<statistics->video_only.width<<statistics->video_only.height;
    //    qDebug()<<"AdvancedFilter::process - pts()"<< statistics->video_only.pts();
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
                    emit onSentFrame();
                }
            }
        }
    }
}

void AdvancedFilter::onPostDraw()
{
    qDebug()<<"AdvancedFilter::onPostDraw";
}


