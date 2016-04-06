#include "ShaderFilterXuno.h"

#include <QVector>
#include <QVariant>
#include <QDebug>

ShaderFilterXuno::ShaderFilterXuno()
{
    qDebug()<<"constructor ShaderFilterXuno::ShaderFilterXuno";
    setProperty("u_pixeloffsetkernel", QVariant::fromValue(QVector<float>(18) << 0.4f ));
    setProperty("u_filterkernel", QVariant::fromValue(QVector<float>(9) << 0.5f ));
    setProperty("u_gammaRGB", QVariant::fromValue(2.2f));
}

void ShaderFilterXuno::setSharpValue(float v)
{
    setProperty("u_pixeloffsetkernel", QVariant::fromValue(QVector<float>(18) << v ));
    setProperty("u_filterkernel", QVariant::fromValue(QVector<float>(9) << v ));
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
                \nuniform float u_filterkernel[9];
                \nuniform float u_gammaRGB;
                \nuniform vec2 u_pixeloffsetkernel[9];
                \n#define USED_GAMMA
                );
}
//                \n#define USED_FILTERS

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
                \n       c = texture(tex, pos+u_pixeloffsetkernel[i]);
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

void ShaderFilterXuno::setUserUniformValue(Uniform &u)
{
    if (u.name==QByteArray("u_gammaRGB")){
        u.set(u_gammaRGB);
    }else if(u.name==QByteArray("u_filterkernel")){
        //u.set(u_filterkernel,9);
    }else if(u.name==QByteArray("u_pixeloffsetkernel")){
        //u.set(u_pixeloffsetkernel,9);
    }
}

bool ShaderFilterXuno::setUserUniformValues()
{
    //Uniform.set(const T& value, int count);
    GLfloat fs=(GLfloat)filterSharp;
    GLfloat fsa=(GLfloat)-((fs-(qreal)1.0)/(qreal)4.0);

    GLfloat kernel[9] =  {
                 0.,fsa ,0.,
               fsa, fs ,fsa,
                 0.,fsa ,0.
     };

    QVector2D pix=pixeloffset();
    QVector2D pixeloffsetkernel[9] =  {
        QVector2D(  -pix.x()   , -pix.y()  ),
        QVector2D(   0.0	   , -pix.y()  ),
        QVector2D(   pix.x()   , -pix.y()  ),
        QVector2D(  -pix.x()   ,  0.0      ),
        QVector2D(   0.0	   ,  0.0      ),
        QVector2D(   pix.x()   ,  0.0      ),
        QVector2D(  -pix.x()   ,  pix.y()  ),
        QVector2D(   0.0	   ,  pix.y()  ),
        QVector2D(   pix.x()   ,  pix.y()  )
     };

    //qDebug()<<"ShaderFilterXuno::setUserUniformValues() u_gammaRGB"<<u_gammaRGB;
    program()->setUniformValue("u_gammaRGB", u_gammaRGB);
    program()->setUniformValueArray("u_pixeloffsetkernel", pixeloffsetkernel,9); //pixeloffsetkernel
    program()->setUniformValueArray("u_filterkernel", kernel, 9);                //filterkernel
/*    program()->setUniformValue("u_pix", u_pix); */                          //pixeloffset


    return true;

}

//QVariant ShaderFilterXuno::getu_pixeloffsetkernel() const
//{
//    return u_pixeloffsetkernel;
//}

//void ShaderFilterXuno::setu_pixeloffsetkernel(const QVariant &value)
//{
//    u_pixeloffsetkernel = value;
//}

//QVariant ShaderFilterXuno::getu_filterkernel() const
//{
//    return u_filterkernel;
//}

//void ShaderFilterXuno::setu_filterkernel(const QVariant &value)
//{
//    u_filterkernel = value;
//}

QVariant ShaderFilterXuno::getU_gammaRGB() const
{
    return QVariant(u_gammaRGB);
}

void ShaderFilterXuno::setU_gammaRGB(const QVariant &value)
{
    qDebug()<<"ShaderFilterXuno::setU_gammaRGB"<<value;
    u_gammaRGB = value.toFloat();
}

QVector2D ShaderFilterXuno::pixeloffset () const
{
    //DPTR_D(const VideoMaterial);
    QSize d=QSize(640,480);
    float w=1.f/d.width();
    float h=1.f/d.height();
    return QVector2D(w,h);
}

void ShaderFilterXuno::setGammaRGB(qreal value)
{
    //qDebug("VideoShader.cpp VideoMaterial::setGammaRGB: %f",value);
    //0-2
    const qreal min=0.,max=2.,w=(max-min)/2.,of=w+min;
    qreal g = ((qreal)value)*w+of;
    g=(g>0)?g:0.000001; // prevent values less than 0
    //qDebug("VideoShader.cpp VideoMaterial::setGammaRGB (corr): %f",g);
    setU_gammaRGB(g);
}

void ShaderFilterXuno::setFilterSharp(qreal value)
{
    //qDebug("VideoShader.cpp VideoMaterial::setFilterSgarp: %f",value);
    //1-10  W=MAX-MIN,   fs=(value/100.0)*w/2+w+min
    const qreal min=1.,max=10.,w=(max-min)/2.,of=w+min;
    qreal fs=((qreal)value)*w+of;
    //qDebug("VideoShader.cpp VideoMaterial::setFilterSgarp: (corr) %f",fs);
    filterSharp=fs;
}


const char *ShaderFilterXuno::userPostProcess() const
{
    return GLSL(
                \n#ifdef USED_GAMMA
                \ngl_FragColor.rgb = pow(gl_FragColor.rgb, 1.0 / vec3(u_gammaRGB));
                \n#endif //USED_GAMMA
            );
}

