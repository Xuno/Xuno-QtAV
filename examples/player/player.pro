TARGET = player
TEMPLATE = app
contains(QT_CONFIG, opengl): QT += opengl
QT += sql svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TRANSLATIONS = res/player_zh_CN.ts res/player.ts
!static:VERSION = $$QTAV_VERSION # vc: will create exp and lib, result in static build error

PROJECTROOT = $$PWD/../..
include($$PROJECTROOT/src/libQtAV.pri)
include($$PROJECTROOT/widgets/libQtAVWidgets.pri)
STATICLINK=1
include($$PWD/../common/libcommon.pri)
preparePaths($$OUT_PWD/../../out)
INCLUDEPATH += $$PWD
mac: RC_FILE = $$PROJECTROOT/src/QtAV.icns
genRC($$TARGET)
include(src.pri)

unix:!android:!mac {
#debian
player_bins = Player QMLPlayer
DEB_INSTALL_LIST = $$join(player_bins, \\n.$$[QT_INSTALL_BINS]/, .$$[QT_INSTALL_BINS]/)
DEB_INSTALL_LIST *= \
            usr/share/applications/Player.desktop \
            usr/share/applications/QMLPlayer.desktop \
            usr/share/icons/hicolor/scalable/apps/QtAV.svg
deb_install_list.target = qtav-players.install
deb_install_list.commands = echo \"$$join(DEB_INSTALL_LIST, \\n)\" >$$PROJECTROOT/debian/$${deb_install_list.target}
QMAKE_EXTRA_TARGETS += deb_install_list
target.depends *= $${deb_install_list.target}

qtav_players_links.target = qtav-players.links
qtav_players_links.commands = echo \"$$[QT_INSTALL_BINS]/Player /usr/bin/Player\n$$[QT_INSTALL_BINS]/QMLPlayer /usr/bin/QMLPlayer\" >$$PROJECTROOT/debian/$${qtav_players_links.target}
QMAKE_EXTRA_TARGETS *= qtav_players_links
target.depends *= $${qtav_players_links.target}
}

tv.files = res/tv.ini
#BIN_INSTALLS += tv
target.path = $$[QT_INSTALL_BINS]
include($$PROJECTROOT/deploy.pri)

RESOURCES += res/player.qrc \
    darkstyle.qrc

contains(QMAKE_HOST.arch, x86_64):{
    message("Host is 64bit")
}else{
    message("Host not 64bit")
}
DEFINES+= ARCH=\"$$QMAKE_HOST.os\"_\"$$QMAKE_HOST.arch\"
message("DEFINES: "$$DEFINES)

DISTFILES += \
    res/shaders/simple_vec.glsl \
    res/shaders/simple_ver.glsl \
    res/shaders/simple_frag.glsl \
    res/shaders/superxbr-naitive-vertex.glsl \
    res/shaders/adaptive-sahrpen-pass0-0.glsl \
    res/shaders/adaptive-sahrpen-pass0-1.glsl \
    res/shaders/adaptive-sahrpen-pass1-0.glsl \
    res/shaders/adaptive-sahrpen-pass1-1.glsl \
    res/shaders/adaptive-sahrpen-pass1-2.glsl \
    res/shaders/superxbr-native-pass0.glsl  \
    res/shaders/superxbr-native-pass1.glsl  \
    res/shaders/superxbr-native-pass2.glsl \
    darkstyle/icon_branch_closed.png \
    darkstyle/icon_branch_end.png \
    darkstyle/icon_branch_more.png \
    darkstyle/icon_branch_open.png \
    darkstyle/icon_checkbox_checked.png \
    darkstyle/icon_checkbox_checked_disabled.png \
    darkstyle/icon_checkbox_checked_pressed.png \
    darkstyle/icon_checkbox_indeterminate.png \
    darkstyle/icon_checkbox_indeterminate_disabled.png \
    darkstyle/icon_checkbox_indeterminate_pressed.png \
    darkstyle/icon_checkbox_unchecked.png \
    darkstyle/icon_checkbox_unchecked_disabled.png \
    darkstyle/icon_checkbox_unchecked_pressed.png \
    darkstyle/icon_close.png \
    darkstyle/icon_radiobutton_checked.png \
    darkstyle/icon_radiobutton_checked_disabled.png \
    darkstyle/icon_radiobutton_checked_pressed.png \
    darkstyle/icon_radiobutton_unchecked.png \
    darkstyle/icon_radiobutton_unchecked_disabled.png \
    darkstyle/icon_radiobutton_unchecked_pressed.png \
    darkstyle/icon_undock.png \
    darkstyle/icon_vline.png \
    darkstyle/darkstyle.qss

//Xuno
QT += webenginewidgets






