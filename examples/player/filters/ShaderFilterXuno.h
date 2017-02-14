#ifndef SHADERSHARPVIDEO_H
#define SHADERSHARPVIDEO_H
#include <QVariant>
#include <QtAV>
#include <QtAV/VideoShaderObject.h>

using namespace QtAV;
#define GLSL(x) #x ""

class ShaderFilterXuno : public QtAV::VideoShader
{
    //Q_OBJECT
    //Q_PROPERTY(QVariant u_pixeloffsetkernel READ getu_pixeloffsetkernel WRITE setu_pixeloffsetkernel NOTIFY u_pixeloffsetkernelChanged)
    //Q_PROPERTY(QVariant u_filterkernel READ getu_filterkernel WRITE setu_filterkernel NOTIFY u_filterkernelChanged)
    //Q_PROPERTY(QVariant u_gammaRGB READ getU_gammaRGB WRITE setU_gammaRGB NOTIFY u_gammaRGBChanged)
public:
    ShaderFilterXuno();
    void setSharpValue(float v);
    void setGammaValue(float v);
    void setCustomProgram(QOpenGLShaderProgram *value);
    QString compile();

private:
    const char* userShaderHeader(QOpenGLShader::ShaderType type) const;
    const char* userPostProcess() const;
    const char* userSample() const;
    QOpenGLShaderProgram* program();
    QOpenGLShaderProgram* customProgram=Q_NULLPTR;

    void setUserUniformValue(Uniform &u);
    bool setUserUniformValues();

    GLfloat u_gammaRGB=1.;
    qreal filterSharp=1.;

    const QVector2D pixeloffsetkernel[9] =  {
       QVector2D(  -1.0   , -1.0  ),
       QVector2D(   0.0   , -1.0  ),
       QVector2D(   1.0   , -1.0  ),
       QVector2D(  -1.0   ,  0.0  ),
       QVector2D(   0.0   ,  0.0  ),
       QVector2D(   1.0   ,  0.0  ),
       QVector2D(  -1.0   ,  1.0  ),
       QVector2D(   0.0   ,  1.0  ),
       QVector2D(   1.0   ,  1.0  )
    };



};

#endif // SHADERSHARPVIDEO_H
