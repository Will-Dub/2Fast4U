#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QVector>
#include <QKeyEvent>
#include <QElapsedTimer>
#include "player.h"
#include "line.h"
#include <QTextDocument>
#include <QDebug>
#include "terrain.h"

constexpr static int FRAME_RATE = 180;

class GameWidget: public QWidget
{
    Q_OBJECT
public:
    GameWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    Player player;
    Terrain terrain;

    int frameCount = 0;
    int currentFps = 0;
    QElapsedTimer timer;
    QElapsedTimer fpsTimer;
    QLinearGradient sunset;
};

#endif // GAMEWIDGET_H
