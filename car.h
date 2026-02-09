#ifndef CAR_H
#define CAR_H

class Car
{
public:
    Car(float masse);

    /**
     * @brief tick met à jour la position par rapport au dt et l'accel
     * @param dt temp passé
     * @param accel throttle de 0-1
     */
    void tick(float dt, float accel);

public:
    float m_position = 0;
    float m_velocite = 0;
    float m_acceleration = 0;
    float m_masse = 10;

    float m_coefficientDrag = 0.05;
    float m_friction = 0.5;
    float m_forceMax = 500;
};

#endif // CAR_H
