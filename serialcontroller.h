#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

struct InputState {
    float steering = 0.0f;

    float acceleration = 0.0f;
    float brake = 0.0f;
    float clutch = 0.0f;

    int joystickX = 0;
    int joystickY = 0;
    bool isJoystickBtnPressed = false;

    bool isAccelPressed = false;
    bool isBrakePressed = false;
    bool isClutchPressed = false;
};

class SerialController : public QObject
{
    Q_OBJECT
public:
    enum PacketType {
        JOYSTICK = 0,
        BUTTONS = 1,
        PEDALES = 2,
        STEERING = 3
    };

    explicit SerialController(const QString& portName, QObject* parent = nullptr);
    ~SerialController() override;

    InputState getState();
    void sendInformation(float dt, int vitesse, int rpm);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial;
    QByteArray m_buffer;

    float m_steering = 0.0f;

    float m_acceleration = 0.0f;
    float m_brake = 0.0f;
    float m_clutch = 0.0f;

    bool m_isAccelerationBtnPressed = 0.0f;
    bool m_isBrakeBtnPressed = 0.0f;
    bool m_isClutchBtnPressed = 0.0f;

    int m_joystickX = 0.0f;
    int m_joystickY = 0.0f;
    bool m_isJoystickBtnPressed = false;

    float m_timeElapsedSinceSend = 0.0f;

    static constexpr float STEERING_DEADZONE = 0.05f;
    static constexpr float STEERING_SMOOTHING = 0.3f;

    void parsePacket(const QByteArray& packet);
};

#endif // SERIALCONTROLLER_H
