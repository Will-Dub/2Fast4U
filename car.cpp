#include "car.h"

Car::Car(float masse): m_masse(masse) {}

void Car::tick(float dt, float accel)
{
    float forceAccel = accel * m_forceMax;

    float resistanceAir = m_coefficientDrag * (m_velocite * m_velocite);
    float resistanceSol = m_friction * m_velocite;

    float forceNet = forceAccel - resistanceAir - resistanceSol;

    m_acceleration = forceNet / m_masse;

    m_velocite += m_acceleration * dt;

    if(m_velocite < 0){
        m_velocite = 0;
    }

    m_position += m_velocite * dt;
}
