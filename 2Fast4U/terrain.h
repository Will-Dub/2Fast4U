#ifndef TERRAIN_H
#define TERRAIN_H

#include <QVector>
#include <QPainter>
#include "line.h"
#include "player.h"
#include <QImageReader>

class Terrain
{
public:
    Terrain();

    constexpr static int N_LINES = 5000;

    void render(QPainter &painter, Player &player, int screenWidth, int screenHeight);
    void generateTerrain();
    void tick(Player &player, float dt);

    Line& getLine(int index);
    const Line& getLine(int index) const;
    int getTotalLines() const;

private:
    QVector<Line> lines;
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

#endif // TERRAIN_H
