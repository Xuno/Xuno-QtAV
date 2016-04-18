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
    int OutputMediaWidth,OutputMediaHeight,imageOutputMediaChannels=4,imageOutputMediaDepth=16;
    GLenum OutputMediaFormat, OutputMediaType;
    bool imageFloatFormat;
    qreal imageScale=1.;
    bool readyInputMedia=false;
    bool readyOutputMedia=false;
    QString lastfilename="", dir, fmt;


    QtAV::AVPlayer *m_player=Q_NULLPTR;

    void getTextureParametersFBO(const GLint fbo, int *width, int *height, GLint *format=Q_NULLPTR);
    bool getTextureParameters(const GLuint id, int *width, int *height, int *format);
    GLuint findLastFBO();
    QString convertInternalFormatToString(GLenum format);
    QString gl_error(GLenum err);
};

#endif // SAVEGL_H
