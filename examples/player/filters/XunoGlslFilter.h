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

#include <QtGui/qopengl.h>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtCore/QStandardPaths>
#include <QtGui/QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include "geometryengine.h"
#include "ShaderFilterXuno.h"



QT_BEGIN_NAMESPACE
class QOpenGLFramebufferObject;
QT_END_NAMESPACE

class XunoGLSLFilter : public QtAV::GLSLFilter
{
public:
    XunoGLSLFilter(QObject* parent = 0);
    void setShader(QtAV::VideoShader *ush);
    void setNeedSave(bool value);
    void setNeedSuperScale(bool value);
    void setSavePath(const QString &value);
    void setPlayer(QtAV::AVPlayer *player);
    void setBrightness(const qreal &value);
    void setContrast(const qreal &value);
    void setHue(const qreal &value);
    void setSaturation(const qreal &value);
    void colorTransform(bool runOnce=true);
    QString defineFileName();

    bool getNeedSuperScale() const;

    bool getNeedSuperScaleLastLinearFiltering() const;
    void setNeedSuperScaleLastLinearFiltering(bool value);

protected slots:
    void beforeRendering();
    void afterRendering();

private:
    QtAV::VideoShader *user_shader=Q_NULLPTR;
    bool needSave=false,colorTransformChanged=true,needSuperScale=true;
    bool needSuperScaleLastLinearFiltering=true;
    bool needAdaptiveSharpen=true;
    QString savePath;
    QtAV::AVPlayer *m_player=Q_NULLPTR;
    qreal brightness=0,contrast=0,hue=0,saturation=0;

    QSize initSize;
    QVector <QOpenGLShaderProgram *> programs;
    QVector <QOpenGLFramebufferObject *> m_fbo;
    int pass=0;
    int maxPass=0,maxPass_adaptive_sahrpen=0;
    QVector <int> scales;
    GeometryEngine *geometries=Q_NULLPTR;
    QStringList shader_files,shader_vertex_files,shader_files_adaptive_sharpen,shader_files_fxaa;
    QString shader_files_prefix,shader_files_include;
    QOpenGLTexture *texture=Q_NULLPTR;
    int frame=0;

    virtual GLuint frameTexture() const;
    GLuint lastSuperscaleTexureId=0;

    void superscale(GLfloat opt_sharpness=1.0f, GLfloat opt_edge_strength=0.6f);
    void initFrameBufers();
    void initFrameBufer(int id);
    bool initShaders2(int pass);
    void initTextures();
    int addFBO(int scale, bool rotate);
    int addProgram();
    bool initShaders(int pass);
    bool initShaders_simple(int pass);
    bool initShaders_xbr(int pass);
    bool initShaders_adaptiveSharpen(int pass);
    GLuint sharpShader(GLuint pfbotextid);
    GLuint adaptiveSharpen(GLuint pfbotextid);
    GLuint fxaaShader(GLuint pfbotextid);
};
#endif // XUNOGLSLFILTER_H
