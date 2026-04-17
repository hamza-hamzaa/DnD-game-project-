#include "mainwindow.h"

#include <QApplication>

// Program entry: start Qt, show the main window, run the event loop until the app quits.
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
