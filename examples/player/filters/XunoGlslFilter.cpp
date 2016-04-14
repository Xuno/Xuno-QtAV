#include "XunoGlslFilter.h"
#include <QDebug>
#include <QImage>

XunoGLSLFilter::XunoGLSLFilter(QObject *parent):
    QtAV::GLSLFilter(parent)
{
    qDebug()<<"constructor XunoGLSLFilter::XunoGLSLFilter";
    if (opengl()){
        qDebug()<<"connect XunoGLSLFilter::XunoGLSLFilter";
        connect(opengl(), &QtAV::OpenGLVideo::afterRendering, this, &XunoGLSLFilter::afterRendering, Qt::DirectConnection);
    }
}

void XunoGLSLFilter::afterRendering()
{
    qDebug()<<"XunoGLSLFilter::afterRendering()";
    if (fbo() && fbo()->isValid()) {
          qDebug()<<"XunoGLSLFilter::afterRendering() fbo size()"<<fbo()->size();
          fbo()->bind();
          //...
          //opengl()->shader->program();
          //...
          fbo()->release();
          //SAVE FBO for text result;
          QMatrix matrix = QMatrix().scale(1,-1); //OpenGL rotate;
          QImage s=fbo()->toImage();
          s.transformed(matrix).save(QString("SaveFBOafterRendering-%1.tif").arg(QDateTime().currentMSecsSinceEpoch()));

    }
}


