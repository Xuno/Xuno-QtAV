/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "version.h"
#include <QUrl>
#include "config/configwebmemu.h"
#include "config/ImageSequenceConfigPage.h"
#include "config/ImageSequenceExtractControl.h"
#include "filters/runmpvpipe.h"


QT_BEGIN_NAMESPACE
class QWidgetAction;
class QToolButton;
QT_END_NAMESPACE
namespace QtAV {
class AudioOutput;
class AVError;
class AVPlayer;
class AVClock;
class VideoRenderer;
class LibAVFilterAudio;
class LibAVFilterVideo;
class SubtitleFilter;
class VideoPreviewWidget;
}
QT_BEGIN_NAMESPACE
class QMenu;
class QTimeEdit;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QTimeEdit;
QT_END_NAMESPACE
class Button;
class Slider;
class PlayList;
class DecoderConfigPage;
class VideoEQConfigPage;
class StatisticsView;
class OSDFilter;
class AVFilterSubtitle;
class Preview;
class ImageSequenceConfigPage;
class AdvancedFilter;


class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setAudioBackends(const QStringList& backends);
    bool setRenderer(QtAV::VideoRenderer* renderer);
    void setVideoDecoderNames(const QStringList& vd);
    void loadRemoteUrlPresset(const QString& url);
    void setStartSequenceFrame(const quint32 sf);
    void setEndSequenceFrame(const quint32 ef);
    void setFpsSequenceFrame(const double fps);
    void setRepeatLoop(const bool loop);
    void setFileName(const QString fname);
    void setExtractTask(const QString fname);

    QString XUNO_QtAV_Version_String();
    QString XUNO_QtAV_Version_String_Long();
public slots:
    void play(const QString& name);
    void play(const QUrl& url);
    void play();
    void pause();
    void openFile();
    void togglePlayPause();
    void showNextOSD();
    void setPlayerScale(const double scale);

signals:
    void ready();

private slots:
    void stopUnload();
    void about();
    void help();
    void openUrl();
    void initAudioTrackMenu();
    void updateChannelMenu();
    void switchAspectRatio(QAction* action);
    void toggleRepeat(bool);
    void setRepeateMax(int);
    void changeVO(QAction* action);
    void changeChannel(QAction* action);
    void changeAudioTrack(QAction* action);
    void onTVMenuClick();
    void playOnlineVideo(QAction *action);
    void onPlayListClick(const QString& key, const QString& value);
    void processPendingActions();
    void initPlayer();
    void setupUi();
    void onSpinBoxChanged(double v);
    void onStartPlay();
    void onStopPlay();
    void onPaused(bool p);
    void onSpeedChange(qreal speed);
    void seek();
    void seek(qint64 msec);
    void seek(QTime time);
    void seek(int);
    void showHideVolumeBar();
    void setVolume();
    void tryHideControlBar();
    void tryShowControlBar();
    void showInfo();
    void onPositionChange(qint64 pos);
    void repeatAChanged(const QTime& t);
    void repeatBChanged(const QTime& t);
    void setTimeSliderVisualMinLimit(const QTime &t);
    void setTimeSliderVisualMaxLimit(const QTime &t);

    void onTimeSliderHover(int pos, int value);
    void onTimeSliderLeave();
    void handleError(const QtAV::AVError& e);
    void onMediaStatusChanged();
    void onBufferProgress(qreal percent);

    void onVideoEQEngineChanged();
    void onBrightnessChanged(int b);
    void onContrastChanged(int c);
    void onHueChanged(int h);
    void onSaturationChanged(int s);
    void onGammaRGBChanged(int s);
    void onFilterSharpChanged(int s);

    void onSeekFinished(qint64 pos);
    void onCaptureConfigChanged();
    void onPreviewEnabledChanged();
    void onAVFilterVideoConfigChanged();
    void onAVFilterAudioConfigChanged();
    void onBufferValueChanged();
    void onAbortOnTimeoutChanged();

    void donate();
    void setup();

    void handleFullscreenChange();
    void onFullScreen();
    void toggoleSubtitleEnabled(bool value);
    void toggleSubtitleAutoLoad(bool value);
    void openSubtitle();
    void setSubtitleCharset(const QString& charSet);
    void setSubtitleEngine(const QString& value);

    void onClickXunoBrowser(QUrl url);
    void customfpsChanged(double n);
    void changeClockType(QAction* action);
    void setClockType();
    void RepeatLoopChanged(int i);
    void syncVolumeUi(qreal value);
    void onImageSequenceConfig();
    void onImageSequenceToogledFrameExtractor(bool state);
    void runMpvPlayer();
    void runMpvPlayerStop();
    void runMpvPlayerRunned();
    void runMpvPlayerFinished(int c);
    void advacedFilterSentFrame();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);


private:
    void workaroundRendererSize();
    void reSizeByMovie();
    void setPlayerPosFromRepeat();
    void tuneRepeatMovieDuration();
    bool isFileImgageSequence();
    bool applyCustomFPS();
    void analyeUsedFPS();
    void installAdvancedFilter();



private:
    bool mIsReady, mHasPendingPlay;
    bool mControlOn;
    int mCursorTimer;
    int mShowControl; //0: can hide, 1: show and playing, 2: always show(not playing)
    int mRepeateMax;
    double mCustomFPS;
    double mPlayerScale;
    QStringList mAudioBackends;
    QVBoxLayout *mpPlayerLayout;

    QWidget *detachedControl=0;
    QVBoxLayout *detachedControlLayout=0;
    QWidget *mpControl;
    QLabel *mpCurrent, *mpEnd;
    QLabel *mpTitle;
    QLabel *mpSpeed;
    Slider *mpTimeSlider, *mpVolumeSlider;
    QToolButton *mpWebBtn, *mpFullScreenBtn;
    QToolButton *mpVolumeBtn;
    QToolButton *mpPlayPauseBtn;
    QToolButton *mpStopBtn, *mpForwardBtn, *mpBackwardBtn;
    QToolButton *mpOpenBtn;
    QToolButton *mpInfoBtn, *mpMenuBtn, *mpSetupBtn, *mpCaptureBtn;
    QMenu *mpMenu;
    QAction *mpVOAction, *mpARAction; //remove mpVOAction if vo.id() is supported
    QAction *mpRepeatEnableAction;
    QCheckBox *mpRepeatLoop;
    QWidgetAction *mpRepeatAction;
    QSpinBox *mpRepeatBox;
    QTimeEdit *mpRepeatA, *mpRepeatB;
    QAction *mpAudioTrackAction;
    QMenu *mpAudioTrackMenu;
    QMenu *mpChannelMenu;
    QMenu *mpClockMenu = 0;
    QActionGroup *mpClockMenuAction = 0;
    QAction *mpChannelAction;
    QList<QAction*> mVOActions;

    QtAV::AVClock *mpClock;
    QtAV::AVPlayer *mpPlayer;
    QtAV::VideoRenderer *mpRenderer;
    QtAV::LibAVFilterVideo *mpVideoFilter;
    QtAV::LibAVFilterAudio *mpAudioFilter;
    QString mFile;
    QString mTitle;

    QLabel *mpPreview;

    DecoderConfigPage *mpDecoderConfigPage;
    VideoEQConfigPage *mpVideoEQ;

    PlayList *mpPlayList, *mpHistory;

    QPointF mGlobalMouse;
    StatisticsView *mpStatisticsView;

    OSDFilter *mpOSD;
    QtAV::SubtitleFilter *mpSubtitle;
    QtAV::VideoPreviewWidget *m_preview;

    QString XUNOserverUrl;
    QString XUNOpresetUrl;

    ImageSequenceConfigPage *mpImageSequence = 0;
    ConfigWebMemu *mpWebMenu = 0;
    ImgSeqExtractControl *mpImgSeqExtract=0;
    QString aboutXunoQtAV_PlainText();
    QString aboutXunoQtAV_HTML();

    AdvancedFilter *mpAdvancedFilter;
    qint64 mpvPlayerPorcessId=0;
    runmpvpipe *mpvpipe=nullptr;
    QWidget *mpvPlayerWindow=nullptr;
    QWidget *mpvPlayerWindow1=nullptr;

};


#endif // MAINWINDOW_H
