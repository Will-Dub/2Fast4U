/*==================================================
* NOM: terrain.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe qui permet de stocker le terrain et d'afficher le terrain
===================================================*/
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

    void render(QPainter& painter, Player& player, int screenWidth, int screenHeight, float finishLineZ);
    void generateTerrain();
    void tick(Player& player, float dt);

    Line& getLine(int index);
    const Line& getLine(int index) const;
    int getTotalLines() const;
    void setMaxDrawDistance(int distance) { m_max_draw_distance = distance; }

    void generateRandomObstacle(Player& player);

private:
    int m_max_draw_distance = 600;
    QVector<Line> lines;

    float precalcFogFactor[1000];
    QColor precalcGrassLight[1000];
    QColor precalcGrassDark[1000];
    QColor precalcRumbleLight[1000];
    QColor precalcRumbleDark[1000];
    QColor precalcShoulderLight[1000];
    QColor precalcShoulderDark[1000];
    QColor precalcRoadLight[1000];
    QColor precalcRoadDark[1000];
    QColor precalcLine[1000];
    QColor precalcEdgeLine[1000];
    QColor precalcRoadGlow[1000];

    /**
     * @brief drawQuad Fait un quad sur l'écran
     * @param x1 x du milieu de la ligne du haut
     * @param y1 y de la ligne du haut
     * @param w1 demi largeur de la ligne du haut
     * @param x2 x du milieu de la ligne du bas
     * @param y2 y de la ligne du bas
     * @param w2 demi largeur de la ligne du bas
     */
    inline void drawQuad(QPainter& painter, QColor color, int x1, int y1, int w1, int x2, int y2, int w2);
    void drawFinishRoadMarking(QPainter& painter, const Line& previousLine, const Line& line);
};

#endif // TERRAIN_H
