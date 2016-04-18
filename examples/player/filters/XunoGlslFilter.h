#ifndef XUNOGLSLFILTER_H
#define XUNOGLSLFILTER_H
#include <QtAV>
#include <QtAV/GLSLFilter.h>
#include <QtAV/OpenGLVideo.h>
#include <QDir>
#include <QFileInfo>
#include <QtCore/QCoreApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QDesktopServices>
#include <QtOpenGL/QGLFramebufferObject>
#undef QOpenGLFramebufferObject
#define QOpenGLFramebufferObject QGLFramebufferObject
#else
#include <QtCore/QStandardPaths>
#include <QtGui/QOpenGLFramebufferObject>
#endif


QT_BEGIN_NAMESPACE
class QOpenGLFramebufferObject;
QT_END_NAMESPACE


class XunoGLSLFilter : public QtAV::GLSLFilter
{
public:
    XunoGLSLFilter(QObject* parent = 0);
    void setShader(QtAV::VideoShader *ush);
    void setNeedSave(bool value);
    void setSavePath(const QString &value);
    void setPlayer(QtAV::AVPlayer *player);
    void setBrightness(const qreal &value);
    void setContrast(const qreal &value);
    void setHue(const qreal &value);
    void setSaturation(const qreal &value);
    void colorTransform(bool runOnce=true);
    QString defineFileName();


protected slots:
    void afterRendering();

private:
    QtAV::VideoShader *user_shader=Q_NULLPTR;
    bool needSave=false,colorTransformChanged=true;
    QString savePath;
    QtAV::AVPlayer *m_player=Q_NULLPTR;
    qreal brightness=0,contrast=0,hue=0,saturation=0;


};

#endif // XUNOGLSLFILTER_H
