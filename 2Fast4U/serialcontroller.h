#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSerialPortInfo>
#include <QElapsedTimer>
#include <QThread>

struct InputState {
    float steering = 0.0f;

    float acceleration = 0.0f;
    float brake = 0.0f;
    float clutch = 0.0f;

    float joystickX = 0;
    float joystickY = 0;
    bool isJoystickBtnPressed = false;

    bool isAccelPressed = false;
    bool isBrakePressed = false;
    bool isClutchPressed = false;

    bool isStarted = false;
};

class SerialController : public QObject
{
    Q_OBJECT
public:
    enum PacketType {
        JOYSTICK = 0,
        BUTTONS = 1,
        PEDALES = 2,
        STEERING = 3,
        STATUS = 4,
        MUON = 5
    };

    explicit SerialController(QObject* parent = nullptr);
    ~SerialController() override;

    InputState getState();
    bool getMuonFlag();
    void sendInformation(float dt, int vitesse, int rpm, bool isStarted=true, bool force=false);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort m_serial;
    QByteArray m_buffer;
    QElapsedTimer m_timer;
    const qint64 SERIAL_INTERVAL_MS = 200;

    float m_steering = 0.0f;

    float m_acceleration = 0.0f;
    float m_brake = 0.0f;
    float m_clutch = 0.0f;

    bool m_isAccelerationBtnPressed = false;
    bool m_isBrakeBtnPressed = false;
    bool m_isClutchBtnPressed = false;

    float m_joystickX = 0.0f;
    float m_joystickY = 0.0f;
    bool m_isJoystickBtnPressed = false;

    bool m_muonFlag = false;

    float m_timeElapsedSinceSend = 0.0f;
    bool m_isStarted = false;

    static constexpr float STEERING_DEADZONE = 0.05f;
    static constexpr float STEERING_SMOOTHING = 0.3f;

    void parsePacket(const QByteArray& packet);
    float normaliseAdc(int valeurAdc);
};

#endif // SERIALCONTROLLER_H
