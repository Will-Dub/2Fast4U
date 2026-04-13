/*==================================================
* NOM: obstacle.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour générer des obstacles durant le jeu
===================================================*/

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QString>

class Obstacle
{
public:
    Obstacle(QString spriteName, int maxFrame, float spriteX, float spriteScale, float hitboxWidth, float movingSpeed = 0)
        : m_currentFrame(0),
        m_maxFrame(maxFrame),
        m_spriteName(spriteName),
        m_spriteX(spriteX),
        m_spriteScale(spriteScale),
        m_hitboxWidth(hitboxWidth),
        m_movingSpeed(movingSpeed),
        m_timeElapsed(0.0f)
    {
    }

    Obstacle() = default;

    void update(float dt) {
        m_timeElapsed += dt;
        if (m_timeElapsed > 0.025f) {
            if (m_maxFrame > 0) {
                m_currentFrame = (m_currentFrame + 1) % m_maxFrame;
            }
            m_spriteX -= m_movingSpeed;
            m_timeElapsed -= 0.100f;
        }
    }

    // On ne retourne plus un QPixmap ici, on retourne juste les infos
    // Le Terrain se chargera de demander la bonne image au SpriteManager
    QString getSpriteName() const { return m_spriteName; }
    int getCurrentFrame() const { return m_currentFrame; }
    float getSpriteX() const { return m_spriteX; }
    float getSpriteScale() const { return m_spriteScale; }
    float getHitboxWidth() const { return m_hitboxWidth; }

private:
    int m_currentFrame;
    int m_maxFrame;
    QString m_spriteName;
    float m_spriteX;
    float m_spriteScale;
    float m_hitboxWidth;
    float m_movingSpeed;
    float m_timeElapsed;
};

#endif // OBSTACLE_H