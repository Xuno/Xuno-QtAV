#ifndef SAVEGL_H
#define SAVEGL_H
#include <QDebug>
#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QtAv>
#include <QtAV/AVPlayer.h>
#include <Config.h>


class SaveGLXuno : public QObject
{
    Q_OBJECT
public:
    explicit SaveGLXuno(QObject *parent = 0);

    void setPlayer(QtAV::AVPlayer *player);

signals:

public slots:
    void saveimg();

private:
    void defineFileName();
    void saveFromGLContext();
    void saveFromRender();
    bool releasePixels();
    uchar *pixels=Q_NULLPTR;
    GLuint max_fbo=0;
    int imageW,imageH,imageColors,imageDepth;
    int wOutputMedia,hOutputMedia,imageChannelsOutputMedia=4,imageDepthOutputMedia=16;
    GLenum formatOutputMedia, typeOutputMedia;
    bool imageFloatFormat;
    qreal imageScale=1.;
    bool readyInputMedia=false;
    bool readyOutputMedia=false;
    QString lastfilename="", dir, fmt;


    QtAV::AVPlayer *m_player=Q_NULLPTR;

};

#endif // SAVEGL_H
