#include "ShaderFilterXuno.h"

#include <QVector>
#include <QVariant>
#include <QDebug>

ShaderFilterXuno::ShaderFilterXuno()
{
    qDebug()<<"constructor ShaderFilterXuno::ShaderFilterXuno";
}

void ShaderFilterXuno::setSharpValue(float v)
{
    //qDebug("VideoShader.cpp VideoMaterial::setFilterSgarp: %f",value);
    //1-10  W=MAX-MIN,   fs=(value/100.0)*w/2+w+min
    const qreal min=1.,max=10.,w=(max-min)/2.,of=w+min;
    qreal fs=((qreal)v)*w+of;
    //qDebug("VideoShader.cpp VideoMaterial::setFilterSgarp: (corr) %f",fs);
    filterSharp=fs;
}

void ShaderFilterXuno::setGammaValue(float v)
{
    qDebug("ShaderFilterXuno::setGammaValue: %f",v);
    //0-2
    const qreal min=0.,max=2.,w=(max-min)/2.,of=w+min;
    qreal g = ((qreal)v)*w+of;
    g=(g>0)?g:0.000001; // prevent values less than 0
    qDebug("ShaderFilterXuno::setGammaValue (corr): %f",g);
    u_gammaRGB=g;
}


const char *ShaderFilterXuno::userShaderHeader(QOpenGLShader::ShaderType type) const
{
    if (type == QOpenGLShader::Vertex)
        return 0;

    return GLSL(\n
                \nuniform float u_gammaRGB;
                \nuniform float u_filterkernel[9];
                \nuniform vec2 u_pixeloffsetkernel[9];
                \n#define USED_GAMMA
                \n#define USED_FILTERS
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
                \n       c = texture(tex, pos+u_texelSize[plane]*u_pixeloffsetkernel[i]);
                \n       if (i==4) a=c.a;
                \n      sum +=  c.rgb * u_filterkernel[i];
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

const char *ShaderFilterXuno::userPostProcess() const
{
    return GLSL(
                \n#ifdef USED_GAMMA
                \ngl_FragColor.rgb = pow(gl_FragColor.rgb, 1.0 / vec3(u_gammaRGB));
                \n#endif //USED_GAMMA
            );
}

void ShaderFilterXuno::setUserUniformValue(Uniform &u)
{
    if (u.name==QByteArray("u_gammaRGB")){
        u.set(u_gammaRGB);
    }else if(u.name==QByteArray("u_filterkernel")){
        //u.set(/*(const float &)*/u_filterkernel);
    }else if(u.name==QByteArray("u_pixeloffsetkernel")){
        //u.set(/*(const float &)*/u_pixeloffsetkernel);
    }
}

bool ShaderFilterXuno::setUserUniformValues()
{
    //Uniform.set(const T& value, int count);
    GLfloat fs=(GLfloat)filterSharp;
    GLfloat fsa=(GLfloat)-((fs-(qreal)1.0)/(qreal)4.0);

    GLfloat filterkernel[9] =  {
                 0.,fsa ,0.,
               fsa, fs ,fsa,
                 0.,fsa ,0.
     };

    program()->setUniformValue("u_gammaRGB", u_gammaRGB);
    program()->setUniformValueArray("u_pixeloffsetkernel", pixeloffsetkernel,9);
    program()->setUniformValueArray("u_filterkernel", filterkernel, 9,1);
    return true;
}





