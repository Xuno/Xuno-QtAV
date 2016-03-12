#include "netstreamfilter.h"
#include <QTime>
#include <QDebug>
#include <QImage>
#include <QThread>
#include <QObject>


NetStreamFilter::NetStreamFilter(QObject *parent)
    : VideoFilter(parent)
    ,firstFrame(false)
    ,m_parent(parent)
{
    qDebug()<<"NetStreamFilter constructor";
    setObjectName("NetStreamFilter");
    //    Nss=new NetStreamServer(this);
    //    Nss->start();
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
                    //qDebug()<<"NetStreamFilter::process before sendFrame pipe"<<pixels<<w<<h<<bpp<<buffer.size();
                    mpvpipe->sendFrame(w,h);
                    //QString name=QString("netStremFilterGLfile-%1.bmp").arg(ct.toString("hhmmsszzz"));
                    //          .save(name);//.rgbSwapped()
                    // qDebug()<<"NetStreamFilter::process saved"<<name;
                }
                //buffer=QByteArray((const char*)pixels,byteCount);
            }

        }
    }
    //    if (0 && frame && frame->isValid()){
    //        //qDebug()<<"NetStreamFilter::process frame size"<<frame->size();
    //        QImage img=frame->toImage(QImage::Format_ARGB32);
    //        //qDebug()<<"NetStreamFilter::process toImage"<<timer.elapsed()<<"milliseconds";
    //        if (!img.isNull()){
    //            //qDebug()<<"NetStreamFilter::process Qimage size"<<img.size();
    //            QTime ct=statistics->video.current_time;
    //            //            uchar* imageData=img.bits();
    //            //            int imageBytes=img.byteCount();
    //            //            int chanels=img.depth()/8;
    //            //            qDebug()<<"NetStreamFilter::process imageBytes"<<imageBytes<<"bpp"<<img.depth();
    //            //            for (int i=0;i<23;i=i+chanels){
    //            //                qDebug()<<QString("%1 - R:%2 G:%3 B:%4 A:%5").arg(i/chanels).arg(QString::number(imageData[i],16).toUpper()).arg(QString::number(imageData[i+1],16).toUpper()).arg(QString::number(imageData[i+2],16).toUpper()).arg(QString::number(imageData[i+3],16).toUpper());
    //            //            }
    //            //img.save(filename);
    //            //Nss.setBuffer(QByteArray::fromRawData((const char*)img.constBits(),img.byteCount()));
    //            //buffer=QByteArray((const char*)img.constBits(),img.byteCount());
    //            //Nss->setBuffer(buffer);
    //            if (mpvpipe){
    //                mpvpipe->sendFrame(frame->width(),frame->height());
    //            }
    //            //qDebug()<<"NetStreamFilter::process setBuffer"<<timer.elapsed()<<"milliseconds";
    //            //            if (!Nss.sentUDPDataBuffer()){
    //            //                qDebug()<<"NetStreamFilter sentDataBuffer error";
    //            //            }


    //            //            if (!Nss.sendTCPDataBuffer()){
    //            //                qDebug()<<"NetStreamFilter sentDataBuffer error";
    //            //            }

    //            //qDebug()<<"NetStreamFilter::process sentUDPDataBuffer"<<timer.elapsed()<<"milliseconds";
    //            //QThread::msleep(50);
    //            //img.bits();
    //            //            if (!firstFrame){
    //            //                QString filename=QString("saved_%1x%2_%3.bmp").arg(frame->width()).arg(frame->height()).arg(ct.toString("hhmmsszzz"));
    //            //                qDebug()<<filename<<img.byteCount();
    //            //                if (Nss) Nss->setTotalBytes((qint64)1001*img.byteCount());
    //            //                //Nss.startTransfer();
    //            //            }
    //            //            firstFrame=true;
    //            //            frames++;
    //        }
    //    }
}

void NetStreamFilter::onPostDraw()
{
    qDebug()<<"NetStreamFilter::onPostDraw";
}


