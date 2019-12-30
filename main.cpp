#include "mainWindow.h"

#include <QApplication>
#include <QStyleFactory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/window_icon.png"));
    a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    window.show();
    return a.exec();
}
