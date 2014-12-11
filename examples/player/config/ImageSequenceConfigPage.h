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
    void openFiles();

signals:

public slots:
    void onSelectImgages();

private:
    QSpinBox *mpFpsBox, *mpTotalFramesBox;
    QPushButton *mpSelectImgButton;

};

#endif // IMAGESEQUENCECONFIGPAGE_H
