#ifndef PLAYER_H
#define PLAYER_H

#include <QDebug>
#include <QtMath>
#include "line.h"
#include "serialcontroller.h"
#include "powertrainaudiocontroller.h"
#include "powertrain.h"
#include "virtualshifter.h"
#include "racemanager.h"

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

    bool keyLeft = false;
    bool keyRight = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keySpace = false;
    bool key1 = false;
    bool key2 = false;
    bool key3 = false;
    bool key4 = false;
    bool key5 = false;
    bool key6 = false;
    bool keyN = false;
    float pitch = 0;

    float getPositionX() { return m_positionX; }
    float getPositionY() { return m_positionY; }
    float getPositionZ() { return m_positionZ; }
    float getAngle() { return m_angle; }
    bool getIsMotorStarted() { return m_powertrain.getStarted(); }
    bool getIsMotorExploded() { return m_powertrain.getIsMotorExploded(); }
    int getSpeed();
    int getRevs();

    void tick(float dt, float currentCurve, float currentSlopeAngle, float terrainFriction, const InputState& input);
    void crash();
    void pause();
    void resume();
    void restart();
    void renderHabitacle(QPainter *painter, RaceState raceState, int screenWidth, int screenHeight);
    void allumer();
    void eteindre();

	void setMinPlayerY(float minY) { m_minPlayerY = minY; }

    float getHitboxHalfWidth() const;
    QPointF getShifterPosition() const;

private:
    // Position
    float m_minPlayerY = 15.0f;
    float m_positionX = 0;
    float m_positionY = m_minPlayerY;
    float m_positionZ = 0;

    float m_angle = 0.0f;
    float m_turnSpeed = 2.0f;

    float m_potAccel = 0;
    float m_potSteering = 0;

    float m_hitboxHalfWidth = 0.5f;

    Powertrain m_powertrain;
    VirtualShifter m_shifter;
    PowertrainAudioController m_powertrainAudioController;
};

#endif // PLAYER_H
