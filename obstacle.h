#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QPixmapCache>
#include <QPixmap>
#include <QString>
#include <QDebug>

class Obstacle
{
public:
    Obstacle(QString spriteName, int maxFrame, float spriteX, float spriteScale, float hitboxWidth)
        : m_currentFrame(0),
        m_maxFrame(maxFrame),
        m_spriteName(spriteName),
        m_spriteX(spriteX),
        m_spriteScale(spriteScale),
        m_hitboxWidth(hitboxWidth){}

    void nextFrame() {
        if (m_maxFrame > 0) {
            m_currentFrame = (m_currentFrame + 1) % m_maxFrame;
        }
    }

    QPixmap getCurrentFrame() const {
        QString cacheFrameKey = m_spriteName + QString::number(m_currentFrame);
        QPixmap framePixmap;

        // Check si l'image est déjà en cache
        if (QPixmapCache::find(cacheFrameKey, &framePixmap)) {
            return framePixmap;
        }

        qWarning() << "Warning: Image pas en cache " << cacheFrameKey;

        // Load l'image
        QString filePath= QString(":/assets/%1.png").arg(cacheFrameKey);
        if (framePixmap.load(filePath)) {
            QPixmapCache::insert(cacheFrameKey, framePixmap);
        } else {
            qCritical() << "Fatal: Asset manquant " << filePath;
            QPixmapCache::insert(cacheFrameKey, QPixmap());
        }

        return framePixmap;
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

};

#endif // OBSTACLE_H
