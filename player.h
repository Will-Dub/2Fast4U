#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include "line.h"

class Player
{
public:
    Player();

    constexpr static int MIN_PLAYER_Y = 15;

    bool keyLeft = false;
    bool keyRight = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keySpace = false;
    float pitch = 0;

    float getPositionX() {return m_positionX; }
    float getPositionY() {return m_positionY; }
    float getPositionZ() {return m_positionZ; }
    float getAngle() {return m_angle; }

    void tick(float dt, float currentCurve, float currentSlope, float terrainFriction);
    void crash(float distancePushBack);

private:
    // Position
    float m_positionX = 0;
    float m_positionY = MIN_PLAYER_Y;
    float m_positionZ = 0;

    float m_angle = 0.0f;
    float m_turnSpeed = 2.0f;

    float m_potAccel = 0;
    float m_potSteering = 0;

    float m_velocite = 0;
    float m_acceleration = 0;
    float m_masse = 1200.0f;

    float m_coefficientDrag = 0.30f;
    float m_friction = 50.0;
    float m_forceMax = 8000.0f;
};

#endif // PLAYER_H
