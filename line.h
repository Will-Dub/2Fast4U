#ifndef LINE_H
#define LINE_H

#include <QPainter>
#include <QPixmap>

constexpr static float CAM_D = 1.00f;
constexpr static float ROAD_W = 9.5f;
constexpr static float SEG_L = 3.0f;

struct Line {
    // position 3D
    float x=0, y=0, z=0;
    // position 2D
    float X=0, Y=0, W=0;
    float scale=0, curve=0, spriteX=0, clip=0;
    int nbLane=1;
    bool isLineFull = false;
    QPixmap sprite;

    Line() {curve=x=y=z=0;}

    // Projete le 3D en 2D
    void project(float camX, float camY, float camZ, int screenWidth, int screenHeight, float camAngle, float pitch) {
        float relativeZ = z - camZ;

        // Si derrière ou proche
        if (relativeZ <= 0.01f) {
            scale = 0.0f;
            X = screenWidth / 2.0f;
            Y = screenHeight;
            W = screenWidth;
        } else {
            scale = CAM_D / relativeZ;
            X = (1.0f + scale * (x - camX) - camAngle) * screenWidth / 2.0f;
            Y = (1.0f - scale * (y - camY) + pitch) * screenHeight / 2.0f;
            W = scale * ROAD_W * nbLane * screenWidth / 2.0f;
        }
    }

    void drawSprite(QPainter &painter, int screenWidth, int screenHeight){
        int w = sprite.width();
        int h = sprite.height();

        float destX = X + scale*spriteX*screenWidth/2;
        float destY = Y + 4;
        float destW = w*W/266;
        float destH = h*W/266;

        destX += destW*spriteX; // offset x
        destY += -destH; // offset y

        float visibleH = destH;
        float clipY = clip;

        if (clipY == 0) clipY = screenHeight;

        if (destY >= clipY) return;

        if (destY + destH > clipY) {
            visibleH = clipY - destY;
        }

        if (visibleH <= 0) return;

        float srcVisibleH = (visibleH / destH) * h;
        painter.drawPixmap(
            QRectF(destX, destY, destW, visibleH),
            sprite,
            QRectF(0, 0, w, srcVisibleH)
            );
    }
};

#endif // LINE_H
