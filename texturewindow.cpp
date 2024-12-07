#include "texturewindow.h"
#include <QImage>
#include <gl/GL.h>
#include <QDir>

struct VertexData
{
    QVector3D position;
    QVector2D textCoord;
};

TextureWindow::TextureWindow(QWindow* parent)
    :OpenGLWindow(parent),m_texture(-1), m_texturefbo(-1),m_ox(0.0f), m_oy(0.0f), m_oz(0.0f),
       m_fbo(-1), m_pixels(nullptr)
{

}

TextureWindow::~TextureWindow()
{
    glDeleteTextures(1, &m_texture);
    glDeleteTextures(1, &m_texturefbo);
    glDeleteBuffers(2, &m_vboid[0]);
    glDeleteFramebuffers(1, &m_fbo);
    delete[] m_pixels;
}

void TextureWindow::InitGeo()
{
    glGenBuffers(2, &m_vboid[0]);
    VertexData verts[] =
    {

        {QVector3D{-1.0f,-1.0f,1.0f}, QVector2D{0.0f, 0.0f}},  //顶
        {QVector3D{1.0f,-1.0f,1.0f}, QVector2D{1.0f, 0.0f}},
        {QVector3D{-1.0f, 1.0f,1.0f}, QVector2D{0.0f, 1.0f}},
        {QVector3D{1.0f,1.0f,1.0f}, QVector2D{1.0f, 1.0f}},

        {QVector3D{1.0f,-1.0f,-1.0f}, QVector2D{0.0f, 0.0f}},
        {QVector3D{-1.0f,-1.0f,-1.0f}, QVector2D{1.0f, 0.0f}},  //底
        {QVector3D{1.0f,1.0f,-1.0f}, QVector2D{0.0f, 1.0f}},
        {QVector3D{-1.0f, 1.0f,-1.0f}, QVector2D{1.0f, 1.0f}},

        {QVector3D{-1.0f,1.0f,-1.0f}, QVector2D{0.0f, 0.0f}},  //左
        {QVector3D{-1.0f,-1.0f,-1.0f}, QVector2D{1.0f, 0.0f}},
        {QVector3D{-1.0f, 1.0f,1.0f}, QVector2D{0.0f, 1.0f}},
        {QVector3D{-1.0f,-1.0f,1.0f}, QVector2D{1.0f, 1.0f}},

        {QVector3D{1.0f,-1.0f,-1.0f}, QVector2D{0.0f, 0.0f}},
        {QVector3D{1.0f,1.0f,-1.0f}, QVector2D{1.0f, 0.0f}},
        {QVector3D{1.0f,-1.0f,1.0f}, QVector2D{0.0f, 1.0f}},  //右
        {QVector3D{1.0f, 1.0f,1.0f}, QVector2D{1.0f, 1.0f}},


        {QVector3D{-1.0f,-1.0f,-1.0f}, QVector2D{0.0f, 0.0f}},  //前
        {QVector3D{1.0f,-1.0f,-1.0f}, QVector2D{1.0f, 0.0f}},
        {QVector3D{-1.0f, -1.0f,1.0f}, QVector2D{0.0f, 1.0f}},
        {QVector3D{1.0f,-1.0f,1.0f}, QVector2D{1.0f, 1.0f}},

        {QVector3D{1.0f,1.0f,-1.0f}, QVector2D{0.0f, 0.0f}},    //后
        {QVector3D{-1.0f,1.0f,-1.0f}, QVector2D{1.0f, 0.0f}},
        {QVector3D{1.0f,1.0f,1.0f}, QVector2D{0.0f, 1.0f}},
        {QVector3D{-1.0f, 1.0f,1.0f}, QVector2D{1.0f, 1.0f}},


    };

    GLushort indics[] =
    {
        0, 1, 2, 3, 3,
        4, 4, 5, 6, 7, 7,
        8, 8, 9, 10, 11, 11,
        12, 12, 13, 14, 15, 15,
        16, 16, 17, 18, 19, 19,
        20, 20, 21, 22, 23
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_vboid[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof (verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboid[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indics), indics, GL_STATIC_DRAW);



}


void TextureWindow::LoadShader()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/v.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/f.glsl");
    m_program->link();
    m_posattr = m_program->attributeLocation("cubepos");
    m_coord = m_program->attributeLocation("textparam");



}

void TextureWindow::LoadGLTexture()
{
    QImage img(":/img/2.jpg");
    img = img.convertToFormat(QImage::Format_RGB888);
 //   img = img.mirrored();

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(),
                 0, GL_RGB, GL_UNSIGNED_BYTE, img.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void TextureWindow::initial()
{
    InitGeo();
    LoadShader();
    LoadGLTexture();
    createFramebuffer();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.5f,0.0f,0.5f,1.0f);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
}

// 创建帧缓冲对象
void TextureWindow::createFramebuffer() {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_texturefbo);
    glBindTexture(GL_TEXTURE_2D, m_texturefbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texturefbo, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qDebug() << "Framebuffer not complete!";
        exit(EXIT_FAILURE);
    }

    if(m_pixels == nullptr)
    {
        m_pixels = new GLubyte[width() * height() * 3]; // 分配足够的空间来存储RGBA数据
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void TextureWindow::render()
{
    m_program->bind();

    // 选择渲染目标
    if (isExposed()) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_modelview.setToIdentity();
    m_modelview.translate(0.0f, 0.0f, -5.0f);
    m_modelview.rotate(m_ox, 1.0, 0.0, 0.0);
    m_modelview.rotate(m_oy, 0.0, 1.0, 0.0);
    m_modelview.rotate(m_oz, 0.0, 0.0, 1.0);

    m_program->setUniformValue("promod_mat", m_project * m_modelview);
    quintptr offset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, m_vboid[0]);
    m_program->enableAttributeArray(m_posattr);
    glVertexAttribPointer(m_posattr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);
    offset += sizeof(QVector3D);
    m_program->enableAttributeArray(m_coord);
    glVertexAttribPointer(m_coord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboid[1]);
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);



    if (isExposed())
    {
           // 读取纹理数据并保存为图片
       glBindTexture(GL_TEXTURE_2D, m_texture);
    }
    else
    {
       // 读取离屏缓冲区数据并保存为图片
       glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    memset(m_pixels, 0, width() * height() * 3);
    glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_BYTE, m_pixels);

    QImage img(m_pixels, width(), height(), QImage::Format_RGB888);

    if(m_animate)
    {
        QString path(QDir::currentPath() + QDir::separator() + QString::fromStdString("bg"));
        QString str = QDir::currentPath() + QDir::separator() + QString::fromStdString("bg")
                + QDir::separator() + QString::fromStdString("bg.jpg");
        QDir pre(path);
        pre.mkpath(QDir::currentPath() + QDir::separator() + QString::fromStdString("bg"));
        if(!img.save(str))
        {
            qDebug()<<"save failed";
        }
        else
        {
            const int maxlen = 1024;
            char narrowStr[maxlen]{0};
            memcpy(narrowStr, str.toStdString().c_str(), str.toStdString().length());
            size_t len = std::strlen(narrowStr) + 1; // +1 for the null terminator
            size_t wcharCount = 0;
            wchar_t* wideStr = nullptr;

            // First call to determine the required buffer size
            mbstowcs_s(&wcharCount, nullptr, 0, narrowStr, len);

            // Allocate the buffer
            wideStr = new wchar_t[wcharCount];

            // Perform the conversion
            mbstowcs_s(nullptr, wideStr, wcharCount, narrowStr, len);

            SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)wideStr, SPIF_UPDATEINIFILE);

            delete[] wideStr;
        }
    }

    m_program->release();

    m_ox+=0.3f;
    m_oy+=0.2f;
    m_oz+=0.4f;
}
