#include "savegl.h"

#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QOpenGLFunctions_3_3_Core>
#include <QtCore/QCoreApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QDesktopServices>
#else
#include <QtCore/QStandardPaths>
#endif

SaveGLXuno::SaveGLXuno(QObject *parent) : QObject(parent)
{

}

void SaveGLXuno::saveimg()
{
    defineFileName();
    //    saveFromRender();
    saveFromGLContext();
}

void SaveGLXuno::defineFileName()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    dir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#else
    dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#endif
    if (dir.isEmpty())
        dir = qApp->applicationDirPath() + QStringLiteral("/capture");
    fmt = QStringLiteral("tif");
    QString filemovie="movie";
    qint64 moviepos=0;
    if (m_player){
        QFileInfo fi(m_player->file());
        filemovie=fi.baseName();
        moviepos=m_player->position();
    }
    filemovie.append("-GL");
    lastfilename=QString("%1/%2-%3.%4").arg(dir).arg(filemovie).arg(moviepos).arg(fmt);
}

bool SaveGLXuno::getTextureParameters(const GLuint id, int *width, int *height, int *format)
{
    qDebug()<<"start SaveGLXuno::getTextureParameters"<<*width<<*height;
    QOpenGLFunctions_3_3_Core *f=QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    if(f->glIsTexture(id) == GL_FALSE){
        qWarning()<<"getTextureParameters no texture"<<id;
        return false;
    }
    //int width, height, format;
    f->glBindTexture(GL_TEXTURE_2D, id);
    f->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, width);            // get texture width
    f->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, height);          // get texture height
    f->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, format); // get texture internal format
    f->glBindTexture(GL_TEXTURE_2D, 0);
    qDebug()<<"end SaveGLXuno::getTextureParameters"<<*width<<*height;
    QString formatName = convertInternalFormatToString(*format);
    QString("InternalFormat FBO %1x%2, %3").arg(*width).arg(*height).arg(formatName);

    return (*width>0 && *height>0);
}

void SaveGLXuno::getTextureParametersFBO(const GLint fbo, int *width, int *height, GLint *format){
    qDebug()<<"getTextureParametersFBO"<<*width<<*height;
    QOpenGLFunctions *f=QOpenGLContext::currentContext()->functions();
    int objectType;
    int objectId;
    //int format;

    f->glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    int colorBufferCount=0;
    f->glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER,
                                             GL_COLOR_ATTACHMENT0+colorBufferCount,
                                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                             &objectType);
    if(objectType != GL_NONE)
    {
        f->glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER,
                                                 GL_COLOR_ATTACHMENT0+colorBufferCount,
                                                 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                 &objectId);

        qDebug() << "Color Attachment FBO " << colorBufferCount << ": ";
        if(objectType == GL_TEXTURE)
        {
            getTextureParameters(objectId,width,height,format);
            qDebug() << "GL_TEXTURE FBO, " << *width<<*height;

        }
    }
}

GLuint SaveGLXuno::findLastFBO(){
    GLuint m_fbo=0;
    QOpenGLFunctions *f=QOpenGLContext::currentContext()->functions();
    if(f->glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
        for (int i=0;i<20;i++){
            f->glBindFramebuffer(GL_READ_FRAMEBUFFER, i);
            GLenum error = f->glGetError();
            //qWarning()<<"error Search fbo"<<error;
            if (error==GL_INVALID_OPERATION ){
                m_fbo=i-1;
                qDebug()<<"Found last used fbo"<<i-1;
                break;
            }
        }
    }
    return m_fbo;
}

void SaveGLXuno::saveFromGLContext()
{
    OutputMediaWidth = 0;
    OutputMediaHeight =0;
    GLint textureFormat=0;

    GLint fbo=findLastFBO();

    if (fbo==0) return;

    getTextureParametersFBO(fbo,&OutputMediaWidth,&OutputMediaHeight,&textureFormat);

    if (!OutputMediaWidth || !OutputMediaHeight) return;

    QOpenGLFunctions *f=QOpenGLContext::currentContext()->functions();

    switch (textureFormat) {
    case GL_RGBA16_EXT:
        imageOutputMediaChannels=4;
        imageOutputMediaDepth=16;
        break;
    case GL_RGBA8_EXT:
        imageOutputMediaChannels=4;
        imageOutputMediaDepth=8;
        break;
    default:
        imageOutputMediaChannels=4;
        imageOutputMediaDepth=8;
        break;
    }


    switch (imageOutputMediaChannels) {
    case 4:
        OutputMediaFormat=GL_BGRA;
        break;
    case 3:
        OutputMediaFormat=GL_BGR;
        break;
    default:
        break;
    }


    qDebug()<<QString("SaveGLXuno saveFromGLContext textureFormat 0x%1, formatOutputMedia 0x%2").arg(QString::number(textureFormat,16)).arg(QString::number(OutputMediaFormat,16));
    qDebug()<<"SaveGLXuno imageChannelsOutputMedia"<< imageOutputMediaDepth;

    switch (imageOutputMediaDepth) {
    case 32:
        OutputMediaType=GL_FLOAT;
        break;
    case 16:
        OutputMediaType=GL_UNSIGNED_SHORT;
        break;
    case 8:
        OutputMediaType=GL_UNSIGNED_BYTE;
        break;
    default:
        OutputMediaType=GL_UNSIGNED_BYTE;
        break;
    }

    int bytesPerCahnnel;

    switch (OutputMediaType) {
    case GL_UNSIGNED_SHORT:
        bytesPerCahnnel=sizeof(GLushort);
        qDebug()<<"SaveGLXuno GL_UNSIGNED_SHORT"<< bytesPerCahnnel;
        break;
    case GL_2_BYTES:
        bytesPerCahnnel=2;
        qDebug()<<"SaveGLXuno GL_2_BYTES"<< bytesPerCahnnel;
        break;
    case GL_UNSIGNED_BYTE:
        bytesPerCahnnel=sizeof(GL_UNSIGNED_BYTE);
        qDebug()<<"SaveGLXuno GL_UNSIGNED_SHORT"<< bytesPerCahnnel;
        break;
    default:
        bytesPerCahnnel=1;
        qDebug()<<"SaveGLXuno typeOutputMedia default"<< bytesPerCahnnel;
        break;
    }

    qDebug()<<"SaveGLXuno::saveGL"<<imageOutputMediaChannels<<bytesPerCahnnel;
    qDebug()<<"SaveGLXuno::saveGL oWxH"<<OutputMediaWidth<<OutputMediaHeight;
    qint64 bytesCount=OutputMediaWidth*OutputMediaWidth*imageOutputMediaChannels*bytesPerCahnnel;

    if (!bytesCount) return;

    qDebug()<<"SaveGLXuno::saveGL imageChannelsOutputMedia,bytesPerCahnnel,size,fbo"<<imageOutputMediaChannels<<bytesPerCahnnel<<bytesCount<<fbo;
    releasePixels();
    if (!pixels) pixels=new unsigned char [bytesCount];

    f->glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    while (f->glGetError() != GL_NO_ERROR);
    qDebug()<<"read pixels1 before, size"<<bytesCount<<pixels[0]<<pixels[1]<<pixels[2]<<pixels[3]<<pixels[4];
    f->glReadPixels(0,0,OutputMediaWidth,OutputMediaHeight,OutputMediaFormat,OutputMediaType,pixels);
    GLenum err=f->glGetError();
    if (err!= GL_NO_ERROR) {
        qDebug()<<"glReadPixels error:"<<gl_error(err);
    }
    qDebug()<<"read pixels1 after, size"<<bytesCount<<pixels[0]<<pixels[1]<<pixels[2]<<pixels[3]<<pixels[4]<<"GL Error"<<err;
    if (pixels && OutputMediaWidth>0 && OutputMediaHeight>0){
        //for rotate OpenGL image vertically ised QImage
        QImage qi= QImage(pixels,OutputMediaWidth,OutputMediaHeight,QImage::Format_ARGB32).copy();
        if (!qi.isNull() && !lastfilename.isEmpty()){
            m_player->videoCapture()->captureDir();
            Config::instance().captureFormat();
            //QMatrix matrix = QMatrix().scale(1,-1);
            //qi.transformed(matrix).save(lastfilename);
            qi.save(lastfilename);
        }
    }
}

QString SaveGLXuno::gl_error(GLenum err){
    QString error="";
    if (err!=GL_NO_ERROR) {
        switch(err) {
        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
    }
    return error;
}

///////////////////////////////////////////////////////////////////////////////
// convert OpenGL internal format enum to string
///////////////////////////////////////////////////////////////////////////////
QString SaveGLXuno::convertInternalFormatToString(GLenum format)
{
    QString formatName;

    switch(format)
    {
    case GL_STENCIL_INDEX:      // 0x1901
        formatName = "GL_STENCIL_INDEX";
        break;
    case GL_DEPTH_COMPONENT:    // 0x1902
        formatName = "GL_DEPTH_COMPONENT";
        break;
    case GL_ALPHA:              // 0x1906
        formatName = "GL_ALPHA";
        break;
    case GL_RGB:                // 0x1907
        formatName = "GL_RGB";
        break;
    case GL_RGBA:               // 0x1908
        formatName = "GL_RGBA";
        break;
    case GL_LUMINANCE:          // 0x1909
        formatName = "GL_LUMINANCE";
        break;
    case GL_LUMINANCE_ALPHA:    // 0x190A
        formatName = "GL_LUMINANCE_ALPHA";
        break;
    case GL_R3_G3_B2:           // 0x2A10
        formatName = "GL_R3_G3_B2";
        break;
    case GL_ALPHA4:             // 0x803B
        formatName = "GL_ALPHA4";
        break;
    case GL_ALPHA8:             // 0x803C
        formatName = "GL_ALPHA8";
        break;
    case GL_ALPHA12:            // 0x803D
        formatName = "GL_ALPHA12";
        break;
    case GL_ALPHA16:            // 0x803E
        formatName = "GL_ALPHA16";
        break;
    case GL_LUMINANCE4:         // 0x803F
        formatName = "GL_LUMINANCE4";
        break;
    case GL_LUMINANCE8:         // 0x8040
        formatName = "GL_LUMINANCE8";
        break;
    case GL_LUMINANCE12:        // 0x8041
        formatName = "GL_LUMINANCE12";
        break;
    case GL_LUMINANCE16:        // 0x8042
        formatName = "GL_LUMINANCE16";
        break;
    case GL_LUMINANCE4_ALPHA4:  // 0x8043
        formatName = "GL_LUMINANCE4_ALPHA4";
        break;
    case GL_LUMINANCE6_ALPHA2:  // 0x8044
        formatName = "GL_LUMINANCE6_ALPHA2";
        break;
    case GL_LUMINANCE8_ALPHA8:  // 0x8045
        formatName = "GL_LUMINANCE8_ALPHA8";
        break;
    case GL_LUMINANCE12_ALPHA4: // 0x8046
        formatName = "GL_LUMINANCE12_ALPHA4";
        break;
    case GL_LUMINANCE12_ALPHA12:// 0x8047
        formatName = "GL_LUMINANCE12_ALPHA12";
        break;
    case GL_LUMINANCE16_ALPHA16:// 0x8048
        formatName = "GL_LUMINANCE16_ALPHA16";
        break;
    case GL_INTENSITY:          // 0x8049
        formatName = "GL_INTENSITY";
        break;
    case GL_INTENSITY4:         // 0x804A
        formatName = "GL_INTENSITY4";
        break;
    case GL_INTENSITY8:         // 0x804B
        formatName = "GL_INTENSITY8";
        break;
    case GL_INTENSITY12:        // 0x804C
        formatName = "GL_INTENSITY12";
        break;
    case GL_INTENSITY16:        // 0x804D
        formatName = "GL_INTENSITY16";
        break;
    case GL_RGB4:               // 0x804F
        formatName = "GL_RGB4";
        break;
    case GL_RGB5:               // 0x8050
        formatName = "GL_RGB5";
        break;
    case GL_RGB8:               // 0x8051
        formatName = "GL_RGB8";
        break;
    case GL_RGB10:              // 0x8052
        formatName = "GL_RGB10";
        break;
    case GL_RGB12:              // 0x8053
        formatName = "GL_RGB12";
        break;
    case GL_RGB16:              // 0x8054
        formatName = "GL_RGB16";
        break;
    case GL_RGBA2:              // 0x8055
        formatName = "GL_RGBA2";
        break;
    case GL_RGBA4:              // 0x8056
        formatName = "GL_RGBA4";
        break;
    case GL_RGB5_A1:            // 0x8057
        formatName = "GL_RGB5_A1";
        break;
    case GL_RGBA8:              // 0x8058
        formatName = "GL_RGBA8";
        break;
    case GL_RGB10_A2:           // 0x8059
        formatName = "GL_RGB10_A2";
        break;
    case GL_RGBA12:             // 0x805A
        formatName = "GL_RGBA12";
        break;
    case GL_RGBA16:             // 0x805B
        formatName = "GL_RGBA16";
        break;
    case GL_DEPTH_COMPONENT16:  // 0x81A5
        formatName = "GL_DEPTH_COMPONENT16";
        break;
    case GL_DEPTH_COMPONENT24:  // 0x81A6
        formatName = "GL_DEPTH_COMPONENT24";
        break;
    case GL_DEPTH_COMPONENT32:  // 0x81A7
        formatName = "GL_DEPTH_COMPONENT32";
        break;
    case GL_DEPTH_STENCIL:      // 0x84F9
        formatName = "GL_DEPTH_STENCIL";
        break;
    case GL_RGBA32F:            // 0x8814
        formatName = "GL_RGBA32F";
        break;
    case GL_RGB32F:             // 0x8815
        formatName = "GL_RGB32F";
        break;
    case GL_RGBA16F:            // 0x881A
        formatName = "GL_RGBA16F";
        break;
    case GL_RGB16F:             // 0x881B
        formatName = "GL_RGB16F";
        break;
    case GL_DEPTH24_STENCIL8:   // 0x88F0
        formatName = "GL_DEPTH24_STENCIL8";
        break;
    default:
        formatName = QString ("Unknown Format(0x%1)").arg(format);
    }

    return formatName;
}

void SaveGLXuno::saveFromRender()
{
    if (m_player){
        pixels=Q_NULLPTR;
        int w=0;
        int h=0;
        int bpp=4;
        bool readypixels=m_player->renderer()->getPixels(pixels, w, h, bpp);
        if (readypixels && pixels && w>0 && h>0){
            if (1){
                qint64 byteCount=w*h*bpp;
                //for rotate OpenGL image vertically ised QImage
                QImage qi= QImage(pixels,w,h,QImage::Format_ARGB32).copy();
                if (!qi.isNull() && !lastfilename.isEmpty()){
                    m_player->videoCapture()->captureDir();
                    Config::instance().captureFormat();
                    QMatrix matrix = QMatrix().scale(1,-1);
                    qi.transformed(matrix).save(lastfilename);
                    qDebug()<<"Saved saveFromRender"<<lastfilename<<w<<"x"<<h;
                    //                    QMatrix matrix = QMatrix().scale(1,-1);
                    //                    buffer=QByteArray((const char*) qi.transformed(matrix).bits(),byteCount);
                    //                    emit onSentFrame();
                }
            }
        }
    }
}

bool SaveGLXuno::releasePixels()
{
    if (pixels) {
        delete []pixels;
        pixels=Q_NULLPTR;
        return true;
    }
    return false;
}

void SaveGLXuno::setPlayer(QtAV::AVPlayer *player)
{
    m_player=player;
}

