#include "ImageSequenceConfigPage.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>


ImageSequenceConfigPage::ImageSequenceConfigPage(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Image sequence config page");

    QVBoxLayout *vb = new QVBoxLayout;
    setLayout(vb);

    QLabel *pFPSLabel = new QLabel(tr("FPS:"));
    QHBoxLayout *hb = new QHBoxLayout;
    mpFpsBox = new QSpinBox(0);
    mpFpsBox->setMinimum(1);
    mpFpsBox->setValue(60);
    mpFpsBox->setSingleStep(5);
    mpFpsBox->setToolTip("frames per second.");
    connect(mpFpsBox, SIGNAL(valueChanged(int)), SLOT(setFPS(int)));
    hb->addWidget(pFPSLabel);
    hb->addWidget(mpFpsBox);

    vb->addLayout(hb);

    QLabel *pTotalFramesLabel = new QLabel(tr("Total frames:"));
    mpTotalFramesBox = new QSpinBox(0);
    mpTotalFramesBox->setRange(0, 999999);
    mpTotalFramesBox->setValue(0);
    //mpTotalFramesBox->setMinimumWidth(50);
    mpTotalFramesBox->setToolTip("total frames - 0: " + tr("infinity"));
    connect(mpTotalFramesBox, SIGNAL(valueChanged(int)), SLOT(setTotalFrames(int)));
    hb = new QHBoxLayout;
    hb->addWidget(pTotalFramesLabel);
    hb->addWidget(mpTotalFramesBox);

    vb->addLayout(hb);
    mpSelectImgButton = new QPushButton(tr("Select Image"));
    connect(mpSelectImgButton, SIGNAL(clicked()), SLOT(onSelectImgPress()));

    vb->addWidget(mpSelectImgButton,Qt::AlignHCenter);

}
