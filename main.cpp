#include "mainwindow.h"

#include <QApplication>
#include "gamewidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    GameWidget *game = new GameWidget(&w);
    w.setCentralWidget(game);
    w.resize(1024,768);

    w.show();
    return a.exec();
}
