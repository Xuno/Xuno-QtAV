#ifndef XUNO_VERSION_H
#define XUNO_VERSION_H

#define XUNO_QTAV_MAJOR VER_MAJ_STRING    //((XUNO_QTAV_VERSION&0xff0000)>>16)
#define XUNO_QTAV_MINOR VER_MIN_STRING    //((XUNO_QTAV_VERSION&0xff00)>>8)
#define XUNO_QTAV_PATCH VER_PAT_STRING    //(XUNO_QTAV_VERSION&0xff)

#ifdef WIN64
#define QTAV_PLATFORM x64
#else

#ifdef ARCH
#define QTAV_PLATFORM ARCH
#endif

#endif


#ifdef QTAV_PLATFORM
#define XUNO_QTAV_VERSION_STR       "v" TOSTR(XUNO_QTAV_MAJOR) "." TOSTR(XUNO_QTAV_MINOR) "." TOSTR(XUNO_QTAV_PATCH) " (" TOSTR(QTAV_PLATFORM) ")"
#else
#define XUNO_QTAV_VERSION_STR       "v." TOSTR(XUNO_QTAV_MAJOR) "." TOSTR(XUNO_QTAV_MINOR) "." TOSTR(XUNO_QTAV_PATCH)
#endif

#define XUNO_QTAV_VERSION_STR_LONG   XUNO_QTAV_VERSION_STR " (" __DATE__ ", " __TIME__ ")"

#endif // XUNO_VERSION_H

