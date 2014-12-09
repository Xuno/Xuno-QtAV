#ifndef IMAGESEQUENCECONFIGPAGE_H
#define IMAGESEQUENCECONFIGPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>


class QPushButton;
class QSpinBox;

class ImageSequenceConfigPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSequenceConfigPage(QWidget *parent = 0);

signals:

public slots:

private:
    QSpinBox *mpFpsBox, *mpTotalFramesBox;
    QPushButton *mpSelectImgButton;

};

#endif // IMAGESEQUENCECONFIGPAGE_H
