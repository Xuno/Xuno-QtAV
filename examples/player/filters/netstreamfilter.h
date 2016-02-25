#ifndef NETSTREAMFILTER_H
#define NETSTREAMFILTER_H

#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>
#include <QtAV/Statistics.h>
#include <QtAV/VideoFrame.h>

class NetStreamFilter : public QtAV::VideoFilter
{
public:
    NetStreamFilter(QObject *parent);
    bool isSupported(QtAV::VideoFilterContext::Type ct) const {
        return ct == QtAV::VideoFilterContext::QtPainter || ct == QtAV::VideoFilterContext::X11;
    }
protected:
    void process(QtAV::Statistics* statistics, QtAV::VideoFrame* frame);
    bool firstFrame;
    qint64 frames=0;

};

#endif // NETSTREAMFILTER_H
