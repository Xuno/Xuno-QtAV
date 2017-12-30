# [QtAV](http://www.qtav.org)  [![Build Status](https://travis-ci.org/wang-bin/QtAV.svg)](https://travis-ci.org/wang-bin/QtAV) [![Appveyor](https://ci.appveyor.com/api/projects/status/github/wang-bin/qtav?svg=true&passingText=windows%20-%20OK)](https://ci.appveyor.com/project/wang-bin/qtav)

XunoPlayer-QtAV is fork of QtAV a multimedia playback library based on Qt and FFmpeg with enhanced user functionality and optimization  for High quality Quality 10-bit YUV444 HEVC video.

XunoPlayer-QtAV has been added to FFmpeg projects page [http://ffmpeg.org/projects.html](http://ffmpeg.org/projects.html)

**QtAV is free software licensed under the term of LGPL v2.1. The player example is licensed under GPL v3.  If you use QtAV or its constituent libraries,
you must adhere to the terms of the license in question.**

# XunoPlayer-QtAV based on QtAV

![Alt text](http://www.xuno.com/images/XunoPlayer_sc.png "XunoPlayer-QtAV")

![Alt text](http://www.xuno.com/images/XunoPlayer_sc2.png "XunoPlayer-QtAV")


### Features

QtAV can meet your most demands

- Hardware decoding suppprt: DXVA2, VAAPI, VDA/VideoToolbox, CedarX, CUDA(the 1st player support CUDA on linux?)
- OpenGL and ES2 support for almost all formats including Hi10P videos (The 1st player/library support 10bit in ES2? VLC, XBMC, mplayer does not support now)
- Real time preview
- Video capture in rgb and yuv format
- OSD and custom filters
- Filters in libavfilter, for example stero3d, blur
- Subtitle track select. Dynamic change FFmpeg and libass engine
- Play frame by frame
- Playback speed control
- Variant streams: locale file, http, rtsp etc. and your custom streams
- Audio channel, tracks and external audio tracks
- Dynamically change render engine when playing.
- Dynamically change video decoder
- Multiple video outputs for 1 player
- Video eq(software and OpenGL): brightness, contrast, saturation, hue
- QML support. Most playback APIs are compatible with QtMultimedia module
- Compatibility: QtAV can be built with both Qt4 and Qt5, FFmpeg(>=1.0) and [Libav](http://libav.org) (>=9.0). Latest FFmpeg release is recommended.


