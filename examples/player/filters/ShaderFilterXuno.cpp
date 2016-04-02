#include "ShaderFilterXuno.h"

#include <QVector>
#include <QVariant>
#include <QDebug>

ShaderFilterXuno::ShaderFilterXuno()
{
    qDebug()<<"constructor ShaderFilterXuno::ShaderFilterXuno";
    setProperty("Xu_pixeloffsetkernel", QVariant::fromValue(QVector<float>(18) << 0.4f ));
    setProperty("Xu_filterkernel", QVariant::fromValue(QVector<float>(9) << 0.5f ));
    setProperty("u_gammaRGB", QVariant::fromValue(0.1f));
}

void ShaderFilterXuno::setSharpValue(float v)
{
    setProperty("Xu_pixeloffsetkernel", QVariant::fromValue(QVector<float>(18) << v ));
    setProperty("Xu_filterkernel", QVariant::fromValue(QVector<float>(9) << v ));
}

void ShaderFilterXuno::setGammaValue(float v)
{
    setProperty("u_gammaRGB", QVariant::fromValue(v));
}


const char *ShaderFilterXuno::userShaderHeader(QOpenGLShader::ShaderType type) const
{
    if (type == QOpenGLShader::Vertex)
        return 0;

    return GLSL(\n
                \nuniform float Xu_filterkernel[9];
                \nuniform vec2 Xu_pixeloffsetkernel[9];
                \nuniform float u_gammaRGB;
                \n#define USED_FILTERS
                \n#define USED_GAMMA
                );
}

const char *ShaderFilterXuno::userSample() const
{
    return GLSL(
                \nvec4 sample2d(sampler2D tex, vec2 pos, int plane)
                \n{
                \n#ifdef USED_FILTERS
                \n    vec3 sum = vec3(0.0);
                \n    vec4 c;
                \n    float a;
                \n    for (int i=0;i<9;i++) {
                \n       c = texture(tex, pos+Xu_pixeloffsetkernel[i]);
                \n       if (i==4) a=c.a;
                \n      sum +=  c.rgb * Xu_filterkernel[i];
                \n     }
                \n    c.rgb=sum.rgb;
                \n    c.a=a;
                \n    return c;
                \n#else
                \n    return texture(tex, pos);
                \n#endif
                \n}
                );
}

QVariant ShaderFilterXuno::getXu_pixeloffsetkernel() const
{
    return Xu_pixeloffsetkernel;
}

void ShaderFilterXuno::setXu_pixeloffsetkernel(const QVariant &value)
{
    Xu_pixeloffsetkernel = value;
}

QVariant ShaderFilterXuno::getXu_filterkernel() const
{
    return Xu_filterkernel;
}

void ShaderFilterXuno::setXu_filterkernel(const QVariant &value)
{
    Xu_filterkernel = value;
}

QVariant ShaderFilterXuno::getU_gammaRGB() const
{
    return u_gammaRGB;
}

void ShaderFilterXuno::setU_gammaRGB(const QVariant &value)
{
    u_gammaRGB = value;
}

const char *ShaderFilterXuno::userPostProcess() const
{
    return GLSL(
                \n#ifdef USED_GAMMA
                \ngl_FragColor.rgb = pow(gl_FragColor.rgb, 1.0 / vec3(u_gammaRGB));
                \n#endif //USED_GAMMA
            );
}

