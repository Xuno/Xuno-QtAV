#include "ImageSequenceConfigPage.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtCore>
#include <QRegularExpression>

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
    mpFpsBox = new QDoubleSpinBox(0);
    mpFpsBox->setMinimum(0.1);
    mpFpsBox->setMaximum(200);
    mpFpsBox->setValue(25);
    mpFpsBox->setSingleStep(1);
    mpFpsBox->setToolTip("frames per second.");
    connect(mpFpsBox, SIGNAL(valueChanged(double)), SLOT(setFPS(double)));
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
    QStringList files = QFileDialog::getOpenFileNames(parentWidget()->parentWidget(), tr("Select one or more media file"),"",tr("Image Files (*.png *.jpg *.tif *.bmp)"));
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
    //qDebug()<<"playImgages :: getDigetsFilename :"<<digs;
    QString filename=fileinfo.absolutePath().append("/%0").append(QString("%1d.").arg(digs)).append(fileinfo.suffix());
    //qDebug()<<"playImgages :: filename :"<<filename;
    if (digs==0) return;
    emit customfpsChanged(fps);
    if (startPos) emit toggleRepeat(true);
    playing_start=true;
    emit play(filename);
}

void ImageSequenceConfigPage::setFPS(double n){
    fps = n;
    if (n) emit stop();
    double step;
    if (fps>1){
      step=1;
    }else if (fps<=1){
      step=0.1;
    }
    mpFpsBox->setSingleStep(step);
    if (fps==1.1) fps=2;
    else if (fps>1.1) fps=(double)qRound(fps);
    mpFpsBox->setValue(fps);
    emit customfpsChanged(fps);
    calculatePos();
}



void ImageSequenceConfigPage::setStartFrame(quint32 n){
    startFrame = n;
    if (startFrame) {
        emit toggleRepeat(true);
        calculatePos();
    }
}

void ImageSequenceConfigPage::setEndFrame(quint32 n){
  //qDebug()<<"ImageSequenceConfigPage::setEndFrame"<<n;
  mpTotalFramesBox->setValue(n-startFrame+1);
  setTotalFrames(n-startFrame+1);
}

void ImageSequenceConfigPage::setTotalFrames(int n){
  //qDebug()<<"ImageSequenceConfigPage::setTotalFrames"<<n;
  frames = n;
  calculatePos();
}

void ImageSequenceConfigPage::calculatePos(){
     qDebug()<<"calculatePos frames"<<frames;
    if (frames<1 || fps<1)
        return;
    startPos=startFrame*(1000UL/fps);
    stopPos=startPos + frames*(1000UL/fps);
    QTime mRepeatA = QTime(0, 0, 0).addMSecs(startPos);
    QTime mRepeatB = QTime(0, 0, 0).addMSecs(stopPos);
    if (startPos) emit repeatAChanged(mRepeatA);
    if (stopPos) emit repeatBChanged(mRepeatB);
    qDebug()<<"calculatePos mRepeatA:"<<mRepeatA<<",mRepeatB:"<<mRepeatB;
}

void ImageSequenceConfigPage::setMovieDuration(qint64 d){
    if (playing_start) {
        QTime mRepeatB = QTime(0, 0, 0).addMSecs(d);
        if (stopPos==0) emit repeatBChanged(mRepeatB);
        playing_start=false;
    }
}

