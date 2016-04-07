
SOURCES += main.cpp \
    MainWindow.cpp \
    ClickableMenu.cpp \
    StatisticsView.cpp \
    Slider.cpp \
    TVView.cpp \
    EventFilter.cpp \
    config/ConfigDialog.cpp \
    config/ConfigPageBase.cpp \
    config/CaptureConfigPage.cpp \
    config/VideoEQConfigPage.cpp \
    config/DecoderConfigPage.cpp \
    config/MiscPage.cpp \
    filters/OSD.cpp \
    filters/OSDFilter.cpp \
    playlist/PlayListModel.cpp \
    playlist/PlayListItem.cpp \
    playlist/PlayListDelegate.cpp \
    playlist/PlayList.cpp \
    config/PropertyEditor.cpp \
    config/AVFormatConfigPage.cpp \
    config/AVFilterConfigPage.cpp \
    filters/AVFilterSubtitle.cpp \
    $$PWD/filters/netstreamserver.cpp \
    $$PWD/filters/netstreamthread.cpp \
    $$PWD/filters/runmpvpipe.cpp \
    $$PWD/filters/advancedfilter.cpp \
    $$PWD/filters/ShaderFilterXuno.cpp \
    $$PWD/filters/savegl.cpp

HEADERS += \
    MainWindow.h \
    ClickableMenu.h \
    StatisticsView.h \
    Slider.h \
    TVView.h \
    EventFilter.h \
    config/ConfigDialog.h \
    config/ConfigPageBase.h \
    config/CaptureConfigPage.h \
    config/VideoEQConfigPage.h \
    config/DecoderConfigPage.h \
    config/MiscPage.h \
    filters/OSD.h \
    filters/OSDFilter.h \
    playlist/PlayListModel.h \
    playlist/PlayListItem.h \
    playlist/PlayListDelegate.h \
    playlist/PlayList.h \
    config/PropertyEditor.h \
    config/AVFormatConfigPage.h \
    config/AVFilterConfigPage.h \
    filters/AVFilterSubtitle.h \
    $$PWD/version.h \
    $$PWD/filters/netstreamserver.h \
    $$PWD/filters/netstreamthread.h \
    $$PWD/filters/runmpvpipe.h \
    $$PWD/filters/advancedfilter.h \
    $$PWD/filters/ShaderFilterXuno.h \
    $$PWD/filters/savegl.h

SOURCES +=  \
    XunoBrowser.cpp \
    config/ImageSequenceConfigPage.cpp \
    config/configwebmemu.cpp \
    config/WebConfigPage.cpp \
    config/ImageSequenceExtractControl.cpp

HEADERS += \
    XunoBrowser.h \
    config/ImageSequenceConfigPage.h \
    config/configwebmemu.h \
    config/WebConfigPage.h \
    config/ImageSequenceExtractControl.h
