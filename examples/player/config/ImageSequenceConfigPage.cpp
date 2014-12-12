#include "ImageSequenceConfigPage.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCore>
#include <QRegularExpression>
#define AVDEMUXFPS_READY 0 //TODO need change when changed AV demuxer option to video dictionary framrate

ImageSequenceConfigPage::ImageSequenceConfigPage(QWidget *parent) :
    QWidget(parent)
  ,startPos(0)
  ,stopPos(0)
  ,fps(25)
  ,startFrame(0)
  ,frames(0)
  ,fileinfo(0)
{
    setWindowTitle("Image sequence config page");

    QVBoxLayout *vb = new QVBoxLayout;
    setLayout(vb);

    QLabel *pFPSLabel = new QLabel(tr("FPS:"));
    QHBoxLayout *hb = new QHBoxLayout;
    mpFpsBox = new QSpinBox(0);
    mpFpsBox->setMinimum(1);
    mpFpsBox->setMaximum(120);
    mpFpsBox->setValue(25);
    mpFpsBox->setSingleStep(5);
    mpFpsBox->setToolTip("frames per second.");
#if (!AVDEMUXFPS_READY)
    mpFpsBox->setDisabled(true);
#endif
    connect(mpFpsBox, SIGNAL(valueChanged(int)), SLOT(setFPS(int)));
    hb->addWidget(pFPSLabel);
    hb->addWidget(mpFpsBox);

    vb->addLayout(hb);

    QLabel *pTotalFramesLabel = new QLabel(tr("Total frames:"));
    mpTotalFramesBox = new QSpinBox(0);
    mpTotalFramesBox->setRange(0, 9999999UL);
    mpTotalFramesBox->setValue(0);
    //mpTotalFramesBox->setMinimumWidth(50);
    mpTotalFramesBox->setToolTip("total frames - 0: " + tr("infinity"));
    connect(mpTotalFramesBox, SIGNAL(valueChanged(int)), SLOT(setTotalFrames(int)));
    hb = new QHBoxLayout;
    hb->addWidget(pTotalFramesLabel);
    hb->addWidget(mpTotalFramesBox);
    vb->addLayout(hb);

    mpSelectImgButton = new QPushButton(tr("Select Image"));
    connect(mpSelectImgButton, SIGNAL(clicked()), SLOT(onSelectImgages()));
    vb->addWidget(mpSelectImgButton,Qt::AlignHCenter);
}

void ImageSequenceConfigPage::onSelectImgages()
{
    QStringList files = QFileDialog::getOpenFileNames(parentWidget()->parentWidget(), tr("Select one or more media file"),"",tr("Image Files (*.png *.jpg *.tif*)"));
    if (files.isEmpty())
        return;
    if (files.size()>1) mpTotalFramesBox->setValue(files.size());
    fileinfo = QFileInfo(files.at(0));
    analyzeFilename();
    calculatePos();
    playImgages();
    return;
}

void ImageSequenceConfigPage::analyzeFilename(){
    bool ok = false;
    QString f=fileinfo.baseName();
    qint32 fr=f.toLong(&ok,10);
    if (fr>0 && ok) startFrame=fr;
    qDebug()<<"analyzeFilename :: startFrame :"<<startFrame;
}

int ImageSequenceConfigPage::getDigetsFilename() {
    int i=0;
    QRegularExpression re("(\\d+)$");
    QRegularExpressionMatch match = re.match(fileinfo.baseName());
    if (match.hasMatch()) {
        i=match.captured(0).count();
    }
    return i;
}


void ImageSequenceConfigPage::playImgages()
{
    calculatePos();
    int digs=getDigetsFilename();
    qDebug()<<"playImgages :: getDigetsFilename :"<<digs;
    QString filename=fileinfo.absolutePath().append("/%0").append(QString("%1d.").arg(digs)).append(fileinfo.suffix());
    qDebug()<<"playImgages :: filename :"<<filename;
    if (digs==0) return;
    if (startPos>0 && stopPos>0) emit toggleRepeat(true);
    emit play(filename);
}

void ImageSequenceConfigPage::setFPS(int n){
Q_UNUSED(n);
#if (AVDEMUXFPS_READY)
      fps = n;
#endif
}

void ImageSequenceConfigPage::setTotalFrames(int n){
  frames = n;
  calculatePos();
}

void ImageSequenceConfigPage::calculatePos(){
    if (frames<1)
        return;
    startPos=startFrame*(1000UL/fps);
    stopPos=startPos + frames*(1000UL/fps);
    qDebug()<<"ImageSequenceConfigPage :: startPos:"<<startPos<<", stopPos:"<<stopPos;
    QTime mRepeatA = QTime(0, 0, 0).addMSecs(startPos);
    QTime mRepeatB = QTime(0, 0, 0).addMSecs(stopPos);
    emit repeatAChanged(mRepeatA);
    emit repeatBChanged(mRepeatB);
}


