#include "oglwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat sformat;
    sformat.setSamples(16);
    sformat.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(sformat);

    OGLWidget oglw;
    oglw.show();

    return a.exec();
}
