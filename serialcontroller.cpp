#include "serialcontroller.h"

SerialController::SerialController(const QString &portName, QObject *parent)
    : QObject(parent)
{
    m_serial.setPortName(portName);
    m_serial.setBaudRate(QSerialPort::Baud115200);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);

    connect(&m_serial, &QSerialPort::readyRead, this, &SerialController::handleReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialController::handleError);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        qCritical() << "Critial: Erreur durant l'ouverure du port série:" << portName << m_serial.errorString();
    } else {
        qInfo() << "SUCCESS: Connecter au port série" << portName;
    }
}

SerialController::~SerialController()
{
    if (m_serial.isOpen()) {
        m_serial.close();
    }
}

InputState SerialController::getState()
{
    InputState state;

    state.steering = m_steering;
    state.acceleration = m_acceleration;
    state.brake = m_brake;
    state.clutch = m_clutch;

    state.joystickX = m_joystickX;
    state.joystickY = m_joystickY;
    state.isJoystickBtnPressed = m_isJoystickBtnPressed;

    state.isAccelPressed = m_isAccelerationBtnPressed;
    state.isBrakePressed = m_isBrakeBtnPressed;
    state.isClutchPressed = m_isClutchBtnPressed;

    return state;
}

void SerialController::sendInformation(float dt, int vitesse, int rpm)
{
    m_timeElapsedSinceSend += dt;
    if (m_timeElapsedSinceSend < 0.05f) {
        return;
    }

    m_timeElapsedSinceSend = 0.0f;

    if (!m_serial.isOpen() || !m_serial.isWritable()) {
        return;
    }

    QJsonObject json;
    json["v"] = vitesse;
    json["r"] = rpm;

    // Fais le payload(sans espace et new line)
    QJsonDocument doc(json);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    // Délimite les payload
    payload.append('\n');

    m_serial.write(payload);
}

void SerialController::handleReadyRead()
{
    m_buffer.append(m_serial.readAll());

    // Process tous les packets dans le buffer
    while (m_buffer.contains('\n')) {
        int newlineIndex = m_buffer.indexOf('\n');

        // Extrait le packet au complet
        QByteArray packet = m_buffer.left(newlineIndex).trimmed();

        // Enleve les données
        m_buffer.remove(0, newlineIndex + 1);

        if (!packet.isEmpty()) {
            parsePacket(packet);
        }
    }
}

void SerialController::parsePacket(const QByteArray& packet) {
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(packet, &parseError);

    // Valide le json
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Warning: Json invalide:" << parseError.errorString() << " Offset:" << parseError.offset;
        qWarning() << "Packet raw:" << packet;
        return;
    }

    if (!jsonDoc.isObject()) {
        qWarning() << "INVALID PAYLOAD: JSON is not an object.";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // TODO ajouter check de temp

    if (!jsonObj.contains("type")) {
        qWarning() << "Warning: Le JSON n'a pas de type";
        return;
    }

    PacketType type = PacketType(jsonObj["type"].toInt());

    switch(type){
    case PacketType::STEERING:
        if (jsonObj.contains("steering")) {
            float rawSteering = std::clamp((float)jsonObj["steering"].toDouble(), -1.0f, 1.0f);

            // Applique les deadzones
            if (std::abs(rawSteering) < STEERING_DEADZONE) {
                rawSteering = 0.0f;
            } else {
                float sign = std::copysign(1.0f, rawSteering);
                rawSteering = sign * ((std::abs(rawSteering) - STEERING_DEADZONE) / (1.0f - STEERING_DEADZONE));
            }

            // Low pass filter
            m_steering += (rawSteering - m_steering) * STEERING_SMOOTHING;
        }
        break;
    case PacketType::JOYSTICK:
        // TODO that shit
        break;
    case PacketType::BUTTONS:
        // TODO that shit
        break;
    case PacketType::PEDALES:
        if (jsonObj.contains("g") && jsonObj.contains("b") && jsonObj.contains("c")) {
            m_acceleration = std::clamp((float)jsonObj["g"].toDouble(), 0.0f, 1.0f);
            m_brake = std::clamp((float)jsonObj["b"].toDouble(), 0.0f, 1.0f);
            m_clutch = std::clamp((float)jsonObj["c"].toDouble(), 0.0f, 1.0f);
        }
        break;
    default:
        qWarning() << "WARNING: Type de packet inconnu (" << type << ").";
        break;
    }
}

void SerialController::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        qCritical() << "CRITICAL: Port série déconnecté";
        m_serial.close();
    }
}
