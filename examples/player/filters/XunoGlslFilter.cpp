#include "XunoGlslFilter.h"
#include <QDebug>
#include <QImage>
#include <QOpenGLPaintDevice>

XunoGLSLFilter::XunoGLSLFilter(QObject *parent):
    QtAV::GLSLFilter(parent)
{
    qDebug()<<"constructor XunoGLSLFilter::XunoGLSLFilter";
    if (opengl()){
        qDebug()<<"connect XunoGLSLFilter::XunoGLSLFilter";
        connect(opengl(), &QtAV::OpenGLVideo::beforeRendering, this, &XunoGLSLFilter::beforeRendering, Qt::DirectConnection);
        connect(opengl(), &QtAV::OpenGLVideo::afterRendering, this, &XunoGLSLFilter::afterRendering, Qt::DirectConnection);
    }

    shader_files_prefix=":/shaders/";
    //shader_files_prefix="/home/lex/project-C/github/SuperXbr-GL/depend/common-shaders/xbr/";

    //    shader_files_include="shaders/super-xbr/super-xbr-params.inc";

    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass0.glsl";
    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass1.glsl";
    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass2.glsl";
    //    shader_files<<"shaders/super-xbr/custom-jinc2-sharper.glsl";

    //shader_files_include="super-xbr-params.inc";

    shader_files_include="";
    if (shader_vertex_files.isEmpty()){
        shader_vertex_files<<"superxbr-naitive-vertex.glsl";
    }

    //    shader_files<<"super-xbr-pass0.glsl";
    //    shader_files<<"super-xbr-pass1.glsl";
    //    shader_files<<"super-xbr-pass2.glsl";
    //    shader_files<<"custom-jinc2-sharper.glsl";

    if (shader_files.isEmpty()){
        shader_files<<"superxbr-native-pass0.glsl";
        shader_files<<"superxbr-native-pass1.glsl";
        //shader_files<<"superxbr-native-pass2.glsl";
    }

    maxPass=shader_files.size()-1;

    if (scales.isEmpty()){
        scales<<2<<1<<1<<1<<1;
    }

}

void XunoGLSLFilter::setShader(QtAV::VideoShader *ush)
{
    user_shader=ush;
    //opengl()->setUserShader(user_shader);
}

void XunoGLSLFilter::beforeRendering()
{
    //qDebug()<<"XunoGLSLFilter::beforeRendering"<<fbo()->size();
    colorTransform();
//    return;
    if (fbo() && fbo()->isValid()){

        QOpenGLFunctions *f=opengl()->openGLContext()->functions();
        if (f && fbo()->textures().size()){
            GLenum target=GL_TEXTURE_2D;
            f->glBindTexture(target,fbo()->texture());
            //f->glGenerateMipmap(target);
            f->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            f->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //f->glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //f->glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glBindTexture(target,0);
        }
    }
}


void XunoGLSLFilter::afterRendering()
{
    //qDebug()<<"XunoGLSLFilter::afterRendering()";
    //return;
    lastSuperscaleTexureId=0;
    pass=0;
    if (fbo() && fbo()->isValid() ) {
        //need save image after rendering
        if (needSave){
            QString name=defineFileName();
            if (name.isEmpty()) name=savePath.append(QString("SaveFBOafterRendering-%1.tif").arg(QDateTime().currentMSecsSinceEpoch()));
            qDebug()<<"XunoGLSLFilter::afterRendering() fbo size() saving"<<fbo()->size()<<name;
            //SAVE FBO for text result;
            QImage s=fbo()->toImage(false);
            //QMatrix matrix = QMatrix().scale(1,-1); //OpenGL rotate;
            //s.transformed(matrix).save(name);
            if (m_player->videoCapture()->quality()>0){
                s.save(name,Q_NULLPTR,m_player->videoCapture()->quality());
            }else{
                s.save(name);
            }
            needSave=false;
        }

        //clear custom programs and fbos
        while (m_fbo.size()) {
            delete m_fbo.takeLast();
        }

        while (programs.size()) {
            delete programs.takeLast();
        }

        //need superscale image after rendering and replace fbo for show by opengl
        if (needSuperScale){
            superscale(2.0,1.0);
        }

        GLuint sfbotextid=sharpShader(frameTexture());
        if (sfbotextid) lastSuperscaleTexureId=sfbotextid;

        //need last linear filtering image
        if (needSuperScaleLastLinearFiltering){
            QOpenGLFunctions *f=opengl()->openGLContext()->functions();
            if (f && fbo()->textures().size()){
                GLenum target=GL_TEXTURE_2D;
                f->glBindTexture(target,frameTexture());
                //f->glGenerateMipmap(target);
                f->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                f->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                f->glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                f->glBindTexture(target,0);
            }
        }
    }
}

bool XunoGLSLFilter::getNeedSuperScaleLastLinearFiltering() const
{
    return needSuperScaleLastLinearFiltering;
}

void XunoGLSLFilter::setNeedSuperScaleLastLinearFiltering(bool value)
{
    needSuperScaleLastLinearFiltering = value;
}

bool XunoGLSLFilter::getNeedSuperScale() const
{
    return needSuperScale;
}

GLuint XunoGLSLFilter::frameTexture() const
{
    //qDebug()<<"XunoGLSLFilter::frameTexture()"<<lastSuperscaleTexureId;
    return (lastSuperscaleTexureId>0) ? lastSuperscaleTexureId : fbo()->texture();
}

void XunoGLSLFilter::colorTransform(bool runOnce)
{
    if (colorTransformChanged){
        opengl()->setBrightness(brightness);
        opengl()->setContrast(contrast);
        opengl()->setHue(hue);
        opengl()->setSaturation(saturation);
        colorTransformChanged=!runOnce;
    }
}

void XunoGLSLFilter::setHue(const qreal &value)
{
    hue = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setContrast(const qreal &value)
{
    contrast = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setSaturation(const qreal &value)
{
    saturation = value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setPlayer(QtAV::AVPlayer *player)
{
    m_player = player;
}

void XunoGLSLFilter::setBrightness(const qreal &value)
{
    brightness=value;
    colorTransformChanged=true;
}

void XunoGLSLFilter::setSavePath(const QString &value)
{
    savePath = value;
}

void XunoGLSLFilter::setNeedSave(bool value)
{
    needSave = value;
}

void XunoGLSLFilter::setNeedSuperScale(bool value)
{
    needSuperScale = value;
}

QString XunoGLSLFilter::defineFileName()
{
    QString dir;

    if (m_player){
        dir=m_player->videoCapture()->captureDir();
    }

    QDir mdir=QDir(dir);
    if (!mdir.exists()) mdir.mkpath(dir);

    if (!mdir.exists()) return "";

    QString fmt=m_player->videoCapture()->saveFormat();
    if (fmt.isEmpty()) fmt = QStringLiteral("tif");

    QString filemovie="movie";
    qint64 moviepos=0;
    if (m_player){
        QFileInfo fi(m_player->file());
        filemovie=fi.baseName();
        moviepos=m_player->position();
    }
    filemovie.append("-GL");
    QString lastfilename=QString("%1/%2-%3.%4").arg(dir).arg(filemovie).arg(moviepos).arg(fmt);
    return lastfilename;
}

//------------superscale----------------

//opt_sharpness [0...2] default 1.0f
//opt_edge_strength [0...1] default 0.6f
void XunoGLSLFilter::superscale(GLfloat opt_sharpness, GLfloat opt_edge_strength)
{

    if ( geometries==Q_NULLPTR) geometries = new GeometryEngine;

    QOpenGLFunctions *f=opengl()->openGLContext()->functions();

    if (!f) return;

    //if (initSize.isEmpty()){
    initSize=outputSize();
    //}

    //qDebug()<<"superscale" << initSize;
    //setOutputSize(initSize*2);
    //qDebug()<<"superscale x2" << outputSize();


    //initTextures();

    frame++;

    // Enable depth buffer
    f->glEnable(GL_DEPTH_TEST);

    // Enable back face culling // QtAV move freeze with it.
    //f->glEnable(GL_CULL_FACE);


    //---------------------------------------------
    //GLuint fbotextid=texture->textureId();
    GLuint fbotextid=fbo()->texture(); //used last fbo of player as source frame

    //qDebug()<<"Texture id start"<<fbotextid<<"texure size:"<<texture->width()<<"x"<<texture->height();

    for (pass=0;pass<=maxPass;pass++){

        bool rotate=false;//(pass>0);
        //qDebug()<<"Programs:"<<
        addProgram();
        if (initShaders_xbr(pass) && !scales.isEmpty()){
            int fboID=addFBO(scales.at(pass),/*rotate*/0);

            QOpenGLShaderProgram *program=Q_NULLPTR;
            if (pass<=programs.size()){
                program=programs.at(pass);
            }

            m_fbo[fboID]->bind();

            f->glViewport(0,0,m_fbo[fboID]->width(),m_fbo[fboID]->height());
            // Use texture unit 0 which contains cube.png

            program->bind();

            //qDebug()<<"texture0 is";
            program->setUniformValue("texture0",  0);


            QVector2D textureSize=QVector2D (float(m_fbo[fboID]->width()),float(m_fbo[fboID]->height()));

            //qDebug()<<"texture_size0 is";
            program->setUniformValue("texture_size0", textureSize);


            //qDebug()<<"pixel_size0 is";
            program->setUniformValue("pixel_size0", QVector2D(1.0f,1.0f)/textureSize);


            //qDebug()<<"texture_rot0 is";
            program->setUniformValue("texture_rot0", QVector2D(0.0f,0.0f));

            //options
            program->setUniformValue("sharpness", opt_sharpness);     // [0...2] default 1.0f
            program->setUniformValue("edge_strength",opt_edge_strength); // [0...1] default 0.6f

            QMatrix4x4 matrix;
            matrix.setToIdentity();

            if (rotate) {
                int sign=(pass==2)?-1:1;
                matrix.rotate(180*sign,0.,0.,1.);
            }

            program->setUniformValue("MVPMatrix", matrix);

            //if (1) {
                //if (0){  //pass==0
                //    f->glActiveTexture(GL_TEXTURE0);
                //    texture->bind();
                //}else{
                f->glActiveTexture(GL_TEXTURE0);
                f->glBindTexture(GL_TEXTURE_2D, fbotextid);
                f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST GL_LINEAR
                f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                //f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//GL_CLAMP_TO_EDGE
                //f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                //}

                f->glClearColor(1.0,0.0,0.0,1.0);//RED
                f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Draw cube geometry
                geometries->drawCubeGeometry(program);

//                if (0){//pass==0
//                    texture->release();
//                }else{

                f->glBindTexture(GL_TEXTURE_2D, 0);

//                }
//            }
            program->release();
            m_fbo[fboID]->release();
#if (unix)
//            QString filename=QString("/home/lex/temp/savefbo_pass_%1_%2x%3-%4.bmp").arg(pass).arg(m_fbo[fboID]->width()).arg(m_fbo[fboID]->height()).arg(frame);
#else
//            QString filename=QString("e:/temp/shader/savefbo_pass_%1_%2x%3-%4.bmp").arg(pass).arg(m_fbo[fboID]->width()).arg(m_fbo[fboID]->height()).arg(frame);
#endif
            // qDebug()<<"Saving:"<<filename;
            // m_fbo[fboID]->toImage(false).save(filename);

            fbotextid=m_fbo[fboID]->texture();
            //qDebug()<<"Texture id"<<fbotextid<<"texure size:"<<m_fbo[fboID]->width()<<"x"<<m_fbo[fboID]->height();
        }else{
            qDebug()<<"initShaders error (pass)"<<pass;
        }
    }

    if (fbotextid) lastSuperscaleTexureId=fbotextid;
}

GLuint XunoGLSLFilter::sharpShader(GLuint pfbotextid)
{
    //qDebug()<<"XunoGLSLFilter::sharpShader";
    ShaderFilterXuno *m_sharpShader = static_cast <ShaderFilterXuno*> (user_shader);
    if (!m_sharpShader)  {
        //qDebug()<<"m_sharpShader skipped 0";
        return 0;
    }
    if (!m_sharpShader->needToRun()) {
        //qDebug()<<"m_sharpShader skipped 1";
        return 0;
    }

    int prid=-1;
    prid=addProgram();
    QOpenGLShaderProgram *program=Q_NULLPTR;
    if (prid!=1){
        program=programs.at(prid);
    }else{
        qDebug()<<"programs not is"<<prid;
        return 0;
    }
    QOpenGLFunctions *f=opengl()->openGLContext()->functions();
    if (!f) {
        qDebug()<<"m_sharpShader skipped 2";
        return 0;
    }
    if (program==Q_NULLPTR) {
        qDebug()<<"m_sharpShader skipped 3, prid"<<prid;
        return 0;
    }


    int fboID=addFBO(1,false);

    program->removeAllShaders();

    if (shader_files.size()){
        QString filename;
        filename=QString(shader_files_prefix).append(shader_vertex_files.at(0));
        //qDebug()<<Shader;
        // Compile vertex shader
        if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, filename)){
            return 0;
        }
    }

    //qDebug()<<"XunoGLSLFilter::sharpShader.userShaderHeader: ";

    m_sharpShader->setCustomProgram(program);
    m_sharpShader->compile();
    //qDebug()<<"getFilterSharp:"<<m_sharpShader->getFilterSharp();

    if (!program->isLinked()) return 0;

    m_fbo[fboID]->bind();
    f->glViewport(0,0,m_fbo[fboID]->width(),m_fbo[fboID]->height());

    program->bind();

    m_sharpShader->setUserUniformValues();

//    qDebug()<<"ShaderFilterXuno::compile() Sharer vertex:";
//    qDebug()<<program->shaders().at(0)->sourceCode();
//    qDebug()<<"ShaderFilterXuno::compile() Sharer fragment:";
//    qDebug()<<program->shaders().at(1)->sourceCode();


    //qDebug()<<"texture0 is";
    program->setUniformValue("texture0",  0);

    QVector2D textureSize=QVector2D (float(m_fbo[fboID]->width()),float(m_fbo[fboID]->height()));

    //qDebug()<<"texture_size0 is";
    program->setUniformValue("u_textureSize", textureSize);

    //qDebug()<<"pixel_size0 is";
    program->setUniformValue("u_texelSize", QVector2D(1.0f,1.0f)/textureSize);

    //qDebug()<<"texture_rot0 is";
    program->setUniformValue("texture_rot0", QVector2D(0.0f,0.0f));

    QMatrix4x4 matrix;
    matrix.setToIdentity();

    program->setUniformValue("MVPMatrix", matrix);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, pfbotextid);
    f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);//GL_NEAREST GL_LINEAR
    f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    f->glClearColor(0.0,1.0,0.0,1.0);//GREEN
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ( geometries==Q_NULLPTR) geometries = new GeometryEngine;

    if ( geometries!=Q_NULLPTR) geometries->drawCubeGeometry(program);

    f->glBindTexture(GL_TEXTURE_2D, 0);

    program->release();
    m_fbo[fboID]->release();

    //delete m_sharpShader;

    return  m_fbo[fboID]->texture();
}


bool XunoGLSLFilter::initShaders(int pass)
{
    QOpenGLShaderProgram *program=Q_NULLPTR;
    if (pass<=programs.size()){
        program=programs[pass];
    }
    if (program==Q_NULLPTR) {
        qDebug()<<"program not present";
        return false;
    }


    //qDebug()<<"initShaders pass"<<pass;

    QByteArray sourceShader,sourceShaderInclude;


    //read include part for shaders
    QString filename=QString(shader_files_prefix).append(shader_files_include);
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)){
        sourceShaderInclude=file.readAll();
        sourceShaderInclude.replace(QByteArray("static "),QByteArray(""));
        file.close();
    }else{
        qWarning()<<"Can't open file:"<<filename;
    }


    //read shaders
    filename=QString(shader_files_prefix).append(shader_files.at(pass));
    file.setFileName(filename);

    if (file.open(QIODevice::ReadOnly)){
        sourceShader=file.readAll();
        sourceShader.prepend(sourceShaderInclude);
        file.close();
    }else{
        qWarning()<<"Can't open file:"<<filename;
    }

    if (!sourceShader.isEmpty()){

        //program->removeAllShaders();

        QByteArray ShaderType="#version 130\n#define VERTEX\n";
        QByteArray Shader=QByteArray(sourceShader).prepend(ShaderType);
        //qDebug()<<Shader;
        // Compile vertex shader
        if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, Shader)){
            return false;
        }
        ShaderType="#version 130\n#define FRAGMENT\n";
        Shader=QByteArray(sourceShader).prepend(ShaderType);
        //qDebug()<<Shader;
        // Compile vertex shader
        if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, Shader)){
            return false;
        }

        if (!program->link()) {
            qDebug()<<program->log();
            qDebug()<<program->shaders().at(0)->sourceCode();
            return false;
        }

        return true;
    }

    return false;
}

bool XunoGLSLFilter::initShaders_simple(int pass)
{
    QOpenGLShaderProgram *program=Q_NULLPTR;
    if (pass<=programs.size()){
        program=programs[pass];
    }
    if (program==Q_NULLPTR) {
        qDebug()<<"program not present";
        return false;
    }

    qDebug()<<"initShaders_simple pass"<<pass;

    program->removeAllShaders();

    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple_ver.glsl")){
        return false;
    }
    // Compile vertex shader
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple_frag.glsl")){
        return false;
    }

    if (!program->link()) {
        qDebug()<<program->log();
        qDebug()<<program->shaders().at(0)->sourceCode();
        return false;
    }


    return true;

}


bool XunoGLSLFilter::initShaders_xbr(int pass)
{
    QOpenGLShaderProgram *program=Q_NULLPTR;
    if (pass<=programs.size()){
        program=programs[pass];
    }
    if (program==Q_NULLPTR) {
        qDebug()<<"program not present";
        return false;
    }

    //qDebug()<<"initShaders pass"<<pass;

    program->removeAllShaders();

    if (shader_files.size()){
        QString filename;
        filename=QString(shader_files_prefix).append(shader_vertex_files.at(0));
        //qDebug()<<Shader;
        // Compile vertex shader
        if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, filename)){
            return false;
        }
        filename=QString(shader_files_prefix).append(shader_files.at(pass));
        //qDebug()<<Shader;
        // Compile vertex shader
        if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, filename)){
            return false;
        }

        if (!program->link()) {
            qDebug()<<program->log();
            qDebug()<<program->shaders().at(0)->sourceCode();
            return false;
        }

        return true;
    }

    return false;
}


int XunoGLSLFilter::addProgram()
{
    QOpenGLShaderProgram *pr=new QOpenGLShaderProgram(this);
    programs.append(pr);
    return programs.size()-1;
}

int XunoGLSLFilter::addFBO(int scale, bool rotate)
{
    int texture_width=0, texture_height=0;
    int prev_id=m_fbo.size()-1;
    if (prev_id<0){
        texture_width=fbo()->size().width();//texture->width();
        texture_height=fbo()->size().height();//texture->height();
        //qDebug()<<"addFBO first"<<texture_width<<texture_height;
    }else{
        texture_width=m_fbo[prev_id]->width();
        texture_height=m_fbo[prev_id]->height();
        //qDebug()<<"addFBO next"<<prev_id<<texture_width<<texture_height;
    }
    texture_width*=scale;
    texture_height*=scale;

    if (rotate){
        int t=texture_width;
        texture_width=texture_height;
        texture_height=t;
    }

    QOpenGLFramebufferObject* cfbo = new QOpenGLFramebufferObject(texture_width,texture_height,GL_TEXTURE_2D);
    m_fbo.append(cfbo);
    return m_fbo.size()-1;

}

void XunoGLSLFilter::initTextures()
{

    //qDebug()<<"hasOpenGLFramebufferBlit()"<<QOpenGLFramebufferObject::hasOpenGLFramebufferBlit();

    // Load cube.png image
    //QImage image=QImage(":/savefbo_pass_0_1920x1080-empty-shader.bmp");//RAW_Yamaha_960x540.bmp
    //QImage image=QImage("/home/lex/temp/RAW_Yamaha_960x540.bmp");

    QImage image=fbo()->toImage(false);
    //this->resize(image.size());

    //qDebug()<<"After Resize";

    if (texture!=Q_NULLPTR){
        texture->release(texture->textureId(),QOpenGLTexture::ResetTextureUnit);
        delete texture;
        texture=Q_NULLPTR;
    }

    texture = new QOpenGLTexture(image);//.mirrored()

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);//Linear Nearest

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
    //qDebug()<<"Texture ID:"<<texture->textureId();
    //image.save("/home/lex/temp/savefbo_manual_initTextures.bmp");

}

bool XunoGLSLFilter::initShaders2(int pass)
{

    QOpenGLShaderProgram *program=Q_NULLPTR;
    if (pass<=programs.size()){
        program=programs[pass];
    }
    if (program==Q_NULLPTR) {
        qDebug()<<"program not present";
        return false;
    }

    qDebug()<<"initShaders pass"<<pass;

    // Compile vertex shader
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader-mpvxbr-v.glsl"))
        return false;

    // Compile fragment shader
    //    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader-mpvxbr-f.glsl"))
    //        return false;

    QByteArray sourceShader;

    sourceShader.append(QString("#version 130\n#define dpass %1 \n").arg(pass));

    //read include part for shaders
    QString filename;

    if (pass!=maxPass){ //last pass ?
        if (0) {
            filename.append(":/shaders/empty-shader-f.glsl");
        }else{
            filename.append(":/shaders/shader-mpvxbr-f.glsl");
        }
    }else{
        filename.append(":/shaders/shader-blur-f.glsl");
    }
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)){
        sourceShader.append(file.readAll());
        file.close();
    }else{
        qWarning()<<"Can't open file:"<<filename;
    }

    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, sourceShader)){
        return false;
    }

    // Link shader pipeline
    if (!program->link())
        return false;

    // Bind shader pipeline for use
    //    if (!program2.bind())
    //        close();
}

void XunoGLSLFilter::initFrameBufer(int id)
{
    qDebug()<<"initFrameBufer"<<id;
    if (id>1) return;
    if (m_fbo[id]) {
        delete m_fbo[id];
        m_fbo[id]=Q_NULLPTR;
    }
    int scale=1;
    if (id==1) scale=2;
    //m_fbo[id] = new QOpenGLFramebufferObject(this->width()*scale,this->height()*scale,GL_TEXTURE_2D);
}

void XunoGLSLFilter::initFrameBufers()
{
    initFrameBufer(0);
    initFrameBufer(1);
}




