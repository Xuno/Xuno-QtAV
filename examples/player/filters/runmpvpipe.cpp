#include "runmpvpipe.h"
#define _POSIX_C_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <QSize>

runmpvpipe::runmpvpipe(QByteArray *_buffer, QObject *parent) :
    QObject(parent),
    buffer(_buffer)
{
    qDebug()<<"runmpvpipe:constructor";
    mpvprocess=new QProcess(parent);
    if (!MpvT) MpvT=new QThread(parent);
    //mpvprocess->setProcessChannelMode(QProcess::ForwardedChannels);
    mpvprocess->setProcessChannelMode(QProcess::MergedChannels);
    mpvprocess->setInputChannelMode(QProcess::ForwardedInputChannel);
    connect(mpvprocess,SIGNAL(finished(int)),this,SIGNAL(finished(int)));
    connect(mpvprocess,SIGNAL(started()),this,SLOT(started()));
    //    mpvprocess->moveToThread(MpvT);
    //    MpvT->start();

}

void runmpvpipe::sendFrame(const int &w, const int &h)
{
    //if frame that sent have other geomtry than was ininitially then skip sending raw image
    if (w!=frameW || h!=frameH) return;
    if (file!=NULL && !buffer->isEmpty()){
        //qDebug()<<"runmpvpipe:sendFrame fwrite"<<buffer->size();
        qint64 written=fwrite(buffer->data(),1,buffer->size(),file);
        if (written!=buffer->size()){
            qDebug()<<"Error written fwrite"<<written<<buffer->size();
        }
        buffer->clear();
    }else if (mpvprocess && mpvprocess->isOpen() && !buffer->isEmpty()){
        qDebug()<<"runmpvpipe:sendFrame"<<buffer->size();
        mpvprocess->write((const QByteArray&) *buffer);
        //        if (!mpvprocess->waitForBytesWritten()){
        //              qDebug()<<"runmpvpipe::sendFrame() waitForBytesWritten error";
        //        }
        buffer->clear();
    }
}

void runmpvpipe::runApp()
{
    if (mpvprocess && frameW>0 && frameH>0){
        //        QProcess process1;
        //        process1.setStandardOutputProcess(mpvprocess);
        int framebytes=frameW*frameH*bytesPerPixel;
        QString mpvparam = QString("- --cache=no --demuxer=rawvideo --demuxer-rawvideo-mp-format=bgra --demuxer-rawvideo-size=%2 --demuxer-rawvideo-fps=%3  --demuxer-rawvideo-w=%4 --demuxer-rawvideo-h=%5 --no-audio").arg(framebytes).arg(fps).arg(frameW).arg(frameH);
        //        mpvprocess->startDetached(mpvAppEXE,mpvparam.split(" "),QApplication::applicationDirPath(),&mpvprocessID);
        //        process1.start("cmd.exe");
        //mpvprocess->start(mpvAppEXE,mpvparam.split(" "));//,QApplication::applicationDirPath(),&mpvprocessID);
        //mpvprocess->start("cmd.exe",QStringList()<<"-c");
        //mpvprocess->waitForStarted();
        //const char* app=;

        file =_popen(mpvAppEXE.append(" ").append(mpvparam).toLatin1().constData(),"wb");
        qDebug()<<"runmpvpipe::runApp"<<mpvAppEXE<<mpvparam<<mpvprocessID;
    }
}

void runmpvpipe::setFameInfo(QSize fsize, qreal _fps)
{
    if (fsize.isValid()){
        frameW=fsize.width();
        frameH=fsize.height();
    }
    fps=_fps;
}

void runmpvpipe::setFameInfo(int w, int h, qreal _fps)
{
    frameW=w;
    frameH=h;
    fps=_fps;
}

void runmpvpipe::started()
{
    qDebug()<<"runmpvpipe::started()";
    emit ready();
}

void runmpvpipe::setBuffer(QByteArray *value)
{
    buffer = value;
}

void runmpvpipe::closeApp()
{
    if (file != NULL){
        _pclose(file);
        file=NULL;
    }
}


runmpvpipe::~runmpvpipe(){

}
