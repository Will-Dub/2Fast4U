#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include "line.h"
#include "serialcontroller.h"

#define STEERING_INPUT input.steering
#define ACCEL_INPUT m_potSteeringFromInput
//#define STEERING_INPUT m_potSteering
//#define ACCEL_INPUT m_potAccel

constexpr float CHASSIS_ROLL_STIFFNESS = 15.0f;
constexpr float STEERING_LEAN_RATIO = 0.003f;
constexpr float CURVE_LEAN_RATIO = 0.015f;

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

    float getPositionX() { return m_positionX; }
    float getPositionY() { return m_positionY; }
    float getPositionZ() { return m_positionZ; }
    float getAngle() { return m_angle; }
    int getSpeed() { return m_velocite*3.6f; }

    void tick(float dt, float currentCurve, float currentSlope, float terrainFriction, const InputState& input);
    void crash(float distancePushBack);
    float getHitboxWidth() const;

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

    float m_hitboxWidth = 2.0f;
};

#endif // PLAYER_H
