#ifndef IMAGESEQUENCECONFIGPAGE_H
#define IMAGESEQUENCECONFIGPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QFileInfo>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCore>
#include <QRegularExpression>
#include <QDialogButtonBox>
#include <QCheckBox>


class QPushButton;
class QSpinBox;

class ImageSequenceConfigPage : public QDialog
{
    Q_OBJECT
public:
    explicit ImageSequenceConfigPage(QWidget *parent = 0);
    void openFiles();
    void playImgages();
    void setMovieDuration(qint64 d);
    void setStartFrame(quint32 n);
    void setEndFrame(quint32 n);


signals:
    void play(QString);
    void stop();
    void repeatAChanged(QTime);
    void repeatBChanged(QTime);
    void toggleRepeat(bool);
    void customfpsChanged(double);


public slots:
    void onSelectImgages();
    void setFPS(double n);
    void setTotalFrames(int n);

private slots:
    void on_buttonPlay_clicked();

private:
    QSpinBox *mpTotalFramesBox;
    QDoubleSpinBox *mpFpsBox;
    QPushButton *mpSelectImgButton, *mpPlayImgButton;
    quint64 startPos,stopPos;
    double fps;
    quint32 startFrame, frames;
    QFileInfo fileinfo;

    void calculatePos();
    void analyzeFilename();
    int getDigetsFilename();
    bool playing_start;

    const QStringList ImageTypes = QStringList()
    <<  "bmp"
    <<  "cr2"
    <<  "dng"
    <<  "dxp"
    <<  "exr"
    <<  "jp2"
    <<  "png"
    <<  "jpg"
    <<  "tga"
    <<  "tif"
    <<  "tiff";

};

#endif // IMAGESEQUENCECONFIGPAGE_H
