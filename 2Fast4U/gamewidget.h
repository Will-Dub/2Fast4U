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

constexpr static int FRAME_RATE = 260;

enum class EndType {
    Crash,
    MotorExploded,
    Win
};

class GameWidget: public QWidget
{
    Q_OBJECT
public:
    GameWidget(QWidget *parent = nullptr);
    void setNom(const QString& nom);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void restartGame();
    void resumeGame();
    void pauseGame();

private slots:
    void gameLoop();

signals:
    void pausePressed();
	void endGame(EndType type, QString nom, double temps);

private:
    Player m_player;
    Terrain m_terrain;
    SerialController m_serialController;
    RaceManager m_raceManager;

    int m_frameCount = 0;
    int m_currentFps = 0;
    QElapsedTimer m_timer;
    QElapsedTimer m_fpsTimer;
    QTimer m_loopTimer;
    bool m_isEscapePressed = false;
};

#endif // GAMEWIDGET_H
