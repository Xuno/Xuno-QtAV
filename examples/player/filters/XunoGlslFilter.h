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
#include <QtGui/qopengl.h>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtCore/QStandardPaths>
#include <QtGui/QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "geometryengine.h"
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
    void superscale();


    bool initShaders(int pass);
    int addProgram();
    int addFBO(int scale, bool rotate);
    void initTextures();
    bool initShaders2(int pass);
    void initFrameBufers();
    void initFrameBufer(int id);
protected slots:
    void beforeRendering();
    void afterRendering();

private:
    QtAV::VideoShader *user_shader=Q_NULLPTR;
    bool needSave=false,colorTransformChanged=true;
    QString savePath;
    QtAV::AVPlayer *m_player=Q_NULLPTR;
    qreal brightness=0,contrast=0,hue=0,saturation=0;
    QSize initSize;
    QVector <QOpenGLShaderProgram *> programs;
    QVector <QOpenGLFramebufferObject *> m_fbo;
    int pass=0;
    int maxPass=0;
    QVector <int> scales;
    GeometryEngine *geometries;
    QStringList shader_files;
    QString shader_files_prefix,shader_files_include;
    QOpenGLTexture *texture=Q_NULLPTR;
    int frame=0;


};

#endif // XUNOGLSLFILTER_H
