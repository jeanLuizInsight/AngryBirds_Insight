#include <QApplication>
#include "glwidget.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GLWidget gl_widget;
    /* tela full */
    gl_widget.setWindowState(Qt::WindowFullScreen);
    gl_widget.show();

    return app.exec();
}

