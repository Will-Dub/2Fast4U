#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPixmapCache>
#include <QPixmap>
#include <QString>
#include <QDebug>
#include "spritemanager.h"

class Obstacle
{
public:
    Obstacle(QString spriteName, int maxFrame, float spriteX, float spriteScale, float hitboxWidth, float movingSpeed=0)
        : m_currentFrame(0),
        m_maxFrame(maxFrame),
        m_spriteName(spriteName),
        m_spriteX(spriteX),
        m_spriteScale(spriteScale),
        m_hitboxWidth(hitboxWidth),
        m_movingSpeed(movingSpeed)
    {
        generateFogCache();
    }

    void nextFrame() {
        if (m_maxFrame > 0) {
            m_currentFrame = (m_currentFrame + 1) % m_maxFrame;
        }
    }

    QPixmap getCurrentFrame() const {
        QString cacheFrameKey = m_spriteName + QString::number(m_currentFrame);
        return SpriteManager::get(cacheFrameKey);
    }

    void update(float dt){
        m_timeElapsed += dt;
        if(m_timeElapsed>0.025){
            nextFrame();
            m_spriteX -= m_movingSpeed;
            m_timeElapsed -= 0.100f;
        }
    }

    void generateFogCache() {
        static const QColor FOG_COLOR(135, 206, 235);

        int totalFrames = (m_maxFrame > 0) ? m_maxFrame : 1;
        m_foggedFrames.resize(totalFrames);

        for (int f = 0; f < totalFrames; ++f) {
            QString cacheFrameKey = m_spriteName + QString::number(f);
            QPixmap baseSprite = SpriteManager::get(cacheFrameKey);

            // Pour chaque frame, on génère les 10 niveaux de brouillard
            for (int i = 0; i < 10; ++i) {
                float fogFactor = i / 10.0f;

                QPixmap tintedPixmap(baseSprite.size());
                tintedPixmap.fill(Qt::transparent);

                QPainter p(&tintedPixmap);
                p.drawPixmap(0, 0, baseSprite);
                p.setCompositionMode(QPainter::CompositionMode_SourceAtop);

                QColor currentFog = FOG_COLOR;
                currentFog.setAlphaF(fogFactor);
                p.fillRect(tintedPixmap.rect(), currentFog);
                p.end();

                m_foggedFrames[f].append(tintedPixmap);
            }
        }
    }

    // Get le bon frame en fonction du facteur
    QPixmap getFoggedFrame(float fogFactor) const {
        if (m_foggedFrames.isEmpty()) {
            return getCurrentFrame();
        }

        int index = std::max(0, std::min(9, (int)(fogFactor * 10)));
        return m_foggedFrames[m_currentFrame][index];
    }

    float getSpriteX() const{
        return m_spriteX;
    }

    float getSpriteScale() const{
        return m_spriteScale;
    }

    float getHitboxWidth() const{
        return m_hitboxWidth;
    }

private:
    int m_currentFrame;
    int m_maxFrame;
    QString m_spriteName;
    float m_spriteX;
    float m_spriteScale;
    float m_hitboxWidth;
    float m_movingSpeed;
    float m_timeElapsed = 0.0f;

    QVector<QVector<QPixmap>> m_foggedFrames;
};

#endif // OBSTACLE_H
