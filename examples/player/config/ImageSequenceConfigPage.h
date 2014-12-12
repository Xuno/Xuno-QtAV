#ifndef IMAGESEQUENCECONFIGPAGE_H
#define IMAGESEQUENCECONFIGPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QFileInfo>


class QPushButton;
class QSpinBox;

class ImageSequenceConfigPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSequenceConfigPage(QWidget *parent = 0);
    void openFiles();
    void playImgages();

signals:
    void play(QString);
    void repeatAChanged(QTime);
    void repeatBChanged(QTime);
    void toggleRepeat(bool);

public slots:
    void onSelectImgages();
private slots:
    void setFPS(int n);
    void setTotalFrames(int n);

private:
    QSpinBox *mpFpsBox, *mpTotalFramesBox;
    QPushButton *mpSelectImgButton, *mpPlayImgButton;
    quint64 startPos,stopPos;
    int fps;
    quint32 startFrame, frames;
    QFileInfo fileinfo;

    void calculatePos();
    void analyzeFilename();
    int getDigetsFilename();

};

#endif // IMAGESEQUENCECONFIGPAGE_H
