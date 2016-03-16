#ifndef RUNMPVPIPE_H
#define RUNMPVPIPE_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QThread>
#include <QWidget>
#include <QDir>

class runmpvpipe : public QObject
{
    Q_OBJECT
public:
    explicit runmpvpipe(QByteArray * _buffer=0, QObject *parent = 0);
    ~runmpvpipe();
    void  sendFrame(const int &w, const int &h);
    bool runApp();
    void setFameInfo(QSize fsize, qreal _fps=25.);
    void setFameInfo(int w, int h, qreal fps=25.);
    void setBuffer(QByteArray *value);
    void closeApp();
    QWidget *moveMpvApp();
    bool presentMpvApp();
    void setWidget(QWidget *&widget);
    bool getMovedApp() const;

Q_SIGNALS:
    void ready();
    void finished(int);
    void error();

public slots:

private slots:
    void started();

private:
    QByteArray *buffer=0;
    QProcess *mpvprocess=0;
    QString mpvAppEXE="mpv";
    const QString mpvAppArguments="mpv.com";
    int frameW=0;
    int frameH=0;
    int bytesPerPixel=4;
    qreal fps=25.;
    qint64 mpvprocessID=0;
    QThread *MpvT=0;
    FILE* file=NULL;
    WId findAppWindow(QString ext="exe");
    QWidget *mpvwidget=nullptr;
    bool movedApp=false;



};

#endif // RUNMPVPIPE_H
