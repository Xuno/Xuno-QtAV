#include "ImageSequenceExtractControl.h"
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
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



ImgSeqExtractControl::ImgSeqExtractControl(QWidget *parent) :
    QWidget (parent)
{

    baseParentMaxHeight=parentWidget()->maximumHeight();

    verticalLayoutWidget=this;
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayoutWidget->setLayout(verticalLayout);
    verticalLayout->setSpacing(2);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    hlStart_6 = new QHBoxLayout();
    hlStart_6->setSpacing(0);
    hlStart_6->setObjectName(QStringLiteral("hlStart_6"));
    startTime = new QLabel(verticalLayoutWidget);
    startTime->setObjectName(QStringLiteral("startTime"));

    hlStart_6->addWidget(startTime);

    hs_start = new QSpacerItem(12, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_start);

    labelSF = new QLabel(verticalLayoutWidget);
    labelSF->setObjectName(QStringLiteral("labelSF"));

    hlStart_6->addWidget(labelSF);

    ImageSequenceStartFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceStartFrame->setObjectName(QStringLiteral("ImageSequenceStartFrame"));
    ImageSequenceStartFrame->setEnabled(false);
    ImageSequenceStartFrame->setMinimum(1);
    ImageSequenceStartFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceStartFrame);

    buttonSetStartFrame = new QToolButton(verticalLayoutWidget);
    buttonSetStartFrame->setObjectName(QStringLiteral("buttonSetStartFrame"));

    hlStart_6->addWidget(buttonSetStartFrame);

    hs_total = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_total);

    labelTF = new QLabel(verticalLayoutWidget);
    labelTF->setObjectName(QStringLiteral("labelTF"));

    hlStart_6->addWidget(labelTF);

    ImageSequenceTotalFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceTotalFrame->setObjectName(QStringLiteral("ImageSequenceTotalFrame"));
    ImageSequenceTotalFrame->setEnabled(false);
    ImageSequenceTotalFrame->setMinimum(1);
    ImageSequenceTotalFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceTotalFrame);

    hs_end = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_end);

    labelEF = new QLabel(verticalLayoutWidget);
    labelEF->setObjectName(QStringLiteral("labelEF"));

    hlStart_6->addWidget(labelEF);

    ImageSequenceEndFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceEndFrame->setObjectName(QStringLiteral("ImageSequenceEndFrame"));
    ImageSequenceEndFrame->setEnabled(false);
    ImageSequenceEndFrame->setMinimum(1);
    ImageSequenceEndFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceEndFrame);

    buttonSetEndFrame = new QToolButton(verticalLayoutWidget);
    buttonSetEndFrame->setObjectName(QStringLiteral("buttonSetEndFrame"));

    hlStart_6->addWidget(buttonSetEndFrame);

    horizontalSpacer_10 = new QSpacerItem(12, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

    hlStart_6->addItem(horizontalSpacer_10);

    endTime = new QLabel(verticalLayoutWidget);
    endTime->setObjectName(QStringLiteral("endTime"));

    hlStart_6->addWidget(endTime);


    verticalLayout->addLayout(hlStart_6);

    horizontalLayout_6 = new QHBoxLayout();
    horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
    btSelectOutputPath = new QPushButton(verticalLayoutWidget);
    btSelectOutputPath->setObjectName(QStringLiteral("btSelectOutputPath"));

    horizontalLayout_6->addWidget(btSelectOutputPath);

    OutputPath = new QLineEdit(verticalLayoutWidget);
    OutputPath->setObjectName(QStringLiteral("OutputPath"));

    horizontalLayout_6->addWidget(OutputPath);

    cb_OutputType = new QComboBox(verticalLayoutWidget);
    cb_OutputType->setObjectName(QStringLiteral("cb_OutputType"));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(cb_OutputType->sizePolicy().hasHeightForWidth());
    cb_OutputType->setSizePolicy(sizePolicy);
    cb_OutputType->setMinimumSize(QSize(60, 0));

    horizontalLayout_6->addWidget(cb_OutputType);

    cbColorTypeOutput = new QComboBox(verticalLayoutWidget);
    cbColorTypeOutput->setObjectName(QStringLiteral("cbColorTypeOutput"));

    horizontalLayout_6->addWidget(cbColorTypeOutput);


    verticalLayout->addLayout(horizontalLayout_6);

    hlPrefix_3 = new QHBoxLayout();
    hlPrefix_3->setObjectName(QStringLiteral("hlPrefix_3"));
    labelFilePrefix = new QLabel(verticalLayoutWidget);
    labelFilePrefix->setObjectName(QStringLiteral("labelFilePrefix"));

    hlPrefix_3->addWidget(labelFilePrefix);

    OutputFileNumberingPrefix = new QLineEdit(verticalLayoutWidget);
    OutputFileNumberingPrefix->setObjectName(QStringLiteral("OutputFileNumberingPrefix"));
    OutputFileNumberingPrefix->setMaxLength(50);

    hlPrefix_3->addWidget(OutputFileNumberingPrefix);

    labelFileSeparator = new QLabel(verticalLayoutWidget);
    labelFileSeparator->setObjectName(QStringLiteral("labelFileSeparator"));

    hlPrefix_3->addWidget(labelFileSeparator);

    OutputFileNumberingSeperator = new QLineEdit(verticalLayoutWidget);
    OutputFileNumberingSeperator->setObjectName(QStringLiteral("OutputFileNumberingSeperator"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(OutputFileNumberingSeperator->sizePolicy().hasHeightForWidth());
    OutputFileNumberingSeperator->setSizePolicy(sizePolicy1);
    OutputFileNumberingSeperator->setMinimumSize(QSize(0, 0));
    OutputFileNumberingSeperator->setMaximumSize(QSize(15, 16777215));
    OutputFileNumberingSeperator->setMaxLength(1);

    hlPrefix_3->addWidget(OutputFileNumberingSeperator, 0, Qt::AlignRight);

    buttonExtractFrames = new QPushButton(verticalLayoutWidget);
    buttonExtractFrames->setObjectName(QStringLiteral("buttonExtractFrames"));

    hlPrefix_3->addWidget(buttonExtractFrames);


    verticalLayout->addLayout(hlPrefix_3);

    retranslateUi();
    adjustSize();

    //QMetaObject::connectSlotsByName(Dialog);
}
void ImgSeqExtractControl::retranslateUi()
{
    startTime->setText(QApplication::translate("ImageSequenceExtract", "00:00:00.000", 0));
    labelSF->setText(QApplication::translate("ImageSequenceExtract", " Start Frames:", 0));
    buttonSetStartFrame->setText(QApplication::translate("ImageSequenceExtract", "Set", 0));
    labelTF->setText(QApplication::translate("ImageSequenceExtract", " Total Frames:", 0));
    labelEF->setText(QApplication::translate("ImageSequenceExtract", " End Frame:", 0));
    buttonSetEndFrame->setText(QApplication::translate("ImageSequenceExtract", "Set", 0));
    endTime->setText(QApplication::translate("ImageSequenceExtract", "00:00:00.000", 0));
    btSelectOutputPath->setText(QApplication::translate("ImageSequenceExtract", "Select Output Path", 0));
    cb_OutputType->clear();
    cb_OutputType->insertItems(0, QStringList()
                               << QApplication::translate("ImageSequenceExtract", "...", 0)
                               << QApplication::translate("ImageSequenceExtract", ".bmp", 0)
                               << QApplication::translate("ImageSequenceExtract", ".cr2", 0)
                               << QApplication::translate("ImageSequenceExtract", ".dng", 0)
                               << QApplication::translate("ImageSequenceExtract", ".exr", 0)
                               << QApplication::translate("ImageSequenceExtract", ".dxp", 0)
                               << QApplication::translate("ImageSequenceExtract", ".jp2", 0)
                               << QApplication::translate("ImageSequenceExtract", ".png", 0)
                               << QApplication::translate("ImageSequenceExtract", ".jpg", 0)
                               << QApplication::translate("ImageSequenceExtract", ".tga", 0)
                               << QApplication::translate("ImageSequenceExtract", ".tiff", 0)
                               << QApplication::translate("ImageSequenceExtract", ".tif", 0)
                               );
    cbColorTypeOutput->clear();
    cbColorTypeOutput->insertItems(0, QStringList()
                                   << QApplication::translate("ImageSequenceExtract", "8-bit RGB", 0)
                                   << QApplication::translate("ImageSequenceExtract", "10-bit RGB", 0)
                                   << QApplication::translate("ImageSequenceExtract", "12-bit RGB", 0)
                                   << QApplication::translate("ImageSequenceExtract", "14-bit RGB", 0)
                                   << QApplication::translate("ImageSequenceExtract", "16-bit RGB", 0)
                                   );
    labelFilePrefix->setText(QApplication::translate("ImageSequenceExtract", " Prefix", 0));
    labelFileSeparator->setText(QApplication::translate("ImageSequenceExtract", " Separator", 0));
    buttonExtractFrames->setText(QApplication::translate("ImageSequenceExtract", "Extract Frames", 0));
} // retranslateUi



ImgSeqExtractControl::~ImgSeqExtractControl()
{

}

void ImgSeqExtractControl::RecalculateParentHeight(bool visible)
{
  if (parentWidget()){
      parentWidget()->setMaximumHeight(baseParentMaxHeight+(visible?height():0));
  }
}

void ImgSeqExtractControl::setVisible(bool visible)
{
    RecalculateParentHeight(visible);
    QWidget::setVisible(visible);
}

void ImgSeqExtractControl::setStartTime(QTime time)
{
    startTime->setText(time.toString("hh:mm:ss.zzz"));
}

void ImgSeqExtractControl::setEndTime(QTime time)
{
    endTime->setText(time.toString("hh:mm:ss.zzz"));
}
