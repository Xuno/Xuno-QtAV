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
                \n  if (u_filterkernel[4]!=1.){
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
                \n  }else{
                \n    return texture(tex, pos);
                \n  }
                \n#else
                \n    return texture(tex, pos);
                \n#endif
                \n}
                  );
}

QOpenGLShaderProgram *ShaderFilterXuno::program()
{
    if (customProgram!=Q_NULLPTR){
      return  customProgram;
    }else{
      return QtAV::VideoShader::program();
    }
}

void ShaderFilterXuno::setCustomProgram(QOpenGLShaderProgram *value)
{
    customProgram = value;
}



const char *ShaderFilterXuno::userPostProcess() const
{
    return GLSL(
                \n#ifdef USED_GAMMA
                \nif (u_gammaRGB!=1.)
                \n gl_FragColor.rgb = pow(gl_FragColor.rgb, 1.0 / vec3(u_gammaRGB));
                \n#endif //USED_GAMMA
            );
}

void ShaderFilterXuno::setUserUniformValue(Uniform &u)
{
    if (u.name==QByteArray("u_gammaRGB")){
        u.set(u_gammaRGB);
    }else if(u.name==QByteArray("u_filterkernel")){
        if (filterSharp!=1.){
            GLfloat fs=(GLfloat)filterSharp;
            GLfloat fsa=(GLfloat)-((fs-(qreal)1.0)/(qreal)4.0);

            GLfloat filterkernel[9] =  {
                0.,fsa ,0.,
                fsa, fs ,fsa,
                0.,fsa ,0.
            };

            u.set(filterkernel);
        }
    }else if(u.name==QByteArray("u_pixeloffsetkernel")){
        //u.set((QVector)pixeloffsetkernel.data());
    }
}

GLfloat ShaderFilterXuno::getGammaRGB() const
{
    return u_gammaRGB;
}

bool ShaderFilterXuno::needToRun()
{
    return (u_gammaRGB!=1. || filterSharp!=1.);
}

qreal ShaderFilterXuno::getFilterSharp() const
{
    return filterSharp;
}

bool ShaderFilterXuno::setUserUniformValues()
{
    //qDebug()<<"ShaderFilterXuno :: setUserUniformValues";
    //Uniform.set(const T& value, int count);
        GLfloat fs=(GLfloat)filterSharp;
        GLfloat fsa=(GLfloat)-((fs-(qreal)1.0)/(qreal)4.0);

        GLfloat filterkernel[9] =  {
            0.,fsa ,0.,
            fsa, fs ,fsa,
            0.,fsa ,0.
        };

        program()->setUniformValueArray("u_pixeloffsetkernel", pixeloffsetkernel,9);
        program()->setUniformValueArray("u_filterkernel", filterkernel, 9,1);
        program()->setUniformValue("u_gammaRGB", u_gammaRGB);
    return true;
}

bool ShaderFilterXuno::compile()
{
    QString ret;
    ret.append("#version 130\n\n"
               "uniform sampler2D texture0;\n"
               "varying vec2 texcoord0;\n"
               "uniform vec2 u_texelSize;\n"
               "uniform vec2 u_pixelSize;\n");
    ret.append(userShaderHeader(QOpenGLShader::Fragment));
    ret.append(userSample());
    ret.append("\nvoid main() {\n"
               "gl_FragColor = sample2d(texture0, texcoord0, 0);\n");
    ret.append(userPostProcess());
    ret.append("\n}\n");

    if (program()==Q_NULLPTR) {
        qDebug()<<"ShaderFilterXuno::compile() : program missing";
        return false;
    }

    program()->addShaderFromSourceCode(QOpenGLShader::Fragment, ret);

    if (!program()->link()) {
        qDebug()<<"ShaderFilterXuno::compile() :  build error";
        qDebug()<<program()->log();
        qDebug()<<program()->shaders().at(0)->sourceCode();
        qDebug()<<program()->shaders().at(1)->sourceCode();
        return false;
    }
return true;
}





