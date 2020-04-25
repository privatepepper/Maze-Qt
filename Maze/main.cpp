#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;


    w.setGeometry(0, 0, 1440, 900);
    w.setWindowTitle("Maze");

    w.show();
    return a.exec();
}
