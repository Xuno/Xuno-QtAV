#include "eventrenderer.h"

EventRenderer::EventRenderer(MainWindow *parent) :
    QObject(parent),
    mMainWindow(parent)
{
}

bool EventRenderer::eventFilter(QObject *obj, QEvent *event)
{
    bool ret0=QObject::eventFilter(obj, event);
    if (waitPaint && (event->type() == QEvent::Paint)) {
        qDebug()<<"EventRender::eventFilter "<<obj->metaObject()->className()<<event->type();
        if (mMainWindow) mMainWindow->calcToUseSuperResolution();
        waitPaint=false;
    }else if (event->type() == QEvent::Resize) {
//      QResizeEvent* re=static_cast<QResizeEvent*>(event);
//      qDebug("EventRender::eventFilter %s", obj->metaObject()->className());
//      qDebug()<<re->size()<<re->oldSize();
        waitPaint=true;
    }
   return ret0;
}
