#include "player.h"

Player::Player() {}

const float ACCEL_SPEED = 2.0f;
const float STEERING_SPEED = 1.5f;
const float FORCE_CENTRIFUGE = 1.2f;
const float BRAKE_MULTIPLIER = 2.5f;
const float GRAVITY = 9.8f;

void Player::tick(float dt, float currentCurve, float currentSlope, float terrainFriction, const InputState& input)
{
    // TODO: faire currentSlope

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

    // Frène plus vite
    float currentForceMax = (m_velocite > 1.0f && ACCEL_INPUT < 0.0f)
                                ? m_forceMax * BRAKE_MULTIPLIER
                                : m_forceMax;

    float forceEngine = ACCEL_INPUT * currentForceMax;

    float totalFriction = m_friction * terrainFriction;
    float resistanceFriction = totalFriction * m_velocite;
    float resistanceAir      = m_coefficientDrag * (m_velocite * m_velocite);

    float angleRad = std::atan(currentSlope / SEG_L);
    float forceGravity = m_masse * GRAVITY * std::sin(angleRad);

    float forceNet = forceEngine - resistanceAir - resistanceFriction - forceGravity;

    m_acceleration = forceNet / m_masse;
    m_velocite += m_acceleration * dt;

    // Empeche de reculer pour l'instant
    if (m_velocite < 0.0f) {
        m_velocite = 0.0f;
        m_acceleration = 0.0f;
    }

    if(ACCEL_INPUT == 0.0f && std::abs(m_velocite) < 0.1f) {
        m_velocite = 0.0f;
        m_acceleration = 0.0f;
    }

    // Avance
    m_positionZ += m_velocite * dt;

    // Déplace gauche/droite selon direction et vitesse
    if (std::abs(m_velocite) > 0.1f) {
        float reverseMultiplier = (m_velocite < 0) ? -1.0f : 1.0f;

        m_positionX += STEERING_INPUT * m_velocite * m_turnSpeed * reverseMultiplier * 0.08f * dt;
    }

    // Force centrifuge lors des tournages
    float forceCentrifuge = currentCurve * m_velocite * FORCE_CENTRIFUGE;
    m_positionX -= forceCentrifuge * dt;

    float targetAngle = (STEERING_INPUT * m_velocite * STEERING_LEAN_RATIO) +
                        (currentCurve * m_velocite * CURVE_LEAN_RATIO);
    m_angle += (targetAngle - m_angle) * CHASSIS_ROLL_STIFFNESS * dt;
}

void Player::crash(float distancePushBack)
{
    m_velocite = 0.0f;
    m_acceleration = 0.0f;

    m_positionZ -= distancePushBack;
}

float Player::getHitboxHalfWidth() const
{
    return m_hitboxHalfWidth;
}
