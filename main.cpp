#include <QCoreApplication>
#include "car.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Car car(1000);
    float currentTime = 0;
    float dt = 0.1;

    for(int i =0; i<10000;i++){
        car.tick(dt, 1);
        currentTime += dt;

        qInfo() << "Position: " << car.m_position
                << " Velocite: " << car.m_velocite
                << " Accel: " << car.m_acceleration;
    }

    return a.exec();
}
