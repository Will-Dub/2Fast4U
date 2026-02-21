#include "car.h"

Car::Car(float masse): m_masse(masse) {}

void Car::tick(float dt, float accel, float currentTurn)
{
    if(currentTurn > 1.0f) currentTurn = 1.0f;
    if(currentTurn < -1.0f) currentTurn = -1.0f;

    float forceAccel = accel * m_forceMax;
    float resistanceAir = m_coefficientDrag * (m_velocite * m_velocite);

    float direction = (m_velocite >= 0) ? 1.0f : -1.0f;
    float resistanceSol = m_friction * m_velocite;

    float forceNet = forceAccel - (resistanceAir * direction) - resistanceSol;

    m_acceleration = forceNet / m_masse;
    m_velocite += m_acceleration * dt;

    if(accel == 0.0f && std::abs(m_velocite) < 0.1f) {
        m_velocite = 0.0f;
    }

    if (std::abs(m_velocite) > 0.1f) {
        float reverseMultiplier = (m_velocite < 0) ? -1.0f : 1.0f;
        m_angle += currentTurn * m_turnSpeed * reverseMultiplier * dt;
    }

    float velX = m_velocite * std::cos(m_angle);
    float velZ = m_velocite * std::sin(m_angle);

    m_positionX += velX * dt;
    m_positionZ += velZ * dt;

    qInfo() << m_positionX;
    qInfo() << m_positionZ;
}
