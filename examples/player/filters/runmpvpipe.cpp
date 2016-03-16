#include "runmpvpipe.h"
//#define _POSIX_C_SOURCE
#include <QFile>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include <QFileInfo>
#include <QSize>
#include <QTimer>
#include <QWindow>
#include <QWidget>

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

bool runmpvpipe::runApp()
{
    if (mpvprocess && frameW>0 && frameH>0){
        if (!presentMpvApp()) return false;
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
        QString filepath=QString(mpvAppEXE).append(".com").append(" ").append(mpvparam);
        file =_popen(filepath.toLatin1().constData(),"wb");
        qDebug()<<"runmpvpipe::runApp"<<filepath<<mpvparam;
        //started();
        return true;
    }
    return false;
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

WId runmpvpipe::findAppWindow(QString ext)
{
    if (ext=="exe"){
        // Windows: Find HWND by window title EXE
        WId id = (WId)FindWindow(NULL, L"No file - mpv");

        if (id) {
            qDebug()<<"Found by tile";
            TCHAR ClassName[MAX_PATH];
            int r=GetClassName((HWND)id,ClassName,_countof(ClassName));
            if (r){
                qDebug()<<"GetClassName"<<r<<QString::fromWCharArray(ClassName);
            }
        }else{
            TCHAR ClassName[]=L"mpv";
            id = (WId)FindWindowEx(NULL,NULL,ClassName,NULL);
            if (id) qDebug()<<"Found by ClassName";
        }

        if (!id) {
            qDebug()<<"not found Window"<<id;
            return -1;
        }

        return id;
    }else if (ext=="com"){
        // Windows: Find HWND by window title COM
        WId id = (WId)FindWindow(NULL, L"C:\\WINDOWS\\system32\\cmd.exe");

        if (id) {
            qDebug()<<"Found by tile";
            TCHAR ClassName[MAX_PATH];
            int r=GetClassName((HWND)id,ClassName,_countof(ClassName));
            if (r){
                qDebug()<<"GetClassName"<<r<<QString::fromWCharArray(ClassName);
            }
        }else{
            TCHAR ClassName[]=L"Windows Command Processor";
            id = (WId)FindWindowEx(NULL,NULL,ClassName,NULL);
            if (id) qDebug()<<"Found by ClassName";
        }

        if (!id) {
            qDebug()<<"not found Window"<<id;
            return -1;
        }

        return id;
    }
    return -1;
}

bool runmpvpipe::getMovedApp() const
{
    return movedApp;
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

QWidget * runmpvpipe::moveMpvApp()
{
    if (movedApp) return nullptr;
    //QThread::sleep(5);
    QWindow* window = nullptr;
    WId id = findAppWindow("com");
    if (id!=-1) {
        window = QWindow::fromWinId(id);
        if (window != nullptr) {
            window->hide();
        }
    }

    id = findAppWindow("exe");
    if (id==-1) return nullptr;
    window = nullptr;
    window = QWindow::fromWinId(id);
    if (window == nullptr) return nullptr;
    window->show();
    window->requestActivate();

    //    QObject::connect(&t, &QTimer::timeout, [=]
    //    {
    //        qDebug() << "=== Inner QWindow ===";
    //        qDebug() << "Geometry:" << window->geometry();
    //        qDebug() << "Active?:" << window->isActive();
    //        qDebug() << "Flags:" << window->flags();
    //    });


    // Part 2
    //if (mpvwidget) delete mpvwidget;
    QWidget *mpvwidget1 = QWidget::createWindowContainer(window,mpvwidget);
    if (!mpvwidget1) return nullptr;
    mpvwidget1->show();
    mpvwidget1->resize(frameW,frameH);
    movedApp=true;

    //    QTimer t;
    //    t.start(2000);

    QObject::connect(mpvwidget1,&QWidget::destroyed, [=](QObject* a){
        qDebug()<<"mpv widject destroyed"<<a;
        movedApp=false;
        emit finished(true);
    });

    return mpvwidget1;

}

bool runmpvpipe::presentMpvApp()
{
    QString apppath=QDir::toNativeSeparators(QApplication::applicationDirPath());
    QString mpvapp=apppath.append(QDir::separator()).append(mpvAppEXE);
    QFileInfo fic(QString(mpvapp).append(".com"));
    QFileInfo fie(QString(mpvapp).append(".exe"));
    //qDebug()<<"runmpvpipe::presentMpvApp"<<mpvapp<<fic.absoluteFilePath()<<fic.exists()<<fie.absoluteFilePath()<<fie.exists();
    return (fic.exists()&& fie.exists());
}

void runmpvpipe::setWidget(QWidget *&widget)
{
    mpvwidget=widget;
}


runmpvpipe::~runmpvpipe(){

}
