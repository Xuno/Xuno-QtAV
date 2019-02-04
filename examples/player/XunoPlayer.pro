PROJECTROOT = $$PWD
include ($$PROJECTROOT/player.pro)

TARGET = XunoPlayer-QtAV
VER_MAJ = 0
VER_MIN = 3
VER_PAT = 2
VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}

DEFINES += VER_MAJ_STRING=$${VER_MAJ}
DEFINES += VER_MIN_STRING=$${VER_MIN}
DEFINES += VER_PAT_STRING=$${VER_PAT}
DEFINES += VERSION_STRING=\\\"$${VER_MAJ}.$${VER_MIN}.$${VER_PAT}\\\"

RC_ICONS = $${PROJECTROOT}/examples/player/ico/XunoPlayer-QtAV.ico
QMAKE_TARGET_COMPANY = "Aaex Corp. http://www.xuno.com. https://github.com/Xuno/Xuno-QtAV"
QMAKE_TARGET_DESCRIPTION = "XunoPlayer. Aaex Corp. http://www.xuno.com. QtAV Multimedia framework."
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2012-2017 WangBin. Aaex Corp."
QMAKE_TARGET_PRODUCT = "XunoPlayer-QtAV $$1"
export(RC_ICONS)
export(QMAKE_TARGET_COMPANY)
export(QMAKE_TARGET_DESCRIPTION)
export(QMAKE_TARGET_COPYRIGHT)
export(QMAKE_TARGET_PRODUCT)

message("XunoPlayer-QtAV DEFINES: "$$DEFINES)

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
    res/shaders/superxbr-native-pass2.glsl

//Xuno
QT += webenginewidgets
