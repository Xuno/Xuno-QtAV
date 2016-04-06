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
#include "MainWindow.h"
#include "EventFilter.h"
#include <QtAV>
#include <QtAVWidgets>
#include <QtCore/QtDebug>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QTimeEdit>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QGraphicsOpacityEffect>
#include <QComboBox>
#include <QResizeEvent>
#include <QWidgetAction>
#include <QLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QToolTip>
#include <QKeyEvent>
#include <QWheelEvent>
#include "ClickableMenu.h"
#include "Slider.h"
#include "StatisticsView.h"
#include "TVView.h"
#include "config/DecoderConfigPage.h"
#include "config/VideoEQConfigPage.h"
#include "config/ConfigDialog.h"
#include "filters/OSDFilter.h"
//#include "filters/AVFilterSubtitle.h"
#include "playlist/PlayList.h"
#include "../common/common.h"
#include <QUrl>
#include "filters/advancedfilter.h"


/*
 *TODO:
 * disable a/v actions if player is 0;
 * use action's value to set player's parameters when start to play a new file
 */


#define AVDEBUG() \
    qDebug("%s %s @%d", __FILE__, __FUNCTION__, __LINE__);

#define IMGSEQOPENGL 0

using namespace QtAV;
const qreal kVolumeInterval = 0.04;

extern QStringList idsToNames(QVector<VideoDecoderId> ids);
extern QVector<VideoDecoderId> idsFromNames(const QStringList& names);

void QLabelSetElideText(QLabel *label, QString text, int W = 0)
{
    QFontMetrics metrix(label->font());
    int width = label->width() - label->indent() - label->margin();
    if (W || label->parent()) {
        int w = W;
        if (!w)
            w = ((QWidget*)label->parent())->width();
        width = qMax(w - label->indent() - label->margin() - 13*(30), 0); //TODO: why 30?
    }
    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
  , mIsReady(false)
  , mHasPendingPlay(false)
  , mControlOn(false)
  , mShowControl(2)
  , mRepeateMax(0)
  , mpVOAction(0)
  , mpPlayer(0)
  , mpRenderer(0)
  , mpVideoFilter(0)
  , mpAudioFilter(0)
  , mpStatisticsView(0)
  , mpOSD(0)
  , mpSubtitle(0)
  , mCustomFPS(0.)
  , mPlayerScale(1.)
  , m_preview(0)
  , mpAdvancedFilter(0)
{
    XUNOserverUrl=QString::fromLatin1("http://www.xuno.com");
    XUNOpresetUrl=XUNOserverUrl+QString::fromLatin1("/getpreset.php?");
    setWindowIcon(QIcon(QString::fromLatin1(":/Xuno-QtAV.ico")));
    mpOSD = new OSDFilter(this);
    mpSubtitle = new SubtitleFilter(this);
    mpChannelAction = 0;
    mpChannelMenu = 0;
    mpAudioTrackAction = 0;
    mGlobalMouse = QPointF();

    setMouseTracking(true); //mouseMoveEvent without press.
    connect(this, SIGNAL(ready()), SLOT(processPendingActions()));
    //QTimer::singleShot(10, this, SLOT(setupUi()));
    setupUi();
    //setToolTip(tr("Click black area to use shortcut(see right click menu)"));
    WindowEventFilter *we = new WindowEventFilter(this);
    installEventFilter(we);
    connect(we, SIGNAL(fullscreenChanged()), SLOT(handleFullscreenChange()));
}

MainWindow::~MainWindow()
{
    if (m_preview) {
        m_preview->close();
        delete m_preview;
    }
    mpHistory->save();
    mpPlayList->save();
    if (mpVolumeSlider && !mpVolumeSlider->parentWidget()) {
        mpVolumeSlider->close();
        delete mpVolumeSlider;
        mpVolumeSlider = 0;
    }
    if (mpStatisticsView) {
        delete mpStatisticsView;
        mpStatisticsView = 0;
    }
}

void MainWindow::initPlayer()
{
    mpPlayer = new AVPlayer(this);
    mIsReady = true;
    VideoRenderer *vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());
    if (!vo || !vo->isAvailable() || !vo->widget()) {
        QMessageBox::critical(0, QString::fromLatin1("QtAV"), tr("Video renderer is ") + tr("not availabe on your platform!"));
    }
    setRenderer(vo);
    //mpSubtitle->installTo(mpPlayer); //filter on frame
    mpSubtitle->setPlayer(mpPlayer);
    //mpPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
    installAdvancedFilter();
    EventFilter *ef = new EventFilter(mpPlayer);
    qApp->installEventFilter(ef);
    connect(ef, SIGNAL(helpRequested()), SLOT(help()));
    connect(ef, SIGNAL(showNextOSD()), SLOT(showNextOSD()));
    onCaptureConfigChanged();
    onAVFilterVideoConfigChanged();
    onAVFilterAudioConfigChanged();
    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureFormatChanged(QString)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(previewEnabledChanged()), SLOT(onPreviewEnabledChanged()));
    connect(&Config::instance(), SIGNAL(avfilterVideoChanged()), SLOT(onAVFilterVideoConfigChanged()));
    connect(&Config::instance(), SIGNAL(avfilterAudioChanged()), SLOT(onAVFilterAudioConfigChanged()));
    connect(&Config::instance(), SIGNAL(bufferValueChanged()), SLOT(onBufferValueChanged()));
    connect(&Config::instance(), SIGNAL(abortOnTimeoutChanged()), SLOT(onAbortOnTimeoutChanged()));
    connect(mpStopBtn, SIGNAL(clicked()), this, SLOT(stopUnload()));
    connect(mpForwardBtn, SIGNAL(clicked()), mpPlayer, SLOT(seekForward()));
    connect(mpBackwardBtn, SIGNAL(clicked()), mpPlayer, SLOT(seekBackward()));
    connect(mpVolumeBtn, SIGNAL(clicked()), SLOT(showHideVolumeBar()));
    connect(mpVolumeSlider, SIGNAL(sliderPressed()), SLOT(setVolume()));
    connect(mpVolumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume()));

    connect(mpPlayer, SIGNAL(seekFinished(qint64)), SLOT(onSeekFinished(qint64)));
    connect(mpPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), SLOT(onMediaStatusChanged()));
    connect(mpPlayer, SIGNAL(bufferProgressChanged(qreal)), SLOT(onBufferProgress(qreal)));
    connect(mpPlayer, SIGNAL(error(QtAV::AVError)), this, SLOT(handleError(QtAV::AVError)));
    connect(mpPlayer, SIGNAL(started()), this, SLOT(onStartPlay()));
    connect(mpPlayer, SIGNAL(stopped()), this, SLOT(onStopPlay()));
    connect(mpPlayer, SIGNAL(paused(bool)), this, SLOT(onPaused(bool)));
    connect(mpPlayer, SIGNAL(speedChanged(qreal)), this, SLOT(onSpeedChange(qreal)));
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChange(qint64)));
    //connect(mpPlayer, SIGNAL(volumeChanged(qreal)), SLOT(syncVolumeUi(qreal)));
    connect(mpVideoEQ, SIGNAL(brightnessChanged(int)), this, SLOT(onBrightnessChanged(int)));
    connect(mpVideoEQ, SIGNAL(contrastChanged(int)), this, SLOT(onContrastChanged(int)));
    connect(mpVideoEQ, SIGNAL(hueChanegd(int)), this, SLOT(onHueChanged(int)));
    connect(mpVideoEQ, SIGNAL(saturationChanged(int)), this, SLOT(onSaturationChanged(int)));
    //connect(mpVideoEQ, SIGNAL(gammaRGBChanged(int)),  this, SLOT(onGammaRGBChanged(int)));
    //connect(mpVideoEQ, SIGNAL(filterSharpChanged(int)),  this, SLOT(onFilterSharpChanged(int)));


    connect(mpCaptureBtn, SIGNAL(clicked()), mpPlayer->videoCapture(), SLOT(capture()));

    emit ready(); //emit this signal after connection. otherwise the slots may not be called for the first time
}

void MainWindow::onSeekFinished(qint64 pos)
{
    qDebug("seek finished at %lld/%lld", pos, mpPlayer->position());
}

void MainWindow::stopUnload()
{
    if (mpPlayer){
        mpPlayer->stop();
        mpPlayer->unload();
    }
}

void MainWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(QMargins());
    setLayout(mainLayout);

    mpPlayerLayout = new QVBoxLayout();

    //QVBoxLayout *mpControlLayout = new QVBoxLayout();

    if (Config::instance().floatControlEnabled()){
        detachedControl = new QWidget(this);
        detachedControl->setWindowTitle(tr("XunoPlayer Controls"));
        detachedControl->setWindowFlags(Qt::Dialog);
        detachedControl->setWindowFlags(detachedControl->windowFlags() & ~Qt::WindowCloseButtonHint);
        detachedControl->setMaximumHeight(125);//785
        detachedControl->setMaximumHeight(55);
        detachedControl->resize(800,detachedControl->minimumHeight());
        this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
        detachedControl->move(this->pos().x(),this->pos().y()+this->rect().height());
        detachedControlLayout = new QVBoxLayout();
        detachedControlLayout->setContentsMargins(0,0,0,0);
        detachedControl->setLayout(detachedControlLayout);
        detachedControl->show();
        detachedControl->raise();
        mpControl = new QWidget(detachedControl);



    }else{
        mpControl = new QWidget(this);
    }

    mpControl->setMinimumWidth(865);
    mpControl->setMaximumHeight(25);
    mpControl->setMaximumHeight(30);

    //mpPreview = new QLable(this);

    mpTimeSlider = new Slider(mpControl);
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setTracking(true);
    mpTimeSlider->setOrientation(Qt::Horizontal);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setStyleSheet(" \
                                QSlider::groove:horizontal { \
                                    margin: 4px 0px 2px 0px;\
                                    background-color: solid #F8F8F8; \
                                } \
                                QSlider::handle:horizontal { \
                                    width: 7px;  \
                                    border: 1px solid #5c5c5c; \
                                    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #ffffff); \
                                    border-radius: 2px;\
                                    margin: -2px 0;\
                                }\
                                QSlider::sub-page:horizontal { \
                                    margin: 2px 0px 3px 0px;\
                                    background-color: #0066FF; \
                                } \
                                ");
                                mpTimeSlider->addVisualLimits();
            mpCurrent = new QLabel(mpControl);
    mpCurrent->setToolTip(tr("Current time"));
    mpCurrent->setContentsMargins(QMargins(2, 2, 2, 2));
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd = new QLabel(mpControl);
    mpEnd->setToolTip(tr("Duration"));
    mpEnd->setContentsMargins(QMargins(2, 2, 2, 2));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    //    mpTitle = new QLabel(mpControl);
    //    mpTitle->setToolTip(tr("Render engine"));
    //    mpTitle->setText("QPainter");
    //    mpTitle->setIndent(8);
    mpSpeed = new QLabel(QString::fromLatin1("1.00"));
    mpSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    mpSpeed->setToolTip(tr("Speed. Ctrl+Up/Down"));

    mpPlayPauseBtn = new QToolButton(mpControl);
    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
    mpStopBtn = new QToolButton(mpControl);
    mpStopBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/stop.svg")));
    mpBackwardBtn = new QToolButton(mpControl);
    mpBackwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/backward.svg")));
    mpForwardBtn = new QToolButton(mpControl);
    mpForwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/forward.svg")));
    mpOpenBtn = new QToolButton(mpControl);
    mpOpenBtn->setToolTip(tr("Open"));
    mpOpenBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/open.svg")));

    mpInfoBtn = new QToolButton();
    mpInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    mpInfoBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/info.svg")));
    mpCaptureBtn = new QToolButton();
    mpCaptureBtn->setToolTip(tr("Capture"));
    mpCaptureBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/capture.svg")));
    mpVolumeBtn = new QToolButton();
    mpVolumeBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/sound.svg")));

    mpVolumeSlider = new Slider();
    //mpVolumeSlider->hide();
    mpVolumeSlider->setOrientation(Qt::Horizontal);
    mpVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax = 100;
    mpVolumeSlider->setMaximum(kVolumeSliderMax);
    //mpVolumeSlider->setMaximumHeight(12);
    mpVolumeSlider->setMaximumWidth(88);
    //mpVolumeSlider->setValue(int(1.0/kVolumeInterval*qreal(kVolumeSliderMax)/100.0));
    mpVolumeSlider->setValue(80);
    setVolume();

    mpWebBtn = new QToolButton();
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setText(tr("Web"));
    //mpWebBtn->setMaximumHeight(kMaxButtonIconMargin);
    mpWebBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpWebBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setPopupMode(QToolButton::InstantPopup);
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setToolTip(tr("Open XunoWeb browser"));
    mpWebBtn->setStyleSheet(QString::fromLatin1("color:grey;"));


    mpWebMenu = new ConfigWebMemu(mpWebBtn);
    mpWebBtn->setMenu(mpWebMenu);
    connect(mpWebMenu, SIGNAL(onPlayXunoBrowser(QUrl)), SLOT(onClickXunoBrowser(QUrl)));
    connect(&Config::instance(),SIGNAL(weblinksChanged()),mpWebMenu,SLOT(onChanged()));

    mpFullScreenBtn = new QToolButton();
    mpFullScreenBtn->setIcon(QPixmap(QString::fromLatin1(":/theme/dark/fullscreen.svg")));
    //mpFullScreenBtn->setIconSize(QSize(a, a));
    //mpFullScreenBtn->setMaximumSize(a+kMaxButtonIconMargin+2, a+kMaxButtonIconMargin);
    mpFullScreenBtn->setToolTip(tr("Full Screen"));

    mpMenuBtn = new QToolButton();
    mpMenuBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/menu.svg")));
    //mpMenuBtn->setAutoRaise(true);
    mpMenuBtn->setPopupMode(QToolButton::InstantPopup);

    mpMenuBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    //mpMenuBtn->setMaximumHeight(a+kMaxButtonIconMargin);
    mpMenuBtn->setText(tr("Menu "));
    mpMenuBtn->setToolTip(tr("Configuratrion menu"));
    mpMenuBtn->setStyleSheet("color:grey;");
    mpMenuBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpMenuBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpMenuBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());

    QMenu *subMenu = 0;
    QWidgetAction *pWA = 0;
    mpMenu = new QMenu(mpMenuBtn);
    mpMenu->addAction(tr("Open File"), this, SLOT(openFile()));
    mpMenu->addAction(tr("Open Url"), this, SLOT(openUrl()));
    //mpMenu->addAction(tr("Online channels"), this, SLOT(onTVMenuClick()));

    //    subMenu = new ClickableMenu(tr("Image Sequence"));
    //mpMenu->addMenu(subMenu);

    mpImageSequence = new ImageSequenceConfigPage();
    connect(mpImageSequence, SIGNAL(play(QString)), SLOT(play(QString)));
    connect(mpImageSequence, SIGNAL(stop()), SLOT(stopUnload()));
    connect(mpImageSequence, SIGNAL(repeatAChanged(QTime)), SLOT(repeatAChanged(QTime)));
    connect(mpImageSequence, SIGNAL(repeatBChanged(QTime)), SLOT(repeatBChanged(QTime)));
    connect(mpImageSequence, SIGNAL(toggleRepeat(bool)), SLOT(toggleRepeat(bool)));
    connect(mpImageSequence, SIGNAL(customfpsChanged(double)), SLOT(customfpsChanged(double)));
    connect(mpImageSequence, SIGNAL(toogledFrameExtractor(bool)), SLOT(onImageSequenceToogledFrameExtractor(bool)));
    connect(mpImageSequence, SIGNAL(setPlayerScale(double)), SLOT(setPlayerScale(double)));
    connect(mpImageSequence, SIGNAL(RepeatLoopChanged(int)), SLOT(RepeatLoopChanged(int)));

    mpMenu->addAction(tr("Image Sequence"), this, SLOT(onImageSequenceConfig()));

    //    pWA = new QWidgetAction(0);
    //    pWA->setDefaultWidget(mpImageSequence);
    //    subMenu->addAction(pWA);

    mpMenu->addSeparator();
    subMenu = new QMenu(tr("Play list"));
    mpMenu->addMenu(subMenu);
    mpPlayList = new PlayList(this);
    mpPlayList->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/playlist.qds"));
    mpPlayList->load();
    connect(mpPlayList, SIGNAL(aboutToPlay(QString)), SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpPlayList);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new QMenu(tr("History"));
    mpMenu->addMenu(subMenu);
    mpHistory = new PlayList(this);
    mpHistory->setMaxRows(20);
    mpHistory->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/history.qds"));
    mpHistory->load();
    connect(mpHistory, SIGNAL(aboutToPlay(QString)), SLOT(play(QString)));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpHistory);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    mpMenu->addSeparator();

    //mpMenu->addAction(tr("Report"))->setEnabled(false); //report bug, suggestions etc. using maillist?
    mpMenu->addAction(tr("About"), this, SLOT(about()));
    mpMenu->addAction(tr("Help"), this, SLOT(help()));
    mpMenu->addAction(tr("Donate"), this, SLOT(donate()));
    mpMenu->addAction(tr("Setup"), this, SLOT(setup()));
    mpMenu->addSeparator();
    mpMenuBtn->setMenu(mpMenu);
    mpMenu->addSeparator();

    subMenu = new QMenu(tr("Speed"));
    mpMenu->addMenu(subMenu);
    QDoubleSpinBox *pSpeedBox = new QDoubleSpinBox(0);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new ClickableMenu(tr("Repeat"));
    mpMenu->addMenu(subMenu);
    subMenu->setObjectName(QString::fromUtf8("RepeatMenu"));
    //subMenu->setEnabled(false);
    mpRepeatEnableAction = subMenu->addAction(tr("Enable"));
    mpRepeatEnableAction->setCheckable(true);
    connect(mpRepeatEnableAction, SIGNAL(toggled(bool)), SLOT(toggleRepeat(bool)));
    // TODO: move to a func or class
    mpRepeatLoop = new QCheckBox(tr("Continuous Loop"), this);
    connect(mpRepeatLoop, SIGNAL(stateChanged(int)), SLOT(RepeatLoopChanged(int)));
    mpRepeatBox = new QSpinBox(0);
    mpRepeatBox->setMinimum(-1);
    mpRepeatBox->setValue(1);
    mpRepeatBox->setToolTip(QString::fromLatin1("-1: ") + tr("infinity"));
    connect(mpRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel *pRepeatLabel = new QLabel(tr("Times"));
    QHBoxLayout *hb = new QHBoxLayout;
    hb->setObjectName(QString::fromUtf8("TimesLayout"));
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatBox);
    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(mpRepeatLoop);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("From"));
    mpRepeatA = new QTimeEdit();
    mpRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatA->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatA, SIGNAL(timeChanged(QTime)), SLOT(repeatAChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatA);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("To"));
    mpRepeatB = new QTimeEdit();
    mpRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatB->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatB, SIGNAL(timeChanged(QTime)), SLOT(repeatBChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatB);
    vb->addLayout(hb);
    QWidget *wgt = new QWidget;
    wgt->setLayout(vb);

    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    mpRepeatAction = pWA;
    mpRepeatLoop->setCheckState(Qt::Checked);

    mpMenu->addSeparator();

    mpClockMenu = new ClickableMenu(tr("Clock"));
    mpClockMenu->setObjectName(QStringLiteral("Clock"));
    mpMenu->addMenu(mpClockMenu);
    mpClockMenuAction = new QActionGroup(mpClockMenu);
    mpClockMenuAction->setObjectName(QStringLiteral("ClockAction"));
    mpClockMenuAction->setExclusive(true);
    connect(mpClockMenu, SIGNAL(triggered(QAction*)), SLOT(changeClockType(QAction*)));
    mpClockMenu->addAction(tr("Auto"))->setData(-1);
    mpClockMenu->addAction(tr("Audio"))->setData(AVClock::AudioClock);
    mpClockMenu->addAction(tr("Video"))->setData(AVClock::VideoClock);
    foreach(QAction* action, mpClockMenu->actions()) {
        action->setActionGroup(mpClockMenuAction);
        action->setCheckable(true);
    }
    QAction *autoClockAction = mpClockMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(tr("Take effect in next playback"));

    subMenu = new ClickableMenu(tr("Subtitle"));
    mpMenu->addMenu(subMenu);
    QAction *act = subMenu->addAction(tr("Enable"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->isEnabled());
    connect(act, SIGNAL(toggled(bool)), SLOT(toggoleSubtitleEnabled(bool)));
    act = subMenu->addAction(tr("Auto load"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->autoLoad());
    connect(act, SIGNAL(toggled(bool)), SLOT(toggleSubtitleAutoLoad(bool)));
    subMenu->addAction(tr("Open"), this, SLOT(openSubtitle()));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Engine")));
    QComboBox *box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(QString::fromLatin1("FFmpeg"), QString::fromLatin1("FFmpeg"));
    box->addItem(QString::fromLatin1("LibASS"), QString::fromLatin1("LibASS"));
    connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleEngine(QString)));
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(tr("FFmpeg supports more subtitles but only render plain text") + QString::fromLatin1("\n") + tr("LibASS supports ass styles"));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Charset")));
    box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(tr("Auto detect"), QString::fromLatin1("AutoDetect"));
    box->addItem(tr("System"), QString::fromLatin1("System"));
    foreach (const QByteArray& cs, QTextCodec::availableCodecs()) {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }
    connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleCharset(QString)));
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(tr("Auto detect requires libchardet"));

    subMenu = new ClickableMenu(tr("Audio track"));
    mpMenu->addMenu(subMenu);
    mpAudioTrackMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeAudioTrack(QAction*)));
    initAudioTrackMenu();

    subMenu = new ClickableMenu(tr("Channel"));
    mpMenu->addMenu(subMenu);
    mpChannelMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeChannel(QAction*)));
    subMenu->addAction(tr("As input"))->setData(AudioFormat::ChannelLayout_Unsupported); //will set to input in resampler if not supported.
    subMenu->addAction(tr("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(tr("Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(tr("Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(tr("Right"))->setData(AudioFormat::ChannelLayout_Right);
    QActionGroup *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu = new QMenu(tr("Aspect ratio"));
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(switchAspectRatio(QAction*)));
    mpARAction = subMenu->addAction(tr("Video"));
    mpARAction->setData(0);
    subMenu->addAction(tr("Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0/3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0/9.0);
    subMenu->addAction(tr("Custom"))->setData(-2);
    foreach(QAction* action, subMenu->actions()) {
        action->setCheckable(true);
    }
    mpARAction->setChecked(true);

    subMenu = new ClickableMenu(tr("Color space"));
    mpMenu->addMenu(subMenu);
    mpVideoEQ = new VideoEQConfigPage();
    connect(mpVideoEQ, SIGNAL(engineChanged()), SLOT(onVideoEQEngineChanged()));
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpVideoEQ);
    subMenu->addAction(pWA);

    mpVideoEQ->setSaveFile(Config::instance().defaultDir() + "/presets.ini");
    mpVideoEQ->loadLocalPresets();

    subMenu = new ClickableMenu(tr("Decoder"));
    mpMenu->addMenu(subMenu);
    mpDecoderConfigPage = new DecoderConfigPage();
    pWA = new QWidgetAction(0);
    pWA->setDefaultWidget(mpDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu = new ClickableMenu(tr("Renderer"));
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeVO(QAction*)));
    //TODO: AVOutput.name,detail(description). check whether it is available
    VideoRendererId *vo = NULL;
    while ((vo = VideoRenderer::next(vo))) {
        // skip non-widget renderers
        if (*vo == VideoRendererId_OpenGLWindow || *vo == VideoRendererId_GraphicsItem)
            continue;
        QAction *voa = subMenu->addAction(QString::fromLatin1(VideoRenderer::name(*vo)));
        voa->setData(*vo);
        voa->setCheckable(true);
        if (!mpVOAction)
            mpVOAction = voa;
    }
    mpVOAction->setChecked(true);
    mVOActions = subMenu->actions();

    //mpImgSeqExtract->hide();
    mainLayout->addLayout(mpPlayerLayout);

    if (detachedControlLayout){
        detachedControlLayout->addWidget(mpTimeSlider);
        detachedControlLayout->addWidget(mpControl);
    }else{
        mainLayout->addWidget(mpTimeSlider);
        mainLayout->addWidget(mpControl);
    }

    // mpControlLayout->addWidget(mpImgSeqExtract);
    //mainLayout->addWidget(mpControl);

    QVBoxLayout *controlVLayout = new QVBoxLayout();
    controlVLayout->setSpacing(0);
    controlVLayout->setMargin(0);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    mpControl->setLayout(controlVLayout);
    controlLayout->addWidget(mpCurrent);
    //controlLayout->addWidget(mpTitle);
    //QSpacerItem *space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    QSpacerItem *space = new QSpacerItem(1, mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpVolumeSlider);
    controlLayout->addWidget(mpVolumeBtn);
    controlLayout->addWidget(mpCaptureBtn);
    controlLayout->addWidget(mpPlayPauseBtn);
    controlLayout->addWidget(mpStopBtn);
    controlLayout->addWidget(mpBackwardBtn);
    controlLayout->addWidget(mpForwardBtn);
    space = new QSpacerItem(mpPlayPauseBtn->width()/2, mpPlayPauseBtn->height(), QSizePolicy::Fixed);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpWebBtn);
    controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpOpenBtn);
    controlLayout->addWidget(mpInfoBtn);
    //controlLayout->addWidget(mpSpeed);
    //controlLayout->addWidget(mpSetupBtn);
    //controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpFullScreenBtn);
    space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::Expanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpEnd);

    controlVLayout->setSizeConstraint(QLayout::SetMinimumSize);
    mpImgSeqExtract = new ImgSeqExtractControl(mpControl);
    connect(mpImgSeqExtract,SIGNAL(seek(QTime)),SLOT(seek(QTime)));
    connect(mpImgSeqExtract,SIGNAL(pause()),SLOT(pause()));
    connect(mpImgSeqExtract,SIGNAL(onStartPlay()),SLOT(onStartPlay()));
    connect(mpImgSeqExtract,SIGNAL(toggleRepeat(bool)),SLOT(toggleRepeat(bool)));
    connect(mpImgSeqExtract,SIGNAL(RepeatLoopChanged(int)),SLOT(RepeatLoopChanged(int)));
    connect(mpImgSeqExtract,SIGNAL(togglePlayPause()),SLOT(togglePlayPause()));
    connect(mpImgSeqExtract,SIGNAL(repeatAChanged(QTime)),SLOT(repeatAChanged(QTime)));
    connect(mpImgSeqExtract,SIGNAL(repeatBChanged(QTime)),SLOT(repeatBChanged(QTime)));
    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMinLimit(QTime)),SLOT(setTimeSliderVisualMinLimit(QTime)));
    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMaxLimit(QTime)),SLOT(setTimeSliderVisualMaxLimit(QTime)));
    controlVLayout->addWidget(mpImgSeqExtract);
    controlVLayout->addLayout(controlLayout);
    mpImgSeqExtract->setVisible(false);
    //    mpImgSeqExtract->setStyleSheet(" \
    //    QSpinBox::up-arrow { \
    //        image: url(:/theme/up_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    QSpinBox::down-arrow { \
    //        image: url(:/theme/down_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    ");

    connect(pSpeedBox, SIGNAL(valueChanged(double)), SLOT(onSpinBoxChanged(double)));
    connect(mpOpenBtn, SIGNAL(clicked()), SLOT(openFile()));
    connect(mpPlayPauseBtn, SIGNAL(clicked()), SLOT(togglePlayPause()));
    connect(mpInfoBtn, SIGNAL(clicked()), SLOT(showInfo()));
    //valueChanged can be triggered by non-mouse event
    connect(mpTimeSlider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
    connect(mpTimeSlider, SIGNAL(sliderPressed()), SLOT(seek()));
    connect(mpTimeSlider, SIGNAL(onLeave()), SLOT(onTimeSliderLeave()));
    connect(mpTimeSlider, SIGNAL(onHover(int,int)), SLOT(onTimeSliderHover(int,int)));
    //connect(mpWebBtn, SIGNAL(clicked()), SLOT(onXunoBrowser()));
    connect(mpFullScreenBtn, SIGNAL(clicked()), SLOT(onFullScreen()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));
}

void MainWindow::changeChannel(QAction *action)
{
    if (action == mpChannelAction) {
        action->toggle();
        return;
    }
    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        qWarning("No audio output!");
        return;
    }
    mpChannelAction->setChecked(false);
    mpChannelAction = action;
    mpChannelAction->setChecked(true);
    if (!ao->close()) {
        qWarning("close audio failed");
        return;
    }
    ao->audioFormat().setChannelLayout(cl);
    if (!ao->open()) {
        qWarning("open audio failed");
        return;
    }
}

void MainWindow::changeAudioTrack(QAction *action)
{
    int track = action->data().toInt();
    if (mpAudioTrackAction == action && track >= 0) { // external action is always clickable
        action->toggle();
        return;
    }
    if (track < 0) {
        QString f = QFileDialog::getOpenFileName(0, tr("Open an external audio track"));
        if (f.isEmpty()) {
            action->toggle();
            return;
        }
        mpPlayer->setExternalAudio(f);
    } else {
        mpPlayer->setExternalAudio(QString());
        if (!mpPlayer->setAudioStream(track)) {
            action->toggle();
            return;
        }
    }

    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(false);
    mpAudioTrackAction = action;
    mpAudioTrackAction->setChecked(true);
    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
}

void MainWindow::changeVO(QAction *action)
{
    if (action == mpVOAction) {
        action->toggle(); //check state changes if clicked
        return;
    }
    VideoRendererId vid = (VideoRendererId)action->data().toInt();
    VideoRenderer *vo = VideoRenderer::create(vid);
    if (vo && vo->isAvailable()) {
        if (!setRenderer(vo))
            action->toggle();
    } else {
        action->toggle(); //check state changes if clicked
        QMessageBox::critical(0, QString::fromLatin1("QtAV"), tr("not availabe on your platform!"));
        return;
    }
}

void MainWindow::processPendingActions()
{
    if (mHasPendingPlay) {
        mHasPendingPlay = false;
        play(mFile);
    }
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    mAudioBackends = backends;
}

bool MainWindow::setRenderer(QtAV::VideoRenderer *renderer)
{
    if (!renderer)
        return false;
    if (!renderer->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV"), tr("Can not use this renderer"));
        return false;
    }
    mpOSD->uninstall();
    mpSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); //mouseMoveEvent without press.
    mpPlayer->setRenderer(renderer);
    QWidget *r = 0;
    if (mpRenderer)
        r = mpRenderer->widget();
    //release old renderer and add new
    if (r) {
        mpPlayerLayout->removeWidget(r);
        if (r->testAttribute(Qt::WA_DeleteOnClose)) {
            r->close();
        } else {
            r->close();
            delete r;
        }
        r = 0;
    }
    mpRenderer = renderer;
    //setInSize?
    mpPlayerLayout->addWidget(renderer->widget());
    if (mpVOAction) {
        mpVOAction->setChecked(false);
    }
    foreach (QAction *action, mVOActions) {
        if (action->data() == renderer->id()) {
            mpVOAction = action;
            break;
        }
    }
    mpVOAction->setChecked(true);
    //mpTitle->setText(mpVOAction->text()); //was crash of player here after merge 2015-01-27
    const VideoRendererId vid = mpPlayer->renderer()->id();
    if (vid == VideoRendererId_GLWidget
            || vid == VideoRendererId_GLWidget2
            || vid == VideoRendererId_OpenGLWidget
            ) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale << VideoEQConfigPage::GLSL);
        mpVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
        installShaderXuno();
    } else if (vid == VideoRendererId_XV) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        mpVideoEQ->setEngine(VideoEQConfigPage::XV);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    } else {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        mpVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        mpPlayer->renderer()->forcePreferredPixelFormat(false);
    }
    onVideoEQEngineChanged();
    mpOSD->installTo(mpRenderer);
    mpSubtitle->installTo(mpRenderer);

    //mpRenderer->widget()->hide();

    return true;
}

void MainWindow::play(const QString &name)
{
    mFile = name;
    if (!mIsReady) {
        mHasPendingPlay = true;
        return;
    }
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://"))) {
        mTitle = QFileInfo(mFile).fileName();
    }
    if (isFileImgageSequence()) {
        mTitle = QString("Sequence of images: %1/%2").arg(QFileInfo(mFile).dir().dirName()).arg(QFileInfo(mFile).fileName());
        //toggleRepeat(true);
    }
    if (mFile.startsWith("http://")){
        mTitle = QString("http://%1/.../%2").arg(QUrl(mFile).host(),QString(QUrl(mFile).fileName()));
    }

    setWindowTitle(mTitle);
    mpPlayer->stop(); //if no stop, in setPriority decoder will reopen
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    if (!mAudioBackends.isEmpty())
        mpPlayer->audio()->setBackends(mAudioBackends);
    if (!mpRepeatEnableAction->isChecked())
        mRepeateMax = 0;
    else
        mRepeateMax = (mpRepeatLoop->isChecked())?-1:mpRepeatBox->value()-1;
    qDebug()<<"mRepeateMax"<<mRepeateMax;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
    mpPlayer->setInterruptTimeout(Config::instance().timeout()*1000.0);
    mpPlayer->setBufferMode(QtAV::BufferPackets);
    if (isFileImgageSequence())
        mpPlayer->setBufferValue(Config::instance().bufferValueI());
    else
        mpPlayer->setBufferValue(Config::instance().bufferValue());
    mpPlayer->setRepeat(mRepeateMax);
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());

    if (isFileImgageSequence()){
        applyCustomFPS(); //set options  avformatOptions included
    }else{
        if (Config::instance().avformatOptionsEnabled())
            mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());
    }

    setClockType();

    qDebug() << Config::instance().avformatOptions();
    PlayListItem item;
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setLastTime(0);
    mpHistory->remove(mFile);
    mpHistory->insertItemAt(item, 0);
    loadRemoteUrlPresset(mFile);
    mpImgSeqExtract->setMovieName(name);
    mpPlayer->play(name);
}

void MainWindow::play(const QUrl &url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
}

void MainWindow::play()
{
    mpPlayer->play();
    mpPlayPauseBtn->setIcon(QIcon(":/theme/dark/pause.svg"));
}


void MainWindow::setFpsSequenceFrame(const double fps)
{
    if (mpImageSequence && fps>0) mpImageSequence->setFPS(fps);
}

void MainWindow::setStartSequenceFrame(const quint32 sf)
{
    if (mpImageSequence && sf>0) mpImageSequence->setStartFrame(sf);
}

void MainWindow::setEndSequenceFrame(const quint32 ef)
{
    if (mpImageSequence && ef>0) mpImageSequence->setEndFrame(ef);
}

void MainWindow::setRepeatLoop(const bool loop)
{
    if (mpRepeatLoop) {
        mpRepeatLoop->setChecked(loop);
    }
    if (mpImageSequence)
        mpImageSequence->setRepeatLoop(loop);

}

void MainWindow::setPlayerScale(const double scale)
{
    if (scale>0) mPlayerScale=scale;
}

void MainWindow::setFileName(const QString fname)
{
    if (!fname.isEmpty()){
        mFile=fname;
        qDebug()<<"Name filename"<<fname;
        if (isFileImgageSequence() && mpImageSequence){
            mpImageSequence->setImageSequenceFileName(fname);
        }
    }
}

void MainWindow::setExtractTask(const QString fname){
    if (!fname.isEmpty()){
        qDebug()<<"MainWindow :: setExtractTask"<<fname;
        onImageSequenceToogledFrameExtractor(true);
        if (mpImgSeqExtract) {
            mpImgSeqExtract->setOutputPath(fname);
        }
    }
}

void MainWindow::setVideoDecoderNames(const QStringList &vd)
{
    QStringList vdnames;
    foreach (const QString& v, vd) {
        vdnames << v.toLower();
    }
    QStringList vidp;
    QStringList vids = idsToNames(VideoDecoder::registered());
    foreach (const QString& v, vids) {
        if (vdnames.contains(v.toLower())) {
            vidp.append(v);
        }
    }
    Config::instance().setDecoderPriorityNames(vidp);
}

void MainWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a media file"), Config::instance().lastFile());
    if (file.isEmpty())
        return;
    Config::instance().setLastFile(file);
    play(file);
}

void MainWindow::togglePlayPause()
{
    if (mpPlayer->isPlaying()) {
        qDebug("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(!mpPlayer->isPaused());
    } else {
        if (mFile.isEmpty())
            return;
        applyCustomFPS();
        if (!mpPlayer->isPlaying())
            play(mFile);
        else
            mpPlayer->play();
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    }
}

void MainWindow::pause()
{
    if (mpPlayer->isPlaying()) {
        qDebug("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(true);
    }
    if (mpImgSeqExtract) {
        Statistics st=mpPlayer->statistics();
        mpImgSeqExtract->setOutputDimension(QSize(st.video_only.width,st.video_only.height));
    }
}

void MainWindow::showNextOSD()
{
    if (!mpOSD)
        return;
    mpOSD->useNextShowType();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!mpPlayer)
        return;
    mpPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p) {
        qDebug("start pausing...");
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
        if (mpImgSeqExtract) mpImgSeqExtract->onPaused();
    } else {
        qDebug("stop pausing...");
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    }
}

void MainWindow::onStartPlay()
{
    //--- TODO --- remove after recover OpenGL rgb48le
    //#if !IMGSEQOPENGL
    //    bool rgb48=mpPlayer->statistics().video_only.pix_fmt.contains("rgb48be");
    //    VideoRenderer *vo = VideoRendererFactory::create( (isFileImgageSequence() && rgb48) ? VideoRendererId_Widget : VideoRendererId_GLWidget2);
    //    if (vo && vo->isAvailable()) {
    //        setRenderer(vo);
    //    }
    //#endif

    mpRenderer->setRegionOfInterest(QRectF());
    mFile = mpPlayer->file(); //open from EventFilter's menu
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
        mTitle = QFileInfo(mFile).fileName();
    setWindowTitle(mTitle);

    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    mpTimeSlider->setMinimum(mpPlayer->mediaStartPosition());
    mpTimeSlider->setMaximum(mpPlayer->mediaStopPosition());
    setPlayerPosFromRepeat();
    mpTimeSlider->setValue(0);
    mpTimeSlider->setEnabled(mpPlayer->isSeekable());
    mpEnd->setText(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    setVolume();
    mShowControl = 0;
    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
    ScreenSaver::instance().disable();
    initAudioTrackMenu();
    mpRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
    mpRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    mpRepeatA->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->startPosition()));
    mpRepeatB->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->stopPosition()));
    mCursorTimer = startTimer(3000);
    PlayListItem item = mpHistory->itemAt(0);
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setDuration(mpPlayer->duration());
    mpHistory->setItemAt(item, 0);
    updateChannelMenu();

    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
    analyeUsedFPS();
    if (mpImgSeqExtract) mpImgSeqExtract->setEndTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    reSizeByMovie();
    QTimer::singleShot(100, this, SLOT(runMpvPlayer()));
}

void MainWindow::onStopPlay()
{
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    if (mpPlayer->currentRepeat() < mpPlayer->repeat())
        return;
    // use shortcut to replay in EventFilter, the options will not be set, so set here
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());
    if (Config::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());

    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
    mpTimeSlider->setValue(0);
    qDebug(">>>>>>>>>>>>>>disable slider");
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setMaximum(0);
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    if (mpImgSeqExtract){
        mpImgSeqExtract->setEndTime(QTime(0, 0, 0));
        mpImgSeqExtract->setStartTime(QTime(0, 0, 0));
    }
    tryShowControlBar();
    ScreenSaver::instance().enable();
    //toggleRepeat(mpRepeatEnableAction->isChecked());  //after stop not reset repeat task
    //mRepeateMax = 0;
    killTimer(mCursorTimer);
    unsetCursor();
    if (m_preview)
        m_preview->setFile(QString());
    runMpvPlayerStop();
}

void MainWindow::onSpeedChange(qreal speed)
{
    mpSpeed->setText(QString::fromLatin1("%1").arg(speed, 4, 'f', 2, QLatin1Char('0')));
}

void MainWindow::seek(int value)
{
    mpPlayer->setSeekType(AccurateSeek);
    mpPlayer->seek((qint64)value);
    if (!m_preview || !Config::instance().previewEnabled())
        return;
    m_preview->setTimestamp(value);
    m_preview->preview();
    m_preview->setWindowFlags(m_preview->windowFlags() |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(Config::instance().previewWidth(), Config::instance().previewHeight());
    m_preview->show();
    if (mpImgSeqExtract) mpImgSeqExtract->setStartTime(QTime(0,0,0).addMSecs((qint64)mpTimeSlider->value()));
}

void MainWindow::seek(qint64 msec)
{
    mpPlayer->seek(msec);
}
void MainWindow::seek(QTime time)
{
    mpPlayer->seek((qint64)QTime(0,0,0).msecsTo(time));
}

void MainWindow::seek()
{
    seek(mpTimeSlider->value());
}

void MainWindow::showHideVolumeBar()
{
    if (mpVolumeSlider->isHidden()) {
        mpVolumeSlider->show();
    } else {
        mpVolumeSlider->hide();
    }
}

void MainWindow::setVolume()
{
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0;
    //qreal v = qreal(mpVolumeSlider->value())*kVolumeInterval;
    qreal v = qreal(mpVolumeSlider->value())/100.;
    if (ao) {
        if (qAbs(int(ao->volume()/kVolumeInterval) - mpVolumeSlider->value()) >= int(0.1/kVolumeInterval)) {
            ao->setVolume(v);
        }
    }
    mpVolumeSlider->setToolTip(QString::number(v));
    mpVolumeBtn->setToolTip(QString::number(v));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    if (mpPlayer)
        mpPlayer->stop();
    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QWidget::resizeEvent(e);
    if (mpvPlayerWindow1 && Config::instance().advancedFilterEnabled()) {
        mpvPlayerWindow1->resize(e->size());
        //        if (mpRenderer){
        //            mpRenderer->widget()->move(0,0);
        //        }
    }
    /*
    if (mpTitle)
        QLabelSetElideText(mpTitle, QFileInfo(mFile).fileName(), e->size().width());
    */
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mCursorTimer) {
        if (mpControl->isVisible())
            return;
        setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (mpPlayer->isSeekable())
        mpTimeSlider->setValue(pos);
    mpCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
    if (mpImgSeqExtract) {
        mpImgSeqExtract->setStartTime(QTime(0, 0, 0).addMSecs(pos));
        if (mpImgSeqExtract->regionPlaying() && mpImgSeqExtract->EndPosExtract() && pos >= mpImgSeqExtract->EndPosExtract()) pause();
    }
    //setWindowTitle(QString::number(mpPlayer->statistics().video_only.currentDisplayFPS(), 'f', 2).append(" ").append(mTitle));
}

void MainWindow::repeatAChanged(const QTime& t)
{
    mpRepeatA->setTime(t);
    if (!mpPlayer)
        return;
    mpPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisualMinLimit(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::repeatBChanged(const QTime& t)
{
    // when this slot is called? even if only range is set?
    if (t <= mpRepeatA->time())
        return;
    mpRepeatB->setTime(t);
    if (!mpPlayer)
        return;
    mpPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisualMaxLimit(QTime(0, 0, 0).msecsTo(t));
}

void MainWindow::setTimeSliderVisualMinLimit(const QTime& t)
{
    mpTimeSlider->setVisualMinLimit(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisibleVisualLimit(true);
}
void MainWindow::setTimeSliderVisualMaxLimit(const QTime& t)
{
    qint64 p=QTime(0, 0, 0).msecsTo(t);
    mpTimeSlider->setVisualMaxLimit(p);
    mpTimeSlider->setVisibleVisualLimit(p!=0);
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    mControlOn = e->key() == Qt::Key_Control;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    mControlOn = false;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //if (!mControlOn)
    //    return;
    mGlobalMouse = e->globalPos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    mGlobalMouse = QPointF();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    unsetCursor();
    if (e->pos().y() > height() - mpTimeSlider->height() - mpControl->height()) {
        if (mShowControl == 0) {
            mShowControl = 1;
            tryShowControlBar();
        }
    } else {
        if (mShowControl == 1) {
            mShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }
    if (!mGlobalMouse.isNull()  && (e->buttons() & Qt::LeftButton) ) {
        if (!mpRenderer || !mpRenderer->widget())
            return;
        QRectF roi = mpRenderer->realROI();
        QPointF delta = e->globalPos() - mGlobalMouse;
        mGlobalMouse=e->globalPos();
        QPointF center=roi.center()-delta;
        roi.moveCenter(center);
        if (roi.top()>1 && roi.left()>1 && roi.right() < mpRenderer->rendererWidth() && roi.bottom() < mpRenderer->rendererHeight()){
            mpRenderer->setRegionOfInterest(roi);
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    if (!mpRenderer || !mpRenderer->widget()) {
        return;
    }
    QPoint dp;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qreal deg = e->angleDelta().y()/8;
    dp = e->pixelDelta();
#else
    qreal deg = e->delta()/8;
#endif //QT_VERSION
#if WHEEL_SPEED
    if (!mControlOn) {
        qreal speed = mpPlayer->speed();
        mpPlayer->setSpeed(qMax(0.01, speed + deg/15.0*0.02));
        return;
    }
#endif //WHEEL_SPEED
    QPointF p = mpRenderer->widget()->mapFrom(this, e->pos());
    QPointF fp = mpRenderer->mapToFrame(p);
    if (fp.x() < 0)
        fp.setX(0);
    if (fp.y() < 0)
        fp.setY(0);
    if (fp.x() > mpRenderer->videoFrameSize().width())
        fp.setX(mpRenderer->videoFrameSize().width());
    if (fp.y() > mpRenderer->videoFrameSize().height())
        fp.setY(mpRenderer->videoFrameSize().height());

    QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)), mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(), mpRenderer->rendererHeight())));
    //qDebug("vo: (%.1f, %.1f)=> frame: (%.1f, %.1f)", p.x(), p.y(), fp.x(), fp.y());
    qreal zoom = 1.0 + deg*3.14/180.0;
    if (!dp.isNull()) {
        zoom = 1.0 + (qreal)dp.y()/100.0;
    }
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(mpRenderer->realROI());
    mpRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), mpRenderer->videoFrameSize()));
}

QString MainWindow::aboutXunoQtAV_PlainText()
{
    return aboutXunoQtAV_HTML().remove(QRegExp(QStringLiteral("<[^>]*>")));
}

QString MainWindow::aboutXunoQtAV_HTML()
{
    static QString about = "<h3>XunoPlayer " XUNO_QTAV_VERSION_STR_LONG "</h3>\n"
                                                                        "<p>" + QObject::tr("Fork project (Xuno-QtAV) of QtAV \n") + QTAV_VERSION_STR "</p>";
    return about;
}

void MainWindow::about()
{
    //QtAV::about();
    //we should use new because a qobject will delete it's children
    QTextBrowser *viewQtAV = new QTextBrowser;
    QTextBrowser *viewFFmpeg = new QTextBrowser;
    viewQtAV->setOpenExternalLinks(true);
    viewFFmpeg->setOpenExternalLinks(true);
    viewQtAV->setHtml(aboutXunoQtAV_HTML().append(aboutQtAV_HTML()));
    viewFFmpeg->setHtml(aboutFFmpeg_HTML());
    QTabWidget *tab = new QTabWidget;
    tab->addTab(viewQtAV, QStringLiteral("Xuno-QtAV"));
    tab->addTab(viewFFmpeg, QStringLiteral("FFmpeg"));
    QPushButton *qbtn = new QPushButton(QObject::tr("About Qt"));
    QPushButton *btn = new QPushButton(QObject::tr("Ok"));
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btn);
    btnLayout->addStretch();
    btnLayout->addWidget(qbtn);
    btn->setFocus();
    QDialog dialog;
    dialog.setWindowTitle(QObject::tr("About") + QStringLiteral("  Xuno-QtAV"));
    QVBoxLayout *layout = new QVBoxLayout;
    dialog.setLayout(layout);
    layout->addWidget(tab);
    layout->addLayout(btnLayout);
    QObject::connect(qbtn, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    QObject::connect(btn, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.exec();
}

void MainWindow::help()
{
    QString name = QString::fromLatin1("help-%1.html").arg(QLocale::system().name());
    QFile f(qApp->applicationDirPath() + QString::fromLatin1("/") + name);
    if (!f.exists()) {
        f.setFileName(QString::fromLatin1(":/") + name);
    }
    if (!f.exists()) {
        f.setFileName(qApp->applicationDirPath() + QString::fromLatin1("/help.html"));
    }
    if (!f.exists()) {
        f.setFileName(QString::fromLatin1(":/help.html"));
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open help-%s.html and help.html: %s", qPrintable(QLocale::system().name()), qPrintable(f.errorString()));
        return;
    }
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    QString text = ts.readAll();
    QMessageBox::information(0, tr("Help"), text);
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(0, tr("Open an url"), tr("Url"));
    if (url.isEmpty())
        return;
    play(url);
}


void MainWindow::updateChannelMenu()
{
    if (mpChannelAction)
        mpChannelAction->setChecked(false);
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        return;
    }
    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as = mpChannelMenu->actions();
    foreach (QAction *action, as) {
        if (action->data().toInt() != (int)cl)
            continue;
        action->setChecked(true);
        mpChannelAction = action;
        break;
    }
}

void MainWindow::initAudioTrackMenu()
{
    int track = -2;
    QAction *a = 0;
    QList<QAction*> as;
    int tracks = 0;
    if (!mpPlayer) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
        goto end;
    }
    track = mpPlayer->currentAudioStream();
    as = mpAudioTrackMenu->actions();
    tracks = mpPlayer->audioStreamCount();
    if (mpAudioTrackAction && tracks == as.size()-1 && mpAudioTrackAction->data().toInt() == track)
        return;
    while (tracks + 1 < as.size()) {
        a = as.takeLast();
        mpAudioTrackMenu->removeAction(a);
        delete a;
    }
    if (as.isEmpty()) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
    }
    while (tracks + 1 > as.size()) {
        a = mpAudioTrackMenu->addAction(QString::number(as.size()-1));
        a->setData(as.size()-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
    }
end:
    foreach(QAction *ac, as) {
        if (ac->data().toInt() == track && track >= 0) {
            if (mpPlayer && mpPlayer->externalAudio().isEmpty()) {
                qDebug("track found!!!!!");
                mpAudioTrackAction = ac;
                ac->setChecked(true);
            }
        } else {
            ac->setChecked(false);
        }
    }
    if (mpPlayer && !mpPlayer->externalAudio().isEmpty()) {
        mpAudioTrackAction = as.first();
    }
    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(true);
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();
    if (action == mpARAction && r != -2) {
        action->toggle(); //check state changes if clicked
        return;
    }
    if (r == 0) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    } else if (r == -1) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
    } else {
        if (r == -2)
            r = QInputDialog::getDouble(0, tr("Aspect ratio"), QString(), 1.0);
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::CustomAspectRation);
        mpPlayer->renderer()->setOutAspectRatio(r);
    }
    mpARAction->setChecked(false);
    mpARAction = action;
    mpARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    mpRepeatEnableAction->setChecked(r);
    mpRepeatAction->defaultWidget()->setEnabled(r); //why need defaultWidget?
    if (r) {
        mRepeateMax = mpRepeatBox->value();
    } else {
        mRepeateMax = 0;
    }
    if (mpPlayer) {
        mpPlayer->setRepeat(mpRepeatLoop->isChecked()?-1:mRepeateMax-1);
        mpTimeSlider->setVisibleVisualLimit(r);
    }
}

void MainWindow::setRepeateMax(int m)
{
    if (m!=0) mRepeateMax = m;
    if (m<1)  mpRepeatBox->setValue(1);
    if (mpPlayer) {
        mpPlayer->setRepeat(mpRepeatLoop->isChecked()?-1:m-1);
    }
}

void MainWindow::playOnlineVideo(QAction *action)
{
    mTitle = action->text();
    play(action->data().toString());
}

void MainWindow::onTVMenuClick()
{
    static TVView *tvv = new TVView;
    tvv->show();
    connect(tvv, SIGNAL(clicked(QString,QString)), SLOT(onPlayListClick(QString,QString)));
    tvv->setMaximumHeight(qApp->desktop()->height());
    tvv->setMinimumHeight(tvv->width()*2);
}

void MainWindow::onPlayListClick(const QString &key, const QString &value)
{
    mTitle = key;
    play(value);
}

void MainWindow::tryHideControlBar()
{
    if (mShowControl > 0) {
        return;
    }
    if ( (mpTimeSlider && mpTimeSlider->isHidden()) &&
         (mpControl && mpControl->isHidden())
         )
        return;

    qDebug()<<"tryHideControlBar";
    if (!detachedControl) mpControl->hide();
    if (mpControl->isHidden()) mpTimeSlider->hide();
    workaroundRendererSize();
}

void MainWindow::tryShowControlBar()
{
    unsetCursor();
    if ((mpTimeSlider && mpTimeSlider->isHidden()) &&
            (mpControl && mpControl->isHidden())
            )
        mpTimeSlider->show();
    if (detachedControl)
        detachedControl->show();
    else
        mpControl->show();
}

void MainWindow::showInfo()
{
    if (!mpStatisticsView)
        mpStatisticsView = new StatisticsView();
    if (mpPlayer)
        mpStatisticsView->setStatistics(mpPlayer->statistics());
    mpStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    QPoint gpos;
    if (detachedControl){
        gpos = (detachedControl->pos() + QPoint(pos, 0));
    }else{
        gpos = mapToGlobal(mpTimeSlider->pos() + QPoint(pos, 0));
    }
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss.zzz")));
    if (!Config::instance().previewEnabled())
        return;
    if (!m_preview)
        m_preview = new VideoPreviewWidget();

    if (!m_preview) return;

    m_preview->setFile(mpPlayer->file());
    m_preview->setTimestamp(value);
    m_preview->preview();
    const int w = Config::instance().previewWidth();
    const int h = Config::instance().previewHeight();
    m_preview->setWindowFlags(m_preview->windowFlags() |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(w, h);
    m_preview->move(gpos - QPoint(w/2, h));
    m_preview->show();

}

void MainWindow::onTimeSliderLeave()
{
    if (m_preview && m_preview->isVisible())
        m_preview->hide();
}

void MainWindow::handleError(const AVError &e)
{
    QMessageBox::warning(0, tr("Player error"), e.string());
}

void MainWindow::onMediaStatusChanged()
{
    QString status;
    AVPlayer *player = reinterpret_cast<AVPlayer*>(sender());
    if (!player) { //why it happens? reinterpret_cast  works.
        qWarning() << "invalid sender() " << sender() << player;
        return;
    }
    switch (player->mediaStatus()) {
    case NoMedia:
        status = tr("No media");
        break;
    case InvalidMedia:
        status = tr("Invalid meida");
        break;
    case BufferingMedia:
        status = tr("Buffering...");
        break;
    case BufferedMedia:
        status = tr("Buffered");
        break;
    case LoadingMedia:
        status = tr("Loading...");
        break;
    case LoadedMedia:
        status = tr("Loaded");
        break;
    case StalledMedia:
        status = tr("Stalled");
        break;
    default:
        status = QString();
        onStopPlay();
        break;
    }
    qDebug() << "status changed " << status;
    setWindowTitle(status + QString::fromLatin1(" ") + mTitle);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = mpPlayer->bufferSpeed();
    QString s;
    if (bs > 1024*1024*1024)
        s = QString("%1G/s").arg(bs/1024.0/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024*1024)
        s = QString("%1M/s").arg(bs/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024)
        s = QString("%1K/s").arg(bs/1024.0, 6, 'f', 1);
    else
        s = QString("%1B/s").arg(bs, 6, 'f', 1);
    setWindowTitle(QString::fromLatin1("Buffering... %1% @%2 ").arg(percent*100.0, 5, 'f', 1).arg(s) + mTitle);
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer *vo = mpPlayer->renderer();
    //vo->setRenderRAWImage(true);
    //connect(vo,SIGNAL(onRenderRAWImage(const uchar*,int,int,int)),this,SLOT(renderedRAWImage(const uchar*,int,int,int)));
    VideoEQConfigPage::Engine e = mpVideoEQ->engine();
    if (e == VideoEQConfigPage::SWScale
            && vo->id() != VideoRendererId_X11 // X11 scales in the renderer
            ) {
        vo->forcePreferredPixelFormat(true);
        vo->setPreferredPixelFormat(VideoFormat::Format_RGB32);
    } else {
        vo->forcePreferredPixelFormat(false);
    }
    onBrightnessChanged(mpVideoEQ->brightness()*100.0);
    onContrastChanged(mpVideoEQ->contrast()*100.0);
    onHueChanged(mpVideoEQ->hue()*100.0);
    onSaturationChanged(mpVideoEQ->saturation()*100.0);
    //onGammaRGBChanged(mpVideoEQ->gammaRGB()*100.0);
    //onFilterSharpChanged(mpVideoEQ->filterSharp()*100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setBrightness(mpVideoEQ->brightness())) {
    //        mpPlayer->setBrightness(0);
    //    } else {
    //        vo->setBrightness(0);
    //        mpPlayer->setBrightness(b);
    //    }
    Q_UNUSED(b);
    VideoRenderer *vo = mpPlayer->renderer();
    vo->setBrightness(mpVideoEQ->brightness());
}

void MainWindow::onContrastChanged(int c)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setContrast(mpVideoEQ->contrast())) {
    //        mpPlayer->setContrast(0);
    //    } else {
    //        vo->setContrast(0);
    //        mpPlayer->setContrast(c);
    //    }
    Q_UNUSED(c);
    VideoRenderer *vo = mpPlayer->renderer();
    vo->setContrast(mpVideoEQ->contrast());
}

void MainWindow::onHueChanged(int h)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setHue(mpVideoEQ->hue())) {
    //        mpPlayer->setHue(0);
    //    } else {
    //        vo->setHue(0);
    //        mpPlayer->setHue(h);
    //    }
    Q_UNUSED(h);
    VideoRenderer *vo = mpPlayer->renderer();
    vo->setHue(mpVideoEQ->hue());
}

void MainWindow::onSaturationChanged(int s)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setSaturation(mpVideoEQ->saturation())) {
    //        mpPlayer->setSaturation(0);
    //    } else {
    //        vo->setSaturation(0);
    //        mpPlayer->setSaturation(s);
    //    }
    Q_UNUSED(s);
    VideoRenderer *vo = mpPlayer->renderer();
    vo->setSaturation(mpVideoEQ->saturation());
}

void MainWindow::onGammaRGBChanged(int g)
{
    Q_UNUSED(g);
    VideoRenderer *vo = mpPlayer->renderer();
    //vo->setGammaRGB(mpVideoEQ->gammaRGB());
}

void MainWindow::onFilterSharpChanged(int fs)
{
    Q_UNUSED(fs);
    VideoRenderer *vo = mpPlayer->renderer();
    //vo->setFilterSharp(mpVideoEQ->filterSharp());
}

void MainWindow::onCaptureConfigChanged()
{
    mpPlayer->videoCapture()->setCaptureDir(Config::instance().captureDir());
    mpPlayer->videoCapture()->setQuality(Config::instance().captureQuality());
    if (Config::instance().captureFormat().toLower() == QLatin1String("original")) {
        mpPlayer->videoCapture()->setOriginalFormat(true);
    } else {
        mpPlayer->videoCapture()->setOriginalFormat(false);
        mpPlayer->videoCapture()->setSaveFormat(Config::instance().captureFormat());
    }
    mpCaptureBtn->setToolTip(QString::fromLatin1("%1\n%2: %3\n%4: %5")
                             .arg(tr("Capture video frame"))
                             .arg(tr("Save to"))
                             .arg(mpPlayer->videoCapture()->captureDir())
                             .arg(tr("Format"))
                             .arg(Config::instance().captureFormat()));

}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (mpVideoFilter) {
        mpVideoFilter->uninstall();
        delete mpVideoFilter;
        mpVideoFilter = 0;
    }
    mpVideoFilter = new LibAVFilterVideo(this);
    mpVideoFilter->setEnabled(Config::instance().avfilterVideoEnable());
    mpPlayer->installFilter(mpVideoFilter);
    mpVideoFilter->setOptions(Config::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (mpAudioFilter) {
        mpAudioFilter->uninstall();
        delete mpAudioFilter;
        mpAudioFilter = 0;
    }
    mpAudioFilter = new LibAVFilterAudio(this);
    mpAudioFilter->setEnabled(Config::instance().avfilterAudioEnable());
    mpAudioFilter->installTo(mpPlayer);
    mpAudioFilter->setOptions(Config::instance().avfilterAudioOptions());
}

void MainWindow::donate()
{
    //QDesktopServices::openUrl(QUrl("https://sourceforge.net/p/qtav/wiki/Donate%20%E6%8D%90%E8%B5%A0/"));
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("http://www.qtav.org/donate.html")));
}

void MainWindow::onBufferValueChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setBufferValue(Config::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
}

void MainWindow::setup()
{
    ConfigDialog::display();
}

void MainWindow::handleFullscreenChange()
{
    workaroundRendererSize();

    // workaround renderer display size for ubuntu
    tryShowControlBar();
    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
}

void MainWindow::toggoleSubtitleEnabled(bool value)
{
    mpSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    mpSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a subtitle file"));
    if (file.isEmpty())
        return;
    mpSubtitle->setFile(file);
}

void MainWindow::setSubtitleCharset(const QString &charSet)
{
    Q_UNUSED(charSet);
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(const QString &value)
{
    Q_UNUSED(value)
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::changeClockType(QAction *action)
{
    action->setChecked(true);
    int value = action->data().toInt();
    if (value < 0) {
        mpPlayer->masterClock()->setClockAuto(true);
        // TODO: guess clock type
        return;
    }
    mpPlayer->masterClock()->setClockAuto(false);
    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::setClockType()
{
    if (isFileImgageSequence() && Config::instance().forceVideoClockI()){
        qDebug()<<"setClockType for ImgageSequence force AVClock::VideoClock";
        mpPlayer->masterClock()->setClockAuto(false);
        mpPlayer->masterClock()->setClockType(AVClock::VideoClock);
    }else{
        qDebug()<<"setClockType for Video from Menu";
        if (mpClockMenuAction) {
            QAction *a = mpClockMenuAction->checkedAction();
            if (a){
                int value = mpClockMenuAction->checkedAction()->data().toInt();
                qDebug()<<"setClockType Video object action from menu action "<<value;
                if (value < 0) {
                    mpPlayer->masterClock()->setClockAuto(true);
                    // TODO: guess clock type
                    return;
                }else{
                    mpPlayer->masterClock()->setClockAuto(false);
                    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
                }
            }
        }
    }
}

void MainWindow::syncVolumeUi(qreal value)
{
    const int v(value/kVolumeInterval);
    if (mpVolumeSlider->value() == v)
        return;
    mpVolumeSlider->setValue(v);
}

void MainWindow::workaroundRendererSize()
{
    if (!mpRenderer)
        return;
    QSize s = rect().size();
    //resize(QSize(s.width()-1, s.height()-1));
    //resize(s); //window resize to fullscreen size will create another fullScreenChange event
    mpRenderer->widget()->resize(QSize(s.width()+1, s.height()+1));
    mpRenderer->widget()->resize(s);
}

void MainWindow::loadRemoteUrlPresset(const QString& url){
    QString lurl=url;
    if (!lurl.contains("://") || lurl.startsWith("file://")) {
        lurl=QFileInfo(lurl).absolutePath()+"/"+QFileInfo(lurl).baseName()+".config";
    }
    qDebug("MainWindow::loadRemoteUrlPresset url: %s, lurl: %s",qPrintable(url),qPrintable(lurl));
    if (lurl.startsWith(XUNOserverUrl,Qt::CaseInsensitive)){
        QString surl=XUNOpresetUrl;
        QByteArray ba;
        ba.append("m="+lurl.remove(XUNOserverUrl+"/",Qt::CaseInsensitive));
        surl.append("q="+ba.toBase64());
        qDebug("MainWindow::openUrl surl: %s",qPrintable(surl));
        mpVideoEQ->setRemoteUrlPresset(surl);
        mpVideoEQ->getRemotePressets();
    }else if (lurl.endsWith(".config")){
        mpVideoEQ->setRemoteUrlPresset(lurl);
        mpVideoEQ->getLocalPressets();
    }
}

void MainWindow::reSizeByMovie()
{
    if (isFullScreen()) return;
    QSize t=mpRenderer->rendererSize();
    Statistics st=mpPlayer->statistics();
    if (st.video_only.width>0 && st.video_only.width>0 && mPlayerScale>0 ){ //(t.width()+t.height())==0
        t.setWidth(st.video_only.width*mPlayerScale);
        t.setHeight(st.video_only.height*mPlayerScale);
    }
    if (t.isValid() && (!t.isNull())) {
        resize(t);
        //        if (Config::instance().advancedFilterEnabled()){
        //           //mpRenderer->widget()->move(st.video_only.width-1,st.video_only.height-1);
        //        }
    }
}

void MainWindow::onClickXunoBrowser(QUrl url){
    if (url.isValid()) play(url.toString());
}

void MainWindow::onFullScreen(){
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::setPlayerPosFromRepeat(){
    tuneRepeatMovieDuration();
    if (mpRepeatEnableAction->isChecked()){
        qint64 RA=QTime(0, 0, 0).msecsTo(mpRepeatA->time());
        qint64 RB=QTime(0, 0, 0).msecsTo(mpRepeatB->time());
        if (RB>RA){
            if (RA>=mpPlayer->mediaStartPosition() && RA<=mpPlayer->mediaStopPosition()) {
                mpPlayer->setStartPosition(RA);
            }
            if (RB>=mpPlayer->mediaStartPosition() && RB<=mpPlayer->mediaStopPosition()) {
                mpPlayer->setStopPosition(RB);
            }
        }
    }
}

void MainWindow::customfpsChanged(double n){
    mCustomFPS=n;
    if (mpImgSeqExtract) mpImgSeqExtract->setFPS(n);
}
void MainWindow::tuneRepeatMovieDuration(){
    mpImageSequence->setMovieDuration(mpPlayer->mediaStopPosition());
}

bool MainWindow::isFileImgageSequence(){
    return (QDir::toNativeSeparators(mFile).contains("%0") && mFile.contains("d."));
}

bool MainWindow::applyCustomFPS(){
    bool ret=false; // return true if custom fps was applied
    if (isFileImgageSequence() && (mCustomFPS>0)){
        QVariantHash tmp;
        if (Config::instance().avformatOptionsEnabledI()) tmp=Config::instance().avformatOptionsI();
        tmp["framerate"]=mCustomFPS;
        mpPlayer->setOptionsForFormat(tmp);
        ret=true;
    }else{
        customfpsChanged(0.);
    }
    return ret;
}

void MainWindow::RepeatLoopChanged(int i){
    bool checked=(i==Qt::Checked);
    mpRepeatLoop->setChecked(i);
    qDebug()<<"RepeatLoopChanged:"<<i;
    mpRepeatBox->setValue(checked?-1:1);
    mpRepeatBox->setVisible(!checked);
    QLayout *layout = mpRepeatBox->parentWidget()->layout();
    QHBoxLayout *layout2 = layout->findChild<QHBoxLayout *>("TimesLayout");
    if (layout2){
        for (int j = 0; j < layout2->count(); ++j){
            QWidget *widget =layout2->itemAt(j)->widget();
            if (widget){
                widget->setVisible(!checked);
            }
        }
    }
    bool ch=mpRepeatEnableAction->isChecked();
    mpRepeatEnableAction->setChecked(!ch);
    mpRepeatEnableAction->setChecked(ch);
}

void MainWindow::onPreviewEnabledChanged(){
    //qDebug()<<"onPreviewEnabledChanged"<<Config::instance().previewEnabled();
    if (m_preview && !Config::instance().previewEnabled()) {
        m_preview->close();
        delete m_preview;
        m_preview = 0;
        return;
    }
}

void MainWindow::onImageSequenceConfig()
{
    int ret;
    bool state=!mpPlayer->isPaused() && mpPlayer->isPlaying();
    if (mpImageSequence) {
        if (state) stopUnload();//togglePlayPause();
        mpImageSequence->setWindowFlags(Qt::WindowStaysOnTopHint);
        if (isFileImgageSequence()) mpImageSequence->setImageSequenceFileName(mFile);
        ret = mpImageSequence->exec();
        if (ret==QDialog::Rejected && state) togglePlayPause();
    }
    qDebug()<<"onImageSequenceConfig after show"<<ret;
}

void MainWindow::onImageSequenceToogledFrameExtractor(bool state)
{
    qDebug()<<"onImageSequenceToogledFrameExtractor "<<state;
    mpImgSeqExtract->setVisible(state);
    mpCurrent->setVisible(!state);
    mpEnd->setVisible(!state);
    if (detachedControl){
        QSize s=QSize(detachedControl->width(),state?detachedControl->maximumHeight():detachedControl->minimumHeight());
        detachedControl->resize(s);
    }

}

void MainWindow::analyeUsedFPS()
{
    Statistics st=mpPlayer->statistics();
    qDebug()<<"analyeUsedFPS"<<st.video.frame_rate<<mCustomFPS;
    if (mCustomFPS==0.)
        if (mpImgSeqExtract){
            //Statistics st=mpPlayer->statistics();
            if (st.video.frame_rate>0.){
                qDebug()<<"setFPS analyeUsedFPS video.frame_rate"<<st.video.frame_rate;
                mpImgSeqExtract->setFPS(st.video.frame_rate);
            }
        }
}

void MainWindow::installAdvancedFilter()
{
    qDebug()<<"MainWindow::installAdvancedFilter";
    if (Config::instance().advancedFilterEnabled()){

        mpvPlayerWindow = new QWidget(this);
        mpvPlayerWindow->setWindowTitle(tr("XunoPlayer MPV view"));
        //mpvPlayerWindow->setWindowFlags(Qt::Dialog);
        //mpvPlayerWindow->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint & ~Qt::WindowMinMaxButtonsHint & Qt::CustomizeWindowHint);
        mpvPlayerWindow->setMinimumHeight(125);//785
        mpvPlayerWindow->setMinimumHeight(55);
        mpvPlayerWindow->setStyleSheet("background-color:black;");
        //mpvPlayerWindow->move(0,0);


        if (mpAdvancedFilter==0 && mpPlayer){
            mpAdvancedFilter = new AdvancedFilter(this);
            connect(mpAdvancedFilter, &AdvancedFilter::onSentFrame,this,&MainWindow::advacedFilterSentFrame);
            mpAdvancedFilter->setEnabled(true);
            mpAdvancedFilter->installTo(mpPlayer);
            mpAdvancedFilter->setPlayer(mpPlayer);
            if (!mpvpipe) {
                mpvpipe=new runmpvpipe();
                mpvpipe->setWidget(mpvPlayerWindow);
                connect(mpvpipe,SIGNAL(ready()),this,SLOT(runMpvPlayerRunned()));
                connect(mpvpipe,SIGNAL(finished(int)),this,SLOT(runMpvPlayerFinished(int)));
            }
            mpAdvancedFilter->setMpvPipe(mpvpipe);
        }

        if (mpRenderer){
            QWidget *r = mpRenderer->widget();
            //release old renderer and add new
            if (r && mpvPlayerWindow && mpPlayerLayout) {
                mpvPlayerWindow->resize(r->size());
                mpPlayerLayout->replaceWidget(r,mpvPlayerWindow);
                r->setParent(mpvPlayerWindow);
                r->move(0,0);
                //r->move(-r->size().width()-1,r->size().height()-1);
            }
        }
        mpvPlayerWindow->show();

        //mpPlayerLayout->addWidget(mpvPlayerWindow);
        // mpvPlayerWindow->raise();

    }

}

void MainWindow::installShaderXuno()
{
    if (mpRenderer && mpRenderer->opengl()){
        if (shaderXuno==nullptr) shaderXuno=new ShaderFilterXuno();
        if (shaderXuno!=nullptr) {
            shaderXuno->setGammaValue(0.f);
            shaderXuno->setSharpValue(0.f);
            mpRenderer->opengl()->setUserShader(shaderXuno);
        }
    }
}

void MainWindow::runMpvPlayer()
{
    Statistics st=mpPlayer->statistics();
    qreal fps=st.video.frame_rate;
#define useFrameGeometry 1
#if (useFrameGeometry)
    int frameW=st.video_only.width;
    int frameH=st.video_only.height;
#else
    int frameW=mpPlayer->renderer()->rendererWidth();
    int frameH=mpPlayer->renderer()->rendererHeight();
#endif
    //    const int bytesPerPixel=4;
    //    int framebytes=frameW*frameH*bytesPerPixel;
    //    int serverPort=8888;

    if (mpvpipe){
        mpvpipe->setFameInfo(frameW,frameH,fps);
        if (mpvpipe->runApp()){
            //mpvpipe->moveMpvApp();
        }
    }
    //    QString mpvparam;//="--vo-defaults=opengl:scale=ewa_lanczossharp:cscale=haasnsoft:dscale=mitchell:target-prim=bt.709:target-trc=srgb:scaler-resizes-only:no-deband:prescale-passes=2:prescale-downscaling-threshold=1.6:prescale=superxbr:superxbr-sharpness=0.7";
    //    //mpv.com "tcp://localhost:8888" --cache=no --demuxer=rawvideo --demuxer-rawvideo-mp-format=bgra --demuxer-rawvideo-size=1228800 --demuxer-rawvideo-fps=25  --demuxer-rawvideo-w=640 --demuxer-rawvideo-h=480 --no-audio
    //    mpvparam = QString("tcp://127.0.0.1:%1 --cache=no --demuxer=rawvideo --demuxer-rawvideo-mp-format=bgra --demuxer-rawvideo-size=%2 --demuxer-rawvideo-fps=%3  --demuxer-rawvideo-w=%4 --demuxer-rawvideo-h=%5 --no-audio").arg(serverPort).arg(framebytes).arg(fps).arg(frameW).arg(frameH);
    //    qDebug()<<"Start mpv.com"<<mpvparam;
    //    QProcess::startDetached("mpv.com", mpvparam.split(" "),QApplication::applicationDirPath(),&mpvPlayerPorcessId);
    //    qDebug()<<"mpv.com mpvPlayerPorcessId"<<mpvPlayerPorcessId;

}

void MainWindow::runMpvPlayerStop()
{
    if (mpvpipe){
        //if (mpvPlayerWindow1) mpvPlayerWindow1->close();
        mpvpipe->closeApp();
    }
}

void MainWindow::runMpvPlayerRunned()
{
    qDebug()<<"MainWindow::runMpvPlayerRunned";
}

void MainWindow::runMpvPlayerFinished(int c)
{
    qDebug()<<"MainWindow::runMpvPlayerFinished"<<c;
}

void MainWindow::advacedFilterSentFrame()
{
    //qDebug()<<"MainWindow::advacedFilterSentFrame";
    if (mpvpipe && !mpvpipe->getMovedApp()) {
        QWidget *r=nullptr;
        r=mpvpipe->moveMpvApp();
        if (r){
            mpvPlayerWindow1=r;
            //mpPlayerLayout->replaceWidget(r,mpvPlayerWindow);
            //r->deleteLater();
        }
    }
}

QString MainWindow::XUNO_QtAV_Version_String()
{
    return XUNO_QTAV_VERSION_STR;
}

QString  MainWindow::XUNO_QtAV_Version_String_Long()
{
    return XUNO_QTAV_VERSION_STR_LONG;
}

