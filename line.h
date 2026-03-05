#ifndef LINE_H
#define LINE_H

#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include "obstacle.h"

constexpr static float CAM_D = 1.00f;
constexpr static float ROAD_W = 9.5f;
constexpr static float SEG_L = 3.0f;

struct Line {
    // position 3D
    float x=0, y=0, z=0;
    // position 2D
    float X=0, Y=0, W=0;
    float scale=0, curve=0, clip=0;
    int nbLane=1;
    bool isLineFull = false;
    QList<Obstacle> obstacles;

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
        if (obstacles.isEmpty()) return;

        for(Obstacle& obstacle : obstacles){
            QPixmap sprite = obstacle.getCurrentFrame();
            if(sprite.isNull()) continue;

            int w = sprite.width();
            int h = sprite.height();

            float destW = w * scale * screenWidth * obstacle.getSpriteScale();
            float destH = h * scale * screenWidth * obstacle.getSpriteScale();

            float destX = X + (W * obstacle.getSpriteX() / nbLane) - (destW / 2.0f);
            float destY = Y - destH;

            float visibleH = destH;
            float clipY = clip;

            if (clipY == 0) clipY = screenHeight;

            if (destY >= clipY) continue;

            if (destY + destH > clipY) {
                visibleH = clipY - destY;
            }

            if (visibleH <= 0) continue;

            float srcVisibleH = (visibleH / destH) * h;
            painter.drawPixmap(
                QRectF(destX, destY, destW, visibleH),
                sprite,
                QRectF(0, 0, w, srcVisibleH)
            );

#ifdef QT_DEBUG
            painter.save();

            painter.setBrush(Qt::NoBrush);

            // Footprint du png
            painter.setPen(QPen(Qt::yellow, 1, Qt::DashLine));
            painter.drawRect(QRectF(destX, destY, destW, destH));

            // Hitbox
            float hitW = obstacle.getHitboxWidth();
            float hitDestW = (W * hitW) / nbLane;

            float hitDestX = X + (W * obstacle.getSpriteX() / nbLane) - (hitDestW / 2.0f);

            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
            painter.drawRect(QRectF(hitDestX, destY, hitDestW, destH));

            painter.setPen(QPen(Qt::green, 4, Qt::SolidLine));
            painter.drawPoint(QPointF(X + (W * obstacle.getSpriteX() / nbLane), destY + destH));

            painter.restore();
#endif
        }
    }
};

#endif // LINE_H
