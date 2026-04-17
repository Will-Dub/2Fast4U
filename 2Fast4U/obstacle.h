/*==================================================
* NOM: obstacle.h
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les méthodes et la classe pour générer et afficher des obstacles durant le jeu
===================================================*/

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QString>
#include <cmath>

enum class ObstacleState {
    Idle,
    Moving,
    Running
};

enum class ObstacleType {
    Arbre,
    Roche,
    Grass,
    Pole,
    Buche,
    Cerf
};

class Obstacle
{
public:
    Obstacle(QString spriteName, ObstacleType type, float spriteX, float spriteScale, float hitboxWidth, ObstacleState startingState = ObstacleState::Idle, float spriteY = 0.0f, int directionX = 1)
        : m_currentFrame(0),
        m_type(type),
        m_spriteName(spriteName),
        m_spriteX(spriteX),
		m_spriteY(spriteY),
        m_spriteScale(spriteScale),
        m_hitboxWidth(hitboxWidth),
        m_timeElapsed(0.0f),
        m_directionX(directionX),
		m_state(startingState)
    {
    }

    Obstacle() = default;

    void update(float dt, float carY) {
		if (m_type != ObstacleType::Cerf) return;
        qInfo() << carY;

		float distance = std::abs(carY - m_spriteY);
        static const float runningTriggerDistance = 50.0f;
        static const float walkingTriggerDistance = 110.0f;

        if (m_state == ObstacleState::Idle && distance <= walkingTriggerDistance) {
            m_state = ObstacleState::Moving;
            m_currentFrame = 0;
            m_timeElapsed = 0.0f;
        }

        if (m_state != ObstacleState::Running && distance <= runningTriggerDistance) {
            m_state = ObstacleState::Running;
            m_currentFrame = 0;
            m_timeElapsed = 0.0f;
        }

        float currentSpeedX = 0.0f;
        float currentAnimationDelay = 0.0f;
        switch (m_state) {
            case ObstacleState::Idle:
                currentSpeedX = 0.0f;
                currentAnimationDelay = 0.0f;
                break;
            case ObstacleState::Moving:
                currentSpeedX = 5.0f;
                currentAnimationDelay = 0.2f;
                break;
            case ObstacleState::Running:
                currentSpeedX = 10.0f;
                currentAnimationDelay = 0.05f;
				break;
        }

        if(m_state != ObstacleState::Moving) {
            m_spriteX -= currentSpeedX * m_directionX * dt;
		}

        if (currentAnimationDelay > 0.0f) {
            m_timeElapsed += dt;

            if (m_timeElapsed >= currentAnimationDelay) {
                if (m_state == ObstacleState::Moving) {
                    if (m_currentFrame == 0) {
                        m_currentFrame = 2;
                    }
                    else if (m_currentFrame == 2) {
                        m_currentFrame = 3;
                    }
                    else {
                        m_currentFrame = 0;
                    }
                    m_spriteX -= currentSpeedX * m_directionX * dt;
                }
                else {
                    m_currentFrame = (m_currentFrame + 1) % 7;
                }

                m_timeElapsed -= currentAnimationDelay;
            }
        }
        else {
            m_currentFrame = 7;
        }

    }

    QString getSpriteName() const { return m_spriteName; }
    int getCurrentFrame() const { return m_currentFrame; }
    float getSpriteX() const { return m_spriteX; }
    float getSpriteScale() const { return m_spriteScale; }
    float getHitboxWidth() const { return m_hitboxWidth; }
    int getDirectionX() const { return m_directionX; }

private:
    int m_directionX;
    int m_currentFrame;
    ObstacleType m_type;
	ObstacleState m_state;
    QString m_spriteName;
    float m_spriteX;
    float m_spriteY;
    float m_spriteScale;
    float m_hitboxWidth;
    float m_timeElapsed;
};

#endif // OBSTACLE_H