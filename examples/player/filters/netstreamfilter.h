#ifndef NETSTREAMFILTER_H
#define NETSTREAMFILTER_H

#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>
#include <QtAV/Statistics.h>
#include <QtAV/VideoFrame.h>
#include <QtAV/AVPlayer.h>
#include <QtAV/VideoRenderer.h>
#include "netstreamserver.h"
#include "runmpvpipe.h"


class NetStreamFilter : public QtAV::VideoFilter
{
    Q_OBJECT
public:
    NetStreamFilter(QObject *parent);
    ~NetStreamFilter();
    bool isSupported(QtAV::VideoFilterContext::Type ct) const {
        return ct == QtAV::VideoFilterContext::QtPainter || ct == QtAV::VideoFilterContext::X11;
    }
    void setMpvPipe(runmpvpipe *value);
    void customEvent(QEvent *event);
    bool event(QEvent *e);
    void setPlayer(QtAV::AVPlayer *player);

signals:
    void onSentFrame();

public slots:
    void onStarted();


public:
    void process(QtAV::Statistics* statistics, QtAV::VideoFrame* frame);

protected:
    QObject *m_parent;
    QtAV::AVPlayer *m_player=0;

    void onPostDraw();
    bool firstFrame;
    qint64 frames=0;


private:
    NetStreamServer *Nss=0;
    runmpvpipe *mpvpipe=0;
    QByteArray buffer;


};

#endif // NETSTREAMFILTER_H
