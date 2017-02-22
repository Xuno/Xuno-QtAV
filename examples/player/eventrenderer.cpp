#include "eventrenderer.h"

EventRenderer::EventRenderer(MainWindow *parent) :
    QObject(parent),
    mMainWindow(parent)
{
}

bool EventRenderer::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
           //qDebug("EventRender::eventFilter %s", obj->metaObject()->className());
           bool ret=QObject::eventFilter(obj, event);
           if (mMainWindow) mMainWindow->calcToUseSuperResolution();
           return ret;
       } else {
           // standard event processing
           return QObject::eventFilter(obj, event);
       }
}
