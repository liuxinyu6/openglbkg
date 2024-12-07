#ifndef TEXTUREWINDOW_H
#define TEXTUREWINDOW_H

#include "openglwindow.h"
#include <QOpenGLShaderProgram>

class TextureWindow : public OpenGLWindow
{
    Q_OBJECT
public:
    explicit TextureWindow(QWindow* window=nullptr);
    ~TextureWindow();

protected:
    void initial() override;
    void render() override;

private:
    void LoadGLTexture();
    void LoadShader();
    void InitGeo();

    void createFramebuffer();

private:
    QOpenGLShaderProgram* m_program;
    GLuint m_posattr;   //绘制面坐标
    GLuint m_coord;     //源图片坐标
    GLuint m_texture;
    GLuint m_texturefbo;
    GLuint m_vboid[2];
    GLfloat m_ox;
    GLfloat m_oy;
    GLfloat m_oz;
    GLuint m_fbo;

    GLubyte* m_pixels;
};

#endif // TEXTUREWINDOW_H
