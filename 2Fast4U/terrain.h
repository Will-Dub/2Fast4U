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
    static constexpr int MAX_DRAW_DISTANCE = 600;
    QVector<Line> lines;

    float precalcFogFactor[MAX_DRAW_DISTANCE];
    QColor precalcGrassLight[MAX_DRAW_DISTANCE];
    QColor precalcGrassDark[MAX_DRAW_DISTANCE];
    QColor precalcRumbleLight[MAX_DRAW_DISTANCE];
    QColor precalcRumbleDark[MAX_DRAW_DISTANCE];
    QColor precalcShoulderLight[MAX_DRAW_DISTANCE];
    QColor precalcShoulderDark[MAX_DRAW_DISTANCE];
    QColor precalcRoadLight[MAX_DRAW_DISTANCE];
    QColor precalcRoadDark[MAX_DRAW_DISTANCE];
    QColor precalcLine[MAX_DRAW_DISTANCE];
    QColor precalcEdgeLine[MAX_DRAW_DISTANCE];
    QColor precalcRoadGlow[MAX_DRAW_DISTANCE];

    /**
     * @brief drawQuad Fait un quad sur l'écran
     * @param x1 x du milieu de la ligne du haut
     * @param y1 y de la ligne du haut
     * @param w1 demi largeur de la ligne du haut
     * @param x2 x du milieu de la ligne du bas
     * @param y2 y de la ligne du bas
     * @param w2 demi largeur de la ligne du bas
     */
    inline void drawQuad(QPainter& painter, const QColor& color, float x1, float y1, float w1, float x2, float y2, float w2);
};

#endif // TERRAIN_H
