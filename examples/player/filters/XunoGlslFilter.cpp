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
    colorTransform();
    if (fbo() && fbo()->isValid()){
        QOpenGLFunctions *f=opengl()->openGLContext()->functions();
        if (f && fbo()->textures().size()){
            GLint target=GL_TEXTURE_2D;
            f->glBindTexture(target,fbo()->texture());
            f->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            f->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            f->glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glBindTexture(target,0);
        }
    }
    if (fbo() && fbo()->isValid() && needSave) {
        QString name=defineFileName();
        if (name.isEmpty()) name=savePath.append(QString("SaveFBOafterRendering-%1.tif").arg(QDateTime().currentMSecsSinceEpoch()));
        qDebug()<<"XunoGLSLFilter::afterRendering() fbo size() saving"<<fbo()->size()<<name;
        //SAVE FBO for text result;
        QImage s=fbo()->toImage(false);
        //QMatrix matrix = QMatrix().scale(1,-1); //OpenGL rotate;
        //s.transformed(matrix).save(name);
        if (m_player->videoCapture()->quality()>0){
            s.save(name,Q_NULLPTR,m_player->videoCapture()->quality());
        }else{
            s.save(name);
        }
        needSave=false;
    }
}

void XunoGLSLFilter::colorTransform(bool runOnce)
{
    if (colorTransformChanged){
        opengl()->setBrightness(brightness);
        opengl()->setContrast(contrast);
        opengl()->setHue(hue);
        opengl()->setSaturation(saturation);
        colorTransformChanged=!runOnce;
    }
}

void XunoGLSLFilter::setHue(const qreal &value)
{
    hue = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setContrast(const qreal &value)
{
    contrast = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setSaturation(const qreal &value)
{
    saturation = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setPlayer(QtAV::AVPlayer *player)
{
    m_player = player;
}

void XunoGLSLFilter::setBrightness(const qreal &value)
{
    brightness=value;
    colorTransformChanged=true;
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

    if (m_player){
        dir=m_player->videoCapture()->captureDir();
    }

    QDir mdir=QDir(dir);
    if (!mdir.exists()) mdir.mkpath(dir);

    if (!mdir.exists()) return "";

    QString fmt=m_player->videoCapture()->saveFormat();
    if (fmt.isEmpty()) fmt = QStringLiteral("tif");

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


