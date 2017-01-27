#include "XunoGlslFilter.h"
#include <QDebug>
#include <QImage>

XunoGLSLFilter::XunoGLSLFilter(QObject *parent):
    QtAV::GLSLFilter(parent)
{
    qDebug()<<"constructor XunoGLSLFilter::XunoGLSLFilter";
    if (opengl()){
        qDebug()<<"connect XunoGLSLFilter::XunoGLSLFilter";
        connect(opengl(), &QtAV::OpenGLVideo::beforeRendering, this, &XunoGLSLFilter::beforeRendering, Qt::DirectConnection);
        connect(opengl(), &QtAV::OpenGLVideo::afterRendering, this, &XunoGLSLFilter::afterRendering, Qt::DirectConnection);
    }
}

void XunoGLSLFilter::setShader(QtAV::VideoShader *ush)
{
    user_shader=ush;
    opengl()->setUserShader(user_shader);
}

void XunoGLSLFilter::beforeRendering()
{
    qDebug()<<"XunoGLSLFilter::beforeRendering";
    colorTransform();
    if (fbo() && fbo()->isValid()){
        QOpenGLFunctions *f=opengl()->openGLContext()->functions();
        if (f && fbo()->textures().size()){
            GLint target=GL_TEXTURE_2D;
            f->glBindTexture(target,fbo()->texture());
            f->glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            f->glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            f->glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            f->glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            f->glBindTexture(target,0);
        }
    }
}


void XunoGLSLFilter::afterRendering()
{
    qDebug()<<"XunoGLSLFilter::afterRendering()";
    if (fbo() && fbo()->isValid() ) {
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
        if (1){
            superscale();
        }
    }
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


#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

void XunoGLSLFilter::superscale()
{
    //QOpenGLFramebufferObject *fbo;
    //QSize size;
    //OpenGLVideo glv;

    QOpenGLFunctions *f=opengl()->openGLContext()->functions();

    //if (initSize.isEmpty()){
    initSize=outputSize();
    //}

    qDebug()<<"superscale" << initSize;
    //setOutputSize(initSize*2);
    //qDebug()<<"superscale x2" << outputSize();


    initTextures();

    //initFrameBufer();

    frame++;

    //! [2]
    // Enable depth buffer
    f->glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    f->glEnable(GL_CULL_FACE);
    //! [2]

    geometries = new GeometryEngine;

    shader_files_prefix=":/shaders/";
    //shader_files_prefix="/home/lex/project-C/github/SuperXbr-GL/depend/common-shaders/xbr/";

    //    shader_files_include="shaders/super-xbr/super-xbr-params.inc";

    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass0.glsl";
    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass1.glsl";
    //    shader_files<<"shaders/super-xbr/super-xbr-fast-pass2.glsl";
    //    shader_files<<"shaders/super-xbr/custom-jinc2-sharper.glsl";

    shader_files_include="super-xbr-params.inc";

    shader_files<<"super-xbr-pass0.glsl";
    shader_files<<"super-xbr-pass1.glsl";
    //shader_files<<"custom-jinc2-sharper.glsl";

    maxPass=shader_files.size()-1;

    maxPass=3+0;//last blur
    //scales per pass relative to previos fbo, note: source first texture always scale=1.0
    //scales<<2<<1<<1<<1;
    //scales<<1<<2<<1<<1<<2<<1<<1;
    scales<<2<<1<<1<<1<<1;


    QMatrix4x4 matrix;


    //---------------------------------------------
    GLint fbotextid=texture->textureId();

    f->glUseProgram(0);
    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    qDebug()<<"Texture id start"<<fbotextid<<"texure size:"<<texture->width()<<"x"<<texture->height();

    for (pass=0;pass<=maxPass;pass++){


        bool rotate=false;//(pass>0);

        qDebug()<<"Programs:"<<addProgram();
        if (initShaders_simple(pass) && !scales.isEmpty()){
            int fboID=addFBO(scales.at(pass),/*rotate*/0);

            QOpenGLShaderProgram *program=Q_NULLPTR;
            if (pass<=programs.size()){
                program=programs[pass];
            }

            m_fbo[fboID]->bind();
            //            GLenum FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            bool FBOstatus = m_fbo[fboID]->isValid();
            qDebug()<<"glCheckFramebufferStatus:"<<FBOstatus<<glGetError();

            glViewport(0,0,m_fbo[fboID]->width(),m_fbo[fboID]->height());
            // Use texture unit 0 which contains cube.png

            program->bind();
            program->setUniformValue("Texture0",  0);

            //program->setUniformValue("pass", fboID);

            //program->setUniformValue("OutputSize",QVector2D(m_fbo[fboID]->width(),m_fbo[fboID]->height()));
            //program->setUniformValue("TextureSize",QVector2D(m_fbo[fboID]->width(),m_fbo[fboID]->height()));

            QVector2D textureSize=QVector2D (float(m_fbo[fboID]->width()),float(m_fbo[fboID]->height()));

            program->setUniformValue("texture_size",textureSize);
            program->setUniformValue("pixel_size",QVector2D(1.0f,1.0f)/textureSize);

            //program->setUniformValue("TextureSize",QVector2D(texture->width(),texture->height()));
            //program->setUniformValue("InputSize",QVector2D(m_fbo[fboID]->width(),m_fbo[fboID]->height()));


            QMatrix4x4 matrix;
            matrix.setToIdentity();



            if (rotate) {
                int sign=(pass==2)?-1:1;
                matrix.rotate(180*sign,0.,0.,1.);
            }

            program->setUniformValue("mvp_matrix", matrix);



            if (0){

                QOpenGLBuffer buffer1(QOpenGLBuffer::VertexBuffer);
                buffer1.create();

                float vertices[] = {
                    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
                    0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
                    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
                    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
                };

                buffer1.bind();
                buffer1.allocate(vertices,sizeof(float)*5*4);
                // buffer1.release();


                QOpenGLBuffer buffer2(QOpenGLBuffer::IndexBuffer);

                GLuint elements[] = {
                    0, 1, 2,
                    2, 3, 0
                };

                buffer2.bind();
                buffer2.allocate(elements,sizeof(GLuint)*6);
                //buffer2.release();


                QOpenGLShaderProgram* shaderProgram_ = new QOpenGLShaderProgram;
                shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/simple_ver.glsl");
                shaderProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple_frag.glsl");
                bool ok = shaderProgram_->link();
                ok = shaderProgram_->bind();

                shaderProgram_->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
                shaderProgram_->enableAttributeArray("vertex");

                f-> glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



            }

            if (1) {
                glActiveTexture(GL_TEXTURE0);

                if (pass==0){
                    glEnable(GL_TEXTURE_2D);
                    //texture->bind();
                    glBindTexture(GL_TEXTURE_2D, fbotextid);
                }else{
                    f->glBindTexture(GL_TEXTURE_2D, fbotextid);
                    f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);//GL_NEAREST GL_LINEAR
                    f->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                }


                f->glClearColor(1.0,0.0,0.0,1.0);//RED
                f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Draw cube geometry
                geometries->drawCubeGeometry(program);

                if (pass==0){
                    texture->release();
                }else{
                    f->glBindTexture(GL_TEXTURE_2D, 0);
                }

            }
            program->release();
            m_fbo[fboID]->release();
            QString filename=QString("/home/lex/temp/savefbo_pass_%1_%2x%3-%4.bmp").arg(pass).arg(m_fbo[fboID]->width()).arg(m_fbo[fboID]->height()).arg(frame);
            qDebug()<<"Saving:"<<filename;
            m_fbo[fboID]->toImage().save(filename);

            fbotextid=m_fbo[fboID]->texture();
            qDebug()<<"Texture id"<<fbotextid<<"texure size:"<<texture->width()<<"x"<<texture->height();

        }else{
            qDebug()<<"initShaders error (pass)"<<pass;
        }
    }



    //clear
    while (m_fbo.size()) {
        delete m_fbo.takeLast();
    }
    while (programs.size()) {
        delete programs.takeLast();
    }

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


    qDebug()<<"initShaders pass"<<pass;

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

int XunoGLSLFilter::addProgram()
{
    QOpenGLShaderProgram *pr= new QOpenGLShaderProgram(this);
    programs.append(pr);
    return programs.size()-1;
}

int XunoGLSLFilter::addFBO(int scale, bool rotate)
{
    int texture_width=0, texture_height=0;
    int prev_id=m_fbo.size()-1;
    if (prev_id<0){
        texture_width=texture->width();
        texture_height=texture->height();
    }else{
        texture_width=m_fbo[prev_id]->width();
        texture_height=m_fbo[prev_id]->height();
    }
    texture_width*=scale;
    texture_height*=scale;

    if (rotate){
        int t=texture_width;
        texture_width=texture_height;
        texture_height=t;
    }

    QOpenGLFramebufferObject* fbo = new QOpenGLFramebufferObject(texture_width,texture_height,GL_TEXTURE_2D);
    m_fbo.append(fbo);
    return m_fbo.size()-1;

}

void XunoGLSLFilter::initTextures()
{

    qDebug()<<"hasOpenGLFramebufferBlit()"<<QOpenGLFramebufferObject::hasOpenGLFramebufferBlit();

    // Load cube.png image
    //QImage image=QImage(":/savefbo_pass_0_1920x1080-empty-shader.bmp");//RAW_Yamaha_960x540.bmp
    //QImage image=QImage("/home/lex/temp/RAW_Yamaha_960x540.bmp");

    QImage image=fbo()->toImage(false);
    //this->resize(image.size());

    qDebug()<<"After Resize";

    if (texture!=Q_NULLPTR){
        delete texture;
        texture=Q_NULLPTR;
    }

    texture = new QOpenGLTexture(image.mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);//Linear Nearest

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

    image.save("/home/lex/temp/savefbo_manual_initTextures.bmp");

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




