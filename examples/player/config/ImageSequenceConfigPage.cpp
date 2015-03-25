#include "ImageSequenceConfigPage.h"


ImageSequenceConfigPage::ImageSequenceConfigPage(QWidget *parent) :
   QDialog(parent)
  ,startPos(0)
  ,stopPos(0)
  ,fps(25)
  ,startFrame(0)
  ,frames(0)
  ,fileinfo(0)
{
    setWindowTitle("Image sequence config page");
    setMaximumSize(QSize(650, 16777215));

    QVBoxLayout *verticalLayoutWidget = new QVBoxLayout(this);
    setLayout(verticalLayoutWidget);

    QGroupBox *groupBox1 = new QGroupBox(this);
    groupBox1->setObjectName(QStringLiteral("groupBox1"));
    groupBox1->setTitle(tr("Play Image Sequence"));

    QVBoxLayout *vb = new QVBoxLayout(groupBox1);
    //setLayout(vb);


    //hb->addWidget(pFPSLabel);
    //hb->addWidget(mpFpsBox);

    //vb->addLayout(hb);

    QLabel *pTotalFramesLabel = new QLabel(tr("Total frames:"));
    mpTotalFramesBox = new QSpinBox(0);
    mpTotalFramesBox->setRange(0, 9999999UL);
    mpTotalFramesBox->setValue(0);
    //mpTotalFramesBox->setMinimumWidth(50);
    mpTotalFramesBox->setToolTip("total frames - 0: " + tr("infinity"));
    connect(mpTotalFramesBox, SIGNAL(valueChanged(int)), SLOT(setTotalFrames(int)));
//    QHBoxLayout *hb = new QHBoxLayout;
//    hb->addWidget(pTotalFramesLabel);
//    hb->addWidget(mpTotalFramesBox);
//    vb->addLayout(hb);
//    mpSelectImgButton = new QPushButton(tr("Select Image"));
//    connect(mpSelectImgButton, SIGNAL(clicked()), SLOT(onSelectImgages()));
//    vb->addWidget(mpSelectImgButton,Qt::AlignHCenter);

    //-------------------------
//    QWidget *tInput = new QWidget();
//    tInput->setObjectName(QStringLiteral("tInput"));
//    QVBoxLayout *vlBatchInput = new QVBoxLayout(tInput);
//    vlBatchInput->setSpacing(6);
//    vlBatchInput->setContentsMargins(11, 11, 11, 11);
//    vlBatchInput->setObjectName(QStringLiteral("vlBatchInput"));
    vb->setSpacing(6);
    vb->setContentsMargins(11, 11, 11, 11);

    QLineEdit *InputPath = new QLineEdit();
    InputPath->setObjectName(QStringLiteral("InputPath"));
    vb->addWidget(InputPath);

    QHBoxLayout *horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
    QPushButton *btSelectInputPath = new QPushButton();
    btSelectInputPath->setObjectName(QStringLiteral("btSelectInputPath"));
    btSelectInputPath->setText(tr("Select Path", 0));
    horizontalLayout_2->addWidget(btSelectInputPath);
    connect(btSelectInputPath, SIGNAL(clicked()), SLOT(onSelectImgages()));

    QSpacerItem *horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer_3);

    QLabel *pFPSLabel = new QLabel(tr("FPS:"));
    QHBoxLayout *hb = new QHBoxLayout;
    mpFpsBox = new QDoubleSpinBox(0);
    mpFpsBox->setMinimum(0.1);
    mpFpsBox->setMaximum(500);
    mpFpsBox->setValue(25);
    mpFpsBox->setSingleStep(1);
    mpFpsBox->setToolTip("frames per second.");
    connect(mpFpsBox, SIGNAL(valueChanged(double)), SLOT(setFPS(double)));
    horizontalLayout_2->addWidget(pFPSLabel);
    horizontalLayout_2->addWidget(mpFpsBox);

    QSpacerItem *horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_4);

    QLabel *label_5 = new QLabel();
    label_5->setText(tr("Color Depth:", 0));
    label_5->setObjectName(QStringLiteral("label_5"));

    horizontalLayout_2->addWidget(label_5);

    QComboBox *cbColorTypeInput = new QComboBox();
    cbColorTypeInput->setObjectName(QStringLiteral("cbColorTypeInput"));
    cbColorTypeInput->clear();
    cbColorTypeInput->insertItems(0, QStringList()
     << tr("8-bit RGB", 0)
     << tr("10-bit RGB", 0)
     << tr("12-bit RGB", 0)
     << tr("14-bit RGB", 0)
     << tr("16-bit RGB", 0)
    );
    horizontalLayout_2->addWidget(cbColorTypeInput);

    QSpacerItem *horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_7);

    QLabel *labelInputScale = new QLabel();
    labelInputScale->setObjectName(QStringLiteral("InputScale"));
    labelInputScale->setText(tr(" Scale:"));
    QDoubleSpinBox *InputScale = new QDoubleSpinBox();
    InputScale->setMinimum(0.1);
    InputScale->setMaximum(2.0);
    InputScale->setSingleStep(0.1);
    InputScale->setValue(1.0);

    horizontalLayout_2->addWidget(labelInputScale);
    horizontalLayout_2->addWidget(InputScale);

    vb->addLayout(horizontalLayout_2);

    QHBoxLayout *hlStart = new QHBoxLayout();
    hlStart->setSpacing(6);
    hlStart->setObjectName(QStringLiteral("hlStart"));
    QLabel *label_2 = new QLabel();
    label_2->setObjectName(QStringLiteral("label_2"));
    label_2->setText(tr("Start Frame:", 0));

    hlStart->addWidget(label_2);

    QSpinBox *InputStartFrame = new QSpinBox();
    InputStartFrame->setObjectName(QStringLiteral("InputStartFrame"));
    InputStartFrame->setEnabled(false);
    InputStartFrame->setMinimum(1);
    InputStartFrame->setMaximum(999999999);
    InputStartFrame->setValue(1);

    hlStart->addWidget(InputStartFrame);

    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart->addItem(horizontalSpacer_2);

    QLabel *label_28 = new QLabel();
    label_28->setObjectName(QStringLiteral("label_28"));
    label_28->setText(tr("End Frame:", 0));

    hlStart->addWidget(label_28);

    QSpinBox *InputEndFrame = new QSpinBox();
    InputEndFrame->setObjectName(QStringLiteral("InputEndFrame"));
    InputEndFrame->setEnabled(false);
    InputEndFrame->setMinimum(1);
    InputEndFrame->setMaximum(999999999);
    InputEndFrame->setValue(1);

    hlStart->addWidget(InputEndFrame);

    QSpacerItem *horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart->addItem(horizontalSpacer_5);

    QLabel *label_4 = new QLabel();
    label_4->setObjectName(QStringLiteral("label_4"));
    label_4->setText(tr("Total Frames:", 0));
    hlStart->addWidget(label_4);

    QSpinBox *InputTotalFrame = new QSpinBox();
    InputTotalFrame->setObjectName(QStringLiteral("InputTotalFrame"));
    InputTotalFrame->setEnabled(false);
    InputTotalFrame->setMinimum(1);
    InputTotalFrame->setMaximum(999999999);

    hlStart->addWidget(InputTotalFrame);

    QLabel *InputAllTotalFrame = new QLabel();
    InputAllTotalFrame->setObjectName(QStringLiteral("InputAllTotalFrame"));
    InputAllTotalFrame->setText(tr("of: NNNNN", 0));
    hlStart->addWidget(InputAllTotalFrame);

    QSpacerItem *horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlStart->addItem(horizontalSpacer_6);


    QCheckBox *checkLoop = new QCheckBox();
    checkLoop->setObjectName(QStringLiteral("Loop"));
    checkLoop->setText(tr("Loop"));
    hlStart->addWidget(checkLoop);

    vb->addLayout(hlStart);

    verticalLayoutWidget->addWidget(groupBox1);

    QGroupBox *groupBox2 = new QGroupBox(this);
    groupBox2->setObjectName(QStringLiteral("groupBox2"));
    groupBox2->setTitle(tr("Generate Image Sequence"));

    QVBoxLayout *vb2 = new QVBoxLayout(groupBox2);
    QCheckBox *checkBox = new QCheckBox(groupBox2);
    checkBox->setObjectName(QStringLiteral("checkBox"));
    checkBox->setText(tr("Enable Frame Extractor"));
    vb2->addWidget(checkBox);

    verticalLayoutWidget->addWidget(groupBox2);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    connect(buttonBox,SIGNAL(rejected()),SLOT(close()));

    verticalLayoutWidget->addWidget(buttonBox);

//    QLabel *lablelGenerate = new QLabel(tr("Generate Image Sequence"));
//    vb->addWidget(lablelGenerate);


}

void ImageSequenceConfigPage::onSelectImgages()
{
    QStringList files = QFileDialog::getOpenFileNames(parentWidget(), tr("Select one or more media file"),"",tr("Image Files (*.png *.jpg *.tif *.bmp)"));
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

