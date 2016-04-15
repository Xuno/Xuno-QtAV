#include "XunoGlslFilter.h"
#include <QDebug>
#include <QImage>

XunoGLSLFilter::XunoGLSLFilter(QObject *parent):
    QtAV::GLSLFilter(parent)
{
    qDebug()<<"constructor XunoGLSLFilter::XunoGLSLFilter";
    if (opengl()){
        qDebug()<<"connect XunoGLSLFilter::XunoGLSLFilter";
        connect(opengl(), &QtAV::OpenGLVideo::afterRendering, this, &XunoGLSLFilter::afterRendering, Qt::DirectConnection);
    }
}

void XunoGLSLFilter::setShader(QtAV::VideoShader *ush)
{
    user_shader=ush;
    opengl()->setUserShader(user_shader);
}

void XunoGLSLFilter::afterRendering()
{
    //qDebug()<<"XunoGLSLFilter::afterRendering()";
    if (fbo() && fbo()->isValid() && needSave) {
        QString name=defineFileName();
        if (name.isEmpty()) name=savePath.append(QString("SaveFBOafterRendering-%1.tif").arg(QDateTime().currentMSecsSinceEpoch()));
        qDebug()<<"XunoGLSLFilter::afterRendering() fbo size() saving"<<fbo()->size()<<name;
        //SAVE FBO for text result;
        QImage s=fbo()->toImage(false);
        //QMatrix matrix = QMatrix().scale(1,-1); //OpenGL rotate;
        //s.transformed(matrix).save(name);
        s.save(name);
        needSave=false;
    }
}

void XunoGLSLFilter::setPlayer(QtAV::AVPlayer *player)
{
    m_player = player;
}

void XunoGLSLFilter::setSavePath(const QString &value)
{
    savePath = value;
}

void XunoGLSLFilter::setNeedSave(bool value)
{
    needSave = value;
}

QString XunoGLSLFilter::defineFileName()
{
    QString dir;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    dir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#else
    dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#endif
    if (dir.isEmpty())
        dir = qApp->applicationDirPath();
    dir+=QStringLiteral("/XunoPlayerCapture");

    QDir mdir=QDir(dir);
    if (!mdir.exists()) mdir.mkpath(dir);

    QString fmt = QStringLiteral("tif");
    QString filemovie="movie";
    qint64 moviepos=0;
    if (m_player){
        QFileInfo fi(m_player->file());
        filemovie=fi.baseName();
        moviepos=m_player->position();
    }
    filemovie.append("-GL");
    QString lastfilename=QString("%1/%2-%3.%4").arg(dir).arg(filemovie).arg(moviepos).arg(fmt);
    return lastfilename;
}

