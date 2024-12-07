#include "texturewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TextureWindow w;
    w.showMaximized();
    w.hide();
    w.setanimating(true);
    return a.exec();
}
