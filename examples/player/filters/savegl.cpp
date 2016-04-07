#include "savegl.h"

#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QtCore/QCoreApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QDesktopServices>
#else
#include <QtCore/QStandardPaths>
#endif

SaveGLXuno::SaveGLXuno(QObject *parent) : QObject(parent)
{

}

void SaveGLXuno::saveimg()
{
    defineFileName();
    saveFromRender();
}

void SaveGLXuno::defineFileName()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    dir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#else
    dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#endif
    if (dir.isEmpty())
        dir = qApp->applicationDirPath() + QStringLiteral("/capture");
    fmt = QStringLiteral("tif");
    QString filemovie="movie";
    qint64 moviepos=0;
    if (m_player){
        QFileInfo fi(m_player->file());
        filemovie=fi.baseName();
        moviepos=m_player->position();
    }
    filemovie.append("-GL");
    lastfilename=QString("%1/%2-%3.%4").arg(dir).arg(filemovie).arg(moviepos).arg(fmt);
}

void SaveGLXuno::saveFromGLContext()
{
    QOpenGLFunctions *f=QOpenGLContext::currentContext()->functions();

    switch (imageChannelsOutputMedia) {
    case 4:
        formatOutputMedia=GL_BGRA;
        break;
    case 3:
        formatOutputMedia=GL_BGR;
        break;
    default:
        break;
    }

    qDebug()<<"SaveGLXuno imageChannelsOutputMedia"<< imageDepthOutputMedia;

    switch (imageDepthOutputMedia) {
    case 32:
        typeOutputMedia=GL_FLOAT;
        break;
    case 16:
        typeOutputMedia=GL_UNSIGNED_SHORT;
        break;
    case 8:
        typeOutputMedia=GL_UNSIGNED_BYTE;
        break;
    default:
        typeOutputMedia=GL_UNSIGNED_BYTE;
        break;
    }

    int bytesPerCahnnel;

    switch (typeOutputMedia) {
    case GL_UNSIGNED_SHORT:
        bytesPerCahnnel=sizeof(GLushort);
        qDebug()<<"SaveGLXuno GL_UNSIGNED_SHORT"<< bytesPerCahnnel;
        break;
    case GL_2_BYTES:
        bytesPerCahnnel=2;
        qDebug()<<"SaveGLXuno GL_2_BYTES"<< bytesPerCahnnel;
        break;
    case GL_UNSIGNED_BYTE:
        bytesPerCahnnel=sizeof(GL_UNSIGNED_BYTE);
        qDebug()<<"SaveGLXuno GL_UNSIGNED_SHORT"<< bytesPerCahnnel;
        break;
    default:
        bytesPerCahnnel=1;
        qDebug()<<"SaveGLXuno typeOutputMedia default"<< bytesPerCahnnel;
        break;
    }

    qDebug()<<"SaveGLXuno::saveGL"<<imageChannelsOutputMedia<<bytesPerCahnnel;
    qDebug()<<"SaveGLXuno::saveGL oWxH"<<wOutputMedia<<wOutputMedia;
    long size=wOutputMedia*wOutputMedia*imageChannelsOutputMedia*bytesPerCahnnel;
    qDebug()<<"SaveGLXuno::saveGL imageChannelsOutputMedia,bytesPerCahnnel,size"<<imageChannelsOutputMedia<<bytesPerCahnnel<<size;
    releasePixels();
    if (!pixels) pixels=new unsigned char [size];

    //f->glBindFramebuffer(GL_READ_FRAMEBUFFER, max_fbo);
    while (f->glGetError() != GL_NO_ERROR);
    qDebug()<<"read pixels1 before, size"<<size<<pixels[0]<<pixels[1]<<pixels[2]<<pixels[3]<<pixels[4];
    f->glReadPixels(0,0,wOutputMedia,hOutputMedia,formatOutputMedia,typeOutputMedia,pixels);
    qDebug()<<"read pixels1 after, size"<<size<<pixels[0]<<pixels[1]<<pixels[2]<<pixels[3]<<pixels[4];


}

void SaveGLXuno::saveFromRender()
{
    if (m_player){
        pixels=Q_NULLPTR;
        int w=0;
        int h=0;
        int bpp=4;
        bool readypixels=m_player->renderer()->getPixels(pixels, w, h, bpp);
        if (readypixels && pixels && w>0 && h>0){
            if (1){
                qint64 byteCount=w*h*bpp;
                //for rotate OpenGL image vertically ised QImage
                QImage qi= QImage(pixels,w,h,QImage::Format_ARGB32).copy();
                if (!qi.isNull() && !lastfilename.isEmpty()){
                    m_player->videoCapture()->captureDir();
                    Config::instance().captureFormat();
                    QMatrix matrix = QMatrix().scale(1,-1);
                    qi.transformed(matrix).save(lastfilename);
                    qDebug()<<"Saved saveFromRender"<<lastfilename;
                    //                    QMatrix matrix = QMatrix().scale(1,-1);
                    //                    buffer=QByteArray((const char*) qi.transformed(matrix).bits(),byteCount);
                    //                    emit onSentFrame();
                }
            }
        }
    }
}

bool SaveGLXuno::releasePixels()
{
    if (pixels) {
        delete []pixels;
        pixels=Q_NULLPTR;
        return true;
    }
    return false;
}

void SaveGLXuno::setPlayer(QtAV::AVPlayer *player)
{
    m_player=player;
}

