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

constexpr static int FRAME_RATE = 180;
constexpr static int N_LINES = 2000;

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

    QVector<Line> lines;

    int frameCount = 0;
    int currentFps = 0;
    QElapsedTimer timer;
    QElapsedTimer fpsTimer;
    QLinearGradient sunset;
    QPixmap testSprite;

    /**
     * @brief drawQuad Fait un quad sur l'écran
     * @param x1 x du milieu de la ligne du haut
     * @param y1 y de la ligne du haut
     * @param w1 demi largeur de la ligne du haut
     * @param x2 x du milieu de la ligne du bas
     * @param y2 y de la ligne du bas
     * @param w2 demi largeur de la ligne du bas
     */
    void drawQuad(QPainter &painter, QColor color, int x1, int y1, int w1, int x2, int y2, int w2);
};

#endif // GAMEWIDGET_H
