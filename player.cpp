#include "player.h"

Player::Player() {}

const float ACCEL_SPEED = 2.0f;
const float STEERING_SPEED = 1.5f;
const float FORCE_CENTRIFUGE = 1.2f;
const float BRAKE_MULTIPLIER = 2.5f;
const float GRAVITY = 9.8f;

void Player::tick(float dt, float currentCurve, float currentSlope, float terrainFriction, const InputState& input)
{
    // Key event
    if (keyUp) {
        m_potAccel += ACCEL_SPEED * dt;
    }
    else if (keyDown) {
        m_potAccel -= ACCEL_SPEED * dt;
    } else{
        m_potAccel = 0;
    }

    if (keyLeft) {
        m_potSteering -= STEERING_SPEED * dt;
    }
    else if (keyRight) {
        m_potSteering += STEERING_SPEED * dt;
    } else {
        // Retoune au centre quand pas appuyé
        if (m_potSteering > 0.0f) {
            m_potSteering = std::max(0.0f, m_potSteering - (STEERING_SPEED * dt));
        } else if (m_potSteering < 0.0f) {
            m_potSteering = std::min(0.0f, m_potSteering + (STEERING_SPEED * dt));
        }
    }

    if(m_potAccel > 1.0f) m_potAccel = 1.0f;
    if(m_potAccel < -1.0f) m_potAccel = -1.0f;
    if(m_potSteering > 1.0f) m_potSteering = 1.0f;
    if(m_potSteering < -1.0f) m_potSteering = -1.0f;

    float m_potSteeringFromInput = input.brake != 0 ? -input.brake : input.acceleration;

    // Met à jour le shifter
    m_shifter.updatePosition(input.joystickX, input.joystickY);

    // Met à jour le gear
    m_powertrain.setGear(m_shifter.getGear());

    m_powertrain.everyRefresh(ACCEL_INPUT*100);

    float velocite = m_powertrain.getSpeed() / 3.6f;

    if(qIsNaN(velocite)){
        return;
    }

    m_positionZ += velocite * dt;

    if (std::abs(velocite) > 0.1f) {
        float reverseMultiplier = (velocite < 0) ? -1.0f : 1.0f;
        m_positionX += STEERING_INPUT * velocite * m_turnSpeed * reverseMultiplier * 0.08f * dt;
    }

    float forceCentrifuge = currentCurve * velocite * FORCE_CENTRIFUGE;
    m_positionX -= forceCentrifuge * dt;

    float targetAngle = (STEERING_INPUT * velocite * STEERING_LEAN_RATIO) +
                        (currentCurve * velocite * CURVE_LEAN_RATIO);
    m_angle += (targetAngle - m_angle) * CHASSIS_ROLL_STIFFNESS * dt;
}

int Player::getSpeed()
{
    return qIsNaN(m_powertrain.getSpeed()) ? 0 : m_powertrain.getSpeed();
}

int Player::getRevs()
{
    return m_powertrain.getRevs();
}

void Player::crash(float distancePushBack)
{
    m_powertrain.setSpeed(0);
    m_powertrain.setAcceleration(0);

    m_positionZ -= distancePushBack;
}

float Player::getHitboxHalfWidth() const
{
    return m_hitboxHalfWidth;
}

QPointF Player::getShifterPosition() const
{
    return m_shifter.getPosition();
}
