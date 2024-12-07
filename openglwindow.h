#ifndef WIDGET_H
#define WIDGET_H

#include <QWindow>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <thread>
#include <chrono>

class OpenGLWindow : public QWindow, public QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    OpenGLWindow(QWindow *parent = nullptr);
    ~OpenGLWindow();

    void setanimating(bool animating);


public slots:
    void renderLater();
    void renderNow();


protected:
    bool event(QEvent *) override;
    void exposeEvent(QExposeEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    virtual void render();
    virtual void initial();
    virtual void resizeGL(int w, int h);

    QMatrix4x4 m_project;
    QMatrix4x4 m_modelview;
    bool m_animate;

private:
    bool m_update_delay;
    QOpenGLContext* m_context;
    bool m_showfull;
    std::thread m_thread;
    bool m_reset;
};
#endif // WIDGET_H
