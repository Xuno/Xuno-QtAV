/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <QApplication>
#include <QtDebug>
#include <QtCore/QDir>
#include <QMessageBox>

#include <QtAV/AVPlayer.h>
#include <QtAV/VideoOutput.h>
#include <QtAVWidgets>
#include "MainWindow.h"
#include "../common/common.h"

using namespace QtAV;
static const struct {
    const char* name;
    VideoRendererId id;
} vid_map[] = {
{ "opengl", VideoRendererId_OpenGLWidget },
{ "gl", VideoRendererId_GLWidget2 },
{ "d2d", VideoRendererId_Direct2D },
{ "gdi", VideoRendererId_GDI },
{ "xv", VideoRendererId_XV },
{ "x11", VideoRendererId_X11 },
{ "qt", VideoRendererId_Widget },
{ 0, 0 }
};

VideoRendererId rendererId_from_opt_name(const QString& name) {
    for (int i = 0; vid_map[i].name; ++i) {
        if (name == QLatin1String(vid_map[i].name))
            return vid_map[i].id;
    }
#ifndef QT_NO_OPENGL
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    return VideoRendererId_OpenGLWidget; // qglwidget is not suitable for android
#endif
    return VideoRendererId_GLWidget2;
#endif
    return VideoRendererId_Widget;
}

int main(int argc, char *argv[])
{
    qDebug() << aboutQtAV_PlainText();
    Config::setName(QString::fromLatin1("Player"));
    QOptions options = get_common_options();
    options.add(QString::fromLatin1("player options"))
            ("ffmpeg-log",  QString(), QString::fromLatin1("ffmpeg log level. can be: quiet, panic, fatal, error, warn, info, verbose, debug. this can override env 'QTAV_FFMPEG_LOG'"))
            ("-vo",
#ifndef QT_NO_OPENGL
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
             QString::fromLatin1("opengl")
#else
             QString::fromLatin1("gl")
#endif
#else
             QString::fromLatin1("qt")
#endif
             , QString::fromLatin1("video renderer engine. can be gl, qt, d2d, gdi, xv, x11."))
            ;
    options.parse(argc, argv);
    do_common_options_before_qapp(options);

    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    qDebug() <<a.arguments();
    a.setApplicationName(QString::fromLatin1("Player"));
//    a.setApplicationDisplayName(QString::fromLatin1("QtAV Player"));
    QDir::setCurrent(qApp->applicationDirPath());

    do_common_options(options);
    set_opengl_backend(options.option(QString::fromLatin1("gl")).value().toString(), a.arguments().first());
    load_qm(QStringList() << QString::fromLatin1("player"), options.value(QString::fromLatin1("language")).toString());
    QtAV::setFFmpegLogLevel(options.value(QString::fromLatin1("ffmpeg-log")).toByteArray());

    QOption op = options.option(QString::fromLatin1("vo"));
    QString vo = op.value().toString();
    if (!op.isSet()) {
        QString exe(a.arguments().at(0));
        int i = exe.lastIndexOf(QLatin1Char('-'));
        if (i > 0) {
            vo = exe.mid(i+1, exe.indexOf(QLatin1Char('.')) - i - 1);
        }
    }
    qDebug("vo: %s", vo.toUtf8().constData());
    MainWindow window;
    window.setProperty("rendererId", rendererId_from_opt_name(vo.toLower()));
    window.show();
    QString title = QString::fromLatin1("XunoPlayer %1 http://www.xuno.com").arg(XUNO_QtAV_Version_String_Long());
    window.setWindowTitle(title);
    AppEventFilter ae(&window);
    qApp->installEventFilter(&ae);

    int x = window.x();
    int y = window.y();
    op = options.option(QString::fromLatin1("width"));
    int w = op.value().toInt();
    op = options.option(QString::fromLatin1("height"));
    int h = op.value().toInt();
    op = options.option(QString::fromLatin1("x"));
    if (op.isSet())
        x = op.value().toInt();
    op = options.option(QString::fromLatin1("y"));
    if (op.isSet())
        y = op.value().toInt();
    window.resize(w, h);
    window.move(x, y);
    if (options.value(QString::fromLatin1("fullscreen")).toBool())
        window.showFullScreen();

    op = options.option(QString::fromLatin1("ao"));
    if (op.isSet()) {
        QString aos(op.value().toString());
        QStringList ao;
        if (aos.contains(QString::fromLatin1(";")))
            ao = aos.split(QString::fromLatin1(";"), QString::SkipEmptyParts);
        else
            ao = aos.split(QString::fromLatin1(","), QString::SkipEmptyParts);
        window.setAudioBackends(ao);
    }


    op = options.option(QString::fromLatin1("fps"));
    if (op.isSet()) {
        double fps = op.value().toDouble(0);
        if (fps>0.0){
            qDebug()<<"fps for sequence"<<fps;
            window.setFpsSequenceFrame(fps);
        }
    }

    op = options.option(QString::fromLatin1("sf"));
    if (op.isSet()) {
        int sf = op.value().toULongLong(0);
        if (sf){
            qDebug()<<"Start frame for sequence"<<sf;
            window.setStartSequenceFrame(sf);
        }
    }

    op = options.option(QString::fromLatin1("ef"));
    if (op.isSet()) {
        int ef = op.value().toULongLong(0);
        if (ef){
            qDebug()<<"End frame for sequence"<<ef;
            window.setEndSequenceFrame(ef);
        }
    }


    op = options.option(QString::fromLatin1("loop"));
    window.setRepeatLoop(op.isSet());

    op = options.option(QString::fromLatin1("scale"));
    if (op.isSet()) {
        float scale = op.value().toFloat(0);
        if (scale){
            qDebug()<<"Scale for player"<<scale;
            window.setPlayerScale(scale);
        }
    }

    op = options.option(QString::fromLatin1("f"));
    if (op.isSet()) {
        QString fname = op.value().toString();
        if (!fname.isEmpty()){
            qDebug()<<"Name filename"<<fname;
            window.setFileName(fname);
        }
    }

    op = options.option(QString::fromLatin1("ep"));
    if (op.isSet()) {
        QString fname = op.value().toString();
        if (!fname.isEmpty()){
            qDebug()<<"extract task"<<fname;
            window.setExtractTask(fname);
        }
    }

    op = options.option(QString::fromLatin1("vd"));
    if (op.isSet()) {
        QStringList vd = op.value().toString().split(QString::fromLatin1(";"), QString::SkipEmptyParts);
        if (!vd.isEmpty())
            window.setVideoDecoderNames(vd);
    }
    op = options.option(QString::fromLatin1("file"));
    if (op.isSet()) {
        qDebug() << "-f set: " << op.value().toString();
        window.play(op.value().toString());
    } else {
        if (argc > 1 && !a.arguments().last().startsWith(QLatin1Char('-')) && !a.arguments().at(argc-2).startsWith(QLatin1Char('-')))
            window.play(a.arguments().last());
    }
    int ret = a.exec();
    return ret;
}
