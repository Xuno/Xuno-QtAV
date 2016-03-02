#include "netstreamfilter.h"
#include <QTime>
#include <QDebug>
#include <QImage>
#include <QThread>


NetStreamFilter::NetStreamFilter(QObject *parent)
    : VideoFilter(parent)
    ,firstFrame(false)
{
    qDebug()<<"NetStreamFilter constructor";
    Nss.start();
}

NetStreamFilter::~NetStreamFilter()
{
    qDebug()<<"NetStreamFilter destructor";
}

void NetStreamFilter::process(QtAV::Statistics *statistics, QtAV::VideoFrame *frame)
{
    //    QTime duration=statistics->duration;
    //    qDebug()<<"NetStreamFilter::process - duration"<< duration;
    //    qDebug()<<"NetStreamFilter::process - video WxH"<<statistics->video_only.width<<statistics->video_only.height;
    //    qDebug()<<"NetStreamFilter::process - pts()"<< statistics->video_only.pts();
    //    QElapsedTimer timer;
    //    timer.start();
    if (frame && frame->isValid()){
        //qDebug()<<"NetStreamFilter::process frame size"<<frame->size();
        QImage img=frame->toImage(QImage::Format_ARGB32);
        //qDebug()<<"NetStreamFilter::process toImage"<<timer.elapsed()<<"milliseconds";
        if (!img.isNull() && frames<10001){
            //qDebug()<<"NetStreamFilter::process Qimage size"<<img.size();
            QTime ct=statistics->video.current_time;
            //            uchar* imageData=img.bits();
            //            int imageBytes=img.byteCount();
            //            int chanels=img.depth()/8;
            //            qDebug()<<"NetStreamFilter::process imageBytes"<<imageBytes<<"bpp"<<img.depth();
            //            for (int i=0;i<23;i=i+chanels){
            //                qDebug()<<QString("%1 - R:%2 G:%3 B:%4 A:%5").arg(i/chanels).arg(QString::number(imageData[i],16).toUpper()).arg(QString::number(imageData[i+1],16).toUpper()).arg(QString::number(imageData[i+2],16).toUpper()).arg(QString::number(imageData[i+3],16).toUpper());
            //            }
            //img.save(filename);
            //Nss.setBuffer(QByteArray::fromRawData((const char*)img.constBits(),img.byteCount()));
            Nss.setBuffer(QByteArray((const char*)img.constBits(),img.byteCount()));
            //qDebug()<<"NetStreamFilter::process setBuffer"<<timer.elapsed()<<"milliseconds";
            //            if (!Nss.sentUDPDataBuffer()){
            //                qDebug()<<"NetStreamFilter sentDataBuffer error";
            //            }


            if (!Nss.sendTCPDataBuffer()){
                qDebug()<<"NetStreamFilter sentDataBuffer error";
            }

            //qDebug()<<"NetStreamFilter::process sentUDPDataBuffer"<<timer.elapsed()<<"milliseconds";
            //QThread::msleep(50);
            //img.bits();
            if (!firstFrame){
                QString filename=QString("saved_%1x%2_%3.bmp").arg(frame->width()).arg(frame->height()).arg(ct.toString("hhmmsszzz"));
                qDebug()<<filename<<img.byteCount();
                Nss.setTotalBytes((qint64)1001*img.byteCount());
                //Nss.startTransfer();
            }
            firstFrame=true;
            frames++;
        }
    }
}

