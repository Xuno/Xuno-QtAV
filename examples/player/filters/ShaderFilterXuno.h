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
    Q_PROPERTY(QVariant Xu_pixeloffsetkernel READ getXu_pixeloffsetkernel WRITE setXu_pixeloffsetkernel NOTIFY Xu_pixeloffsetkernelChanged)
    Q_PROPERTY(QVariant Xu_filterkernel READ getXu_filterkernel WRITE setXu_filterkernel NOTIFY Xu_filterkernelChanged)
    Q_PROPERTY(QVariant u_gammaRGB READ getU_gammaRGB WRITE setU_gammaRGB NOTIFY u_gammaRGBChanged)
public:
    ShaderFilterXuno();
    void setSharpValue(float v);
    void setGammaValue(float v);
    QVariant getU_gammaRGB() const;
    void setU_gammaRGB(const QVariant &value);

    QVariant getXu_filterkernel() const;
    void setXu_filterkernel(const QVariant &value);

    QVariant getXu_pixeloffsetkernel() const;
    void setXu_pixeloffsetkernel(const QVariant &value);

signals:
    void Xu_pixeloffsetkernelChanged(QVariant);
    void Xu_filterkernelChanged(QVariant);
    void u_gammaRGBChanged(QVariant);

private:
    const char* userShaderHeader(QOpenGLShader::ShaderType type) const;
    const char* userPostProcess() const;
    const char* userSample() const;

    QVariant u_gammaRGB;
    QVariant Xu_filterkernel;
    QVariant Xu_pixeloffsetkernel;

};

#endif // SHADERSHARPVIDEO_H
