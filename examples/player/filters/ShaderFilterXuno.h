#ifndef SHADERSHARPVIDEO_H
#define SHADERSHARPVIDEO_H
#include <QVariant>
#include <QtAV>
#include <QtAV/VideoShaderObject.h>

using namespace QtAV;
#define GLSL(x) #x ""

class ShaderFilterXuno : public QtAV::VideoShaderObject
{
    Q_OBJECT
    //Q_PROPERTY(QVariant u_pixeloffsetkernel READ getu_pixeloffsetkernel WRITE setu_pixeloffsetkernel NOTIFY u_pixeloffsetkernelChanged)
    //Q_PROPERTY(QVariant u_filterkernel READ getu_filterkernel WRITE setu_filterkernel NOTIFY u_filterkernelChanged)
     Q_PROPERTY(QVariant u_gammaRGB READ getU_gammaRGB WRITE setU_gammaRGB NOTIFY u_gammaRGBChanged)
public:
    ShaderFilterXuno();
    void setSharpValue(float v);
    void setGammaValue(float v);
    QVariant getU_gammaRGB() const;
    void setU_gammaRGB(const QVariant &value);

//    QVariant getu_filterkernel() const;
//    void setu_filterkernel(const QVariant &value);

//    QVariant getu_pixeloffsetkernel() const;
//    void setu_pixeloffsetkernel(const QVariant &value);

signals:
    void u_pixeloffsetkernelChanged(QVariant);
    void u_filterkernelChanged(QVariant);
    void u_gammaRGBChanged(QVariant);

private:
    const char* userShaderHeader(QOpenGLShader::ShaderType type) const;
    const char* userPostProcess() const;
    const char* userSample() const;
    void setUserUniformValue(Uniform &u);
    bool setUserUniformValues();

    GLfloat u_gammaRGB;
    QVector <GLfloat> u_filterkernel;
    QVector <QVector2D> u_pixeloffsetkernel;
    QVector2D pixeloffset() const;
    void setGammaRGB(qreal value);
    void setFilterSharp(qreal value);

    qreal filterSharp;

};

#endif // SHADERSHARPVIDEO_H
