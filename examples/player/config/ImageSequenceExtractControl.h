#ifndef IMGSEQEXTRACTCONTROL_H
#define IMGSEQEXTRACTCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDebug>


class ImgSeqExtractControl : public QWidget
{
    Q_OBJECT
public:
    explicit ImgSeqExtractControl(QWidget *parent=0);
    ~ImgSeqExtractControl();

    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hlStart_6;
    QLabel *startTime;
    QSpacerItem *hs_start;
    QLabel *labelSF;
    QSpinBox *ImageSequenceStartFrame;
    QToolButton *buttonSetStartFrame;
    QSpacerItem *hs_total;
    QLabel *labelTF;
    QSpinBox *ImageSequenceTotalFrame;
    QSpacerItem *hs_end;
    QLabel *labelEF;
    QSpinBox *ImageSequenceEndFrame;
    QToolButton *buttonSetEndFrame;
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
    void setVisible(bool visible) Q_DECL_OVERRIDE;

private:
    void retranslateUi();
    int baseParentMaxHeight;
    void RecalculateParentHeight(bool visible);



};

#endif // IMGSEQEXTRACTCONTROL_H
