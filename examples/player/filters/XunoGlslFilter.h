#ifndef XUNOGLSLFILTER_H
#define XUNOGLSLFILTER_H
#include <QtAV>
#include <QtAV/GLSLFilter.h>
#include <QtAV/OpenGLVideo.h>


class XunoGLSLFilter : public QtAV::GLSLFilter
{
public:
    XunoGLSLFilter(QObject* parent = 0);

protected slots:
    void afterRendering();

};

#endif // XUNOGLSLFILTER_H
