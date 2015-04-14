#ifndef IMGSEQEXTRACTCONTROL_H
#define IMGSEQEXTRACTCONTROL_H

#include <QtCore/QVariant>
#include <QDebug>
#include <QTime>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTime>
#include "Button.h"

class Button;

class ImgSeqExtractControl : public QWidget
{
    Q_OBJECT
public:
    explicit ImgSeqExtractControl(QWidget *parent=0);
    ~ImgSeqExtractControl();

    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void setStartTime(QTime time);
    void setEndTime(QTime time);
    void setTotalFrames(int frames);
    void setStartFrame(int frame);
    void setEndFrame(int frame);
    int getTotalFrames() const;
    int getStartFrame() const;
    int getEndFrame() const;
    void calcStartFrame();
    void calcStartFrame(QTime t);
    void calcEndFrame();
    void calcEndFrame(QTime t);
    void calcTotalFrames();
    void setFPS(float fps);
    qint64 StartPosExtract();
    qint64 EndPosExtract();
    bool regionPlaying();

public slots:
    void onPaused();

signals:
    void seek(QTime time);
    void pause();
    void onStartPlay();
    void togglePlayPause();
    void toggleRepeat(bool);
    void RepeatLoopChanged(int);
    void repeatAChanged(const QTime &time);
    void repeatBChanged(const QTime &time);
    void setTimeSliderVisualMinLimit(const QTime &time);
    void setTimeSliderVisualMaxLimit(const QTime &time);



private:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hlStart_6;
    QLabel *startTime;
    QSpacerItem *hs_start;
    QLabel *labelSF;
    QSpinBox *ImageSequenceStartFrame;
    QToolButton *buttonSetStartFrame;
    QToolButton *buttonSetStartSeekFrame;
    QSpacerItem *hs_total;
    QLabel *labelTF;
    QSpinBox *ImageSequenceTotalFrame;
    QSpacerItem *hs_end;
    QLabel *labelEF;
    QSpinBox *ImageSequenceEndFrame;
    QToolButton *buttonSetEndFrame;
    QToolButton *buttonSetEndSeekFrame;
    QSpacerItem *horizontalSpacer_10;
    QLabel *endTime;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *btSelectOutputPath;
    QLineEdit *OutputPath;
    QComboBox *cb_OutputType;
    QComboBox *cbColorTypeOutput;
    QHBoxLayout *hlPrefix_3;
    QLabel *labelFilePrefix;
    QLineEdit *OutputFileNumberingPrefix;
    QLabel *labelFileSeparator;
    QLineEdit *OutputFileNumberingSeperator;
    QPushButton *buttonExtractFrames;
    QPixmap mPlayPixmap;
    QPixmap mPausePixmap;
    Button *mpPlayPauseBtn;

    const QString timeFormat ="hh:mm:ss.zzz";
    QTime isStartTime,isEndTime;
    qint64 isStartPosExtract, isEndPosExtract;
    bool isRegionPlaying;
    float isFPS;
    void retranslateUi();
    int baseParentMaxHeight;
    void RecalculateParentHeight(bool visible);
    QTime frameToTime(int f);
    int timeToFrame(QTime t);

private slots:
    void on_buttonSetStartFrame_clicked(bool state);
    void on_buttonSetEndFrame_clicked(bool state);
    void on_buttonSetStartSeekFrame_clicked();
    void on_buttonSetEndSeekFrame_clicked();
    void on_ImageSequenceEndFrame_valueChanged(int i);
    void on_ImageSequenceStartFrame_valueChanged(int i);
    void on_ImageSequenceTotalFrame_valueChanged(int i);
    void on_mpPlayPauseBtn_clicked();
    void on_btSelectOutputPath_clicked();
};

#endif // IMGSEQEXTRACTCONTROL_H
