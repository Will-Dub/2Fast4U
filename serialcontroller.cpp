#include "serialcontroller.h"

SerialController::SerialController(QObject *parent)
    : QObject(parent)
{
    QString portName = "";

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QString port = info.portName().toLower();

        if (port.startsWith("ttys") && !port.contains("usb")) {
            continue;
        }

        if (port.contains("ttyacm") || port.contains("ttyusb") || port.contains("com")) {
            portName = info.portName();
            break;
        }
    }

    if (portName.isEmpty()) {
        qCritical() << "CRITICAL: Aucun port série détecté";
        return;
    }

    m_serial.setPortName(portName);
    m_serial.setBaudRate(QSerialPort::Baud115200);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    connect(&m_serial, &QSerialPort::readyRead, this, &SerialController::handleReadyRead);
    connect(&m_serial, &QSerialPort::errorOccurred, this, &SerialController::handleError);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        qCritical() << "Critial: Erreur durant l'ouverure du port série:" << portName << m_serial.errorString();
    } else {
        m_serial.setDataTerminalReady(true);
        m_serial.setRequestToSend(true);
        qInfo() << "SUCCESS: Connecter au port série" << portName;
    }

    QThread::sleep(1);
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

    state.isStarted = m_isStarted;

    return state;
}

void SerialController::sendInformation(float dt, int vitesse, int rpm, bool isStarted)
{

    if (!m_timer.isValid()) {
        m_timer.start();
    }

    if (!m_timer.hasExpired(SERIAL_INTERVAL_MS)) {
        return;
    }

    m_timer.restart();

    if (!m_serial.isOpen() || !m_serial.isWritable()) {
        return;
    }

    QJsonObject json;
    json["v"] = vitesse;
    json["r"] = rpm;
    json["e"] = isStarted;

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
        if (jsonObj.contains("x") && jsonObj.contains("y") && jsonObj.contains("b")) {
            m_joystickX = normaliseAdc(jsonObj["x"].toInt()) * -1.0f;
            m_joystickY = normaliseAdc(jsonObj["y"].toInt());
            m_isJoystickBtnPressed = jsonObj["b"].toBool();
        }
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
    case PacketType::STATUS:
        if (jsonObj.contains("e")) {
            m_isStarted = jsonObj["e"].toBool();
        }
        break;
    default:
        qWarning() << "WARNING: Type de packet inconnu (" << type << ").";
        break;
    }
}

float SerialController::normaliseAdc(int valeurAdc)
{
    // Deadzone
    if(valeurAdc > 490 && valeurAdc < 530){
        return 0.0f;
    }

    // Met la valeur de 0-1023
    int valeurAdcLimite = std::max(0, std::min(1023, valeurAdc));

    // Divise par 1023 -> 0.0 à 1.0, multiplie par 2 et enleve 1. Donc de -1 à 1
    float normalise = ((float)valeurAdcLimite/1023.0f) * 2.0f - 1.0f;

    return normalise;
}

void SerialController::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        qCritical() << "CRITICAL: Port série déconnecté";
        m_serial.close();
    }
}
