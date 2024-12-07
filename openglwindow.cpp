#include "openglwindow.h"
#include <QCoreApplication>
#include <memory>
#include <random>
#include <iostream>

OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent),
    m_project(QMatrix4x4()),
    m_modelview(QMatrix4x4()),
    m_animate(false),
    m_update_delay(false),
    m_context(nullptr),
    m_showfull(false),
    m_reset{false}
{
    setSurfaceType(QWindow::OpenGLSurface);
    installEventFilter(this);

    m_thread = std::thread([&,this]()
    {
        std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> range(300,600);
        int timex = range(mt);
        std::cout<<timex<<std::endl;

        std::chrono::steady_clock clk;
        static auto n1 = clk.now();
        static auto n2 = clk.now();
        static auto n3 =
         std::chrono::duration_cast<std::chrono::seconds>(n2.time_since_epoch() - n1.time_since_epoch());
        while(!m_reset)
        {
            if(n3.count() < timex)
            {
                n2 = clk.now();
                n3 = std::chrono::duration_cast<std::chrono::seconds>(n2.time_since_epoch() - n1.time_since_epoch());
                continue;
            }
            else
            {
                QCoreApplication::postEvent(this, new QEvent(QEvent::Quit));
                break;
            }
        }
    });

}

OpenGLWindow::~OpenGLWindow()
{
}

 void OpenGLWindow::setanimating(bool animating)
 {
     m_animate = animating;
     if(m_animate)
     {
         renderLater();
     }

 }

 void OpenGLWindow::renderLater()
 {
    if(!m_update_delay)
    {
        m_update_delay = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
 }

 void OpenGLWindow::renderNow()
 {

    bool bInitial = false;
    if(!m_context)
    {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();
        bInitial = true;
    }

    m_context->makeCurrent(this);
    if(bInitial)
    {
        initializeOpenGLFunctions();
        initial();
        const qreal ratio = devicePixelRatio();
        resizeGL(width() * ratio, height() * ratio);
    }

    render();

    if(isExposed())
    {
        m_context->swapBuffers(this);
    }


    Sleep(50);
    if(m_animate)
    {
        renderLater();
    }
 }

 void OpenGLWindow::resizeGL(int w, int h)
 {
    if(h == 0)
    {
        h = 1;
    }
    if(m_context)
    {
        glViewport(0, 0, w, h);
    }

    m_project.setToIdentity();
    m_project.perspective(35, (float)w/(float)h, 1, 1000);
    m_modelview.setToIdentity();
 }

 void OpenGLWindow::render()
 {

 }

 void OpenGLWindow::initial()
 {

 }

 bool OpenGLWindow::eventFilter(QObject* obj, QEvent* event)
 {
     if((obj == this) && (event->type() == QEvent::Close))
     {
        setanimating(false);
        m_reset = true;
        m_thread.join();
     }
     if((obj == this) && (event->type() == QEvent::Quit))
     {
        setanimating(false);
        m_thread.join();
        QCoreApplication::exit();
     }
     return QWindow::eventFilter(obj, event);
 }

 bool OpenGLWindow::event(QEvent *event)
 {
     switch (event->type())
     {
        case QEvent::UpdateRequest:
         m_update_delay = false;
         renderNow();
        break;
       default:
        break;
     }
     return QWindow::event(event);
 }

 void OpenGLWindow::exposeEvent(QExposeEvent *event)
 {

     renderNow();

     QWindow::exposeEvent(event);
 }

 void OpenGLWindow::resizeEvent(QResizeEvent *event)
 {
     const qreal ratio = QWindow::devicePixelRatio();
     resizeGL(width() * ratio, height() * ratio);
     renderNow();
     QWindow::resizeEvent(event);
 }

 void OpenGLWindow::keyPressEvent(QKeyEvent *event)
 {
     switch (event->key())
     {
        case Qt::Key_F1:
         showFullScreen();
        break;

        case Qt::Key_Escape:
         showNormal();
        break;

        default:
        break;
     }
     QWindow::keyPressEvent(event);
 }

