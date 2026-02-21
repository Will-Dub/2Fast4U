#include <QCoreApplication>
#include "car.h"
#include "consolereader.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConsoleReader reader;

    // Connect the KeyPressed signal to a slot (using a lambda for simplicity)
    /*QObject::connect(&reader, &ConsoleReader::KeyPressed, [&a](char key){

        // Print the character code and the character itself
        qDebug() << "Key caught:" << key << "(ASCII:" << (int)key << ")";

        // Exit condition: Quit if 'q' or 'Q' is pressed
        if (key == 'q' || key == 'Q') {
            qDebug() << "Quitting application...";
            a.quit();
        }
    });*/

    // Start the thread
    reader.start();

    Car car(1000);
    QElapsedTimer timer;
    float potAccel = 0;
    float potSteering = 0;
    const float ACCEL_SPEED = 10;
    const float STEERING_SPEED = 10;

    /*for(int i =0; i<10000;i++){
        car.tick(dt, 1);
        currentTime += dt;

        qInfo() << "Position: " << car.m_position
                << " Velocite: " << car.m_velocite
                << " Accel: " << car.m_acceleration;
    }*/

    while(true){

        float dt = timer.restart() / 1000.0f;

        if (dt > 0.1f) dt = 0.1f;

        //qInfo() << "accel: " << potAccel;
        //qInfo() << "steering: " << potSteering;

        if (reader.isKeyPressed('w')) {
            potAccel += ACCEL_SPEED * dt;
            if(potAccel > 1){
                potAccel = 1;
            }
        }
        if (reader.isKeyPressed('s')) {
            potAccel -= ACCEL_SPEED * dt;
            if(potAccel < -1){
                potAccel = -1;
            }
        }
        if (reader.isKeyPressed('a')) {
            potSteering -= STEERING_SPEED * dt;
            if(potSteering < -1){
                potSteering = -1;
            }
        }
        if (reader.isKeyPressed('d')) {
            potSteering += STEERING_SPEED * dt;
            if(potSteering > 1){
                potSteering = 1;
            }
        }

        car.tick(dt, potAccel, potSteering);

        QThread::usleep(10000);
    }

    return a.exec();
}









