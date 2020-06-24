/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

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
#include <QUrl>

#include "version.h"
#include "config/configwebmemu.h"
#include "config/ImageSequenceConfigPage.h"
#include "config/ImageSequenceExtractControl.h"
#include "filters/ShaderFilterXuno.h"
#include "filters/savegl.h"
#include "filters/XunoGlslFilter.h"
#include "filters/XunoSimpleFilter.h"

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
class DynamicShaderObject;
class GLSLFilter;
}
QT_BEGIN_NAMESPACE
class QMenu;
class QTimeEdit;
class QVBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QTimeEdit;
class QCheckBox;
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
class ShaderFilterXuno;
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
    void calcToUseSuperResolution();

    QString XUNO_QtAV_Version_String(bool longstring=true);
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
    void setFrameRate();
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

    void onUserShaderChanged();

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
    void captureGL();
    void onScaleX2Btn();
    void onScaleX15Btn();
    void onScaleX1Btn();

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
    void installShaderXuno();
    void installSaveGL();
    void installGLSLFilter();
    void installSimpleFilter();
    void onScaleBtn(qreal _scale);
    void scaleReset();



private:
    bool mIsReady, mHasPendingPlay;
    bool mControlOn;
    bool mNaitiveScaleOn=false;
    int mCursorTimer;
    int mShowControl; //0: can hide, 1: show and playing, 2: always show(not playing)
    int mRepeateMax;
    double mCustomFPS;
    double mPlayerScale;
    QStringList mAudioBackends;
    QVBoxLayout *mpPlayerLayout;

    QWidget *detachedControl=Q_NULLPTR;
    QVBoxLayout *detachedControlLayout=Q_NULLPTR;
    QWidget *mpControl = Q_NULLPTR;;
    QLabel *mpCurrent, *mpEnd;
    QLabel *mpTitle;
    QLabel *mpSpeed;
    Slider *mpTimeSlider, *mpVolumeSlider;
    QToolButton *mpWebBtn, *mpFullScreenBtn, *mpScaleX2Btn, *mpScaleX15Btn,*mpScaleX1Btn;
    QToolButton *mpVolumeBtn;
    QToolButton *mpPlayPauseBtn;
    QToolButton *mpStopBtn, *mpForwardBtn, *mpBackwardBtn;
    QToolButton *mpOpenBtn = Q_NULLPTR;
    QToolButton *mpInfoBtn = Q_NULLPTR, *mpMenuBtn, *mpSetupBtn, *mpCaptureBtn;
    QMenu *mpMenu = Q_NULLPTR;;
    QAction *mpVOAction = Q_NULLPTR, *mpARAction = Q_NULLPTR; //remove mpVOAction if vo.id() is supported
    QAction *mpRepeatEnableAction = Q_NULLPTR;
    QCheckBox *mpRepeatLoop = Q_NULLPTR;
    QWidgetAction *mpRepeatAction = Q_NULLPTR;
    QSpinBox *mpRepeatBox = Q_NULLPTR;
    QTimeEdit *mpRepeatA, *mpRepeatB;
    QAction *mpAudioTrackAction = Q_NULLPTR;
    QMenu *mpAudioTrackMenu = Q_NULLPTR;
    QMenu *mpChannelMenu = Q_NULLPTR;
    QMenu *mpClockMenu = Q_NULLPTR;
    QActionGroup *mpClockMenuAction = Q_NULLPTR;
    QAction *mpChannelAction = Q_NULLPTR;
    QList<QAction*> mVOActions;

    QtAV::AVClock *mpClock = Q_NULLPTR;
    QtAV::AVPlayer *mpPlayer = Q_NULLPTR;
    QtAV::VideoRenderer *mpRenderer = Q_NULLPTR;
    QtAV::LibAVFilterVideo *mpVideoFilter = Q_NULLPTR;
    QtAV::LibAVFilterAudio *mpAudioFilter = Q_NULLPTR;
    QString mFile;
    QString mTitle;

    QLabel *mpPreview = Q_NULLPTR;

    DecoderConfigPage *mpDecoderConfigPage = Q_NULLPTR;
    VideoEQConfigPage *mpVideoEQ = Q_NULLPTR;

    PlayList *mpPlayList = Q_NULLPTR, *mpHistory = Q_NULLPTR;

    QPointF mGlobalMouse;
    StatisticsView *mpStatisticsView = Q_NULLPTR;

    OSDFilter *mpOSD = Q_NULLPTR;
    QtAV::SubtitleFilter *mpSubtitle = Q_NULLPTR;
    QtAV::VideoPreviewWidget *m_preview = Q_NULLPTR;

    QString XUNOserverUrl;
    QString XUNOpresetUrl;

    ImageSequenceConfigPage *mpImageSequence = Q_NULLPTR;
    ConfigWebMemu *mpWebMenu = Q_NULLPTR;
    ImgSeqExtractControl *mpImgSeqExtract=Q_NULLPTR;
    QString aboutXunoQtAV_PlainText();
    QString aboutXunoQtAV_HTML();

    ShaderFilterXuno *shaderXuno=Q_NULLPTR;
    SaveGLXuno *mSaveGLXuno=Q_NULLPTR;

    XunoGLSLFilter *mpGLSLFilter=Q_NULLPTR;

    QtAV::DynamicShaderObject *m_shader;
    QtAV::GLSLFilter *m_glsl;
    QUrl remove_fistsubdomain(QUrl url);
    bool same_site_domain(const QUrl &url1,const QUrl &url2);


    bool needToUseSuperResolution=false;
    bool needToUseSuperResolutionLastLinearFiltering=true;
    bool needToUseFXAAFiltering=false;


};


#endif // MAINWINDOW_H
