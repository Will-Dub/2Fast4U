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
#include "serialcontroller.h"
#include "racemanager.h"

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
    void restartGame();

private:
    Player m_player;
    Terrain m_terrain;
    SerialController m_serialController;
    RaceManager m_raceManager;

    int m_frameCount = 0;
    int m_currentFps = 0;
    QElapsedTimer m_timer;
    QElapsedTimer m_fpsTimer;
    QLinearGradient m_sunset;
};

#endif // GAMEWIDGET_H
