#ifndef EVENTRENDERER_H
#define EVENTRENDERER_H

#include <QObject>
#include <QEvent>
#include <QDebug>
#include <QString>
#include "MainWindow.h"

class EventRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EventRenderer(MainWindow *parent = 0);

signals:

public slots:
protected:
   virtual bool eventFilter(QObject *obj, QEvent *event);
private:
  MainWindow *mMainWindow;
  bool waitPaint=false;
};

#endif // EVENTRENDERER_H
