#include "mainwindow.h"

#include <QApplication>
#include "gamewidget.h"
#include <QImageReader>

int main(int argc, char *argv[])
{
    QImageReader::setAllocationLimit(0);

    QApplication a(argc, argv);
    MainWindow w;

    GameWidget *game = new GameWidget(&w);
    w.setCentralWidget(game);
    w.resize(1920,1080);

    w.show();
    return a.exec();
}
