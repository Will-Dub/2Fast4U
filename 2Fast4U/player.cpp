#include "player.h"

Player::Player(): m_powertrainAudioController(&m_powertrain) {
    m_powertrainAudioController.initSounds();
    m_powertrainAudioController.start();
    m_powertrain.setStarted(true);
}

const float ACCEL_SPEED = 0.05f;
const float STEERING_SPEED = 1.5f;
const float FORCE_CENTRIFUGE = 1.2f;
const float BRAKE_MULTIPLIER = 2.5f;
const float GRAVITY = 9.8f;

void Player::tick(float dt, float currentCurve, float currentSlopeAngle, float terrainFriction, const InputState& input)
{
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

    float potAccelBrake = m_potAccel < 0.0f ? -1*m_potAccel : 0;
    float potAccelAccel = m_potAccel > 0.0f ? m_potAccel : 0;

    // Met à jour le shifter
    if(input.clutch >= 0.95){
        m_shifter.updatePosition(input.joystickX, input.joystickY);
    }

    // Met à jour le gear
    if(key1){
        m_shifter.setGear(Node::GEAR_1);
    }else if(key2){
        m_shifter.setGear(Node::GEAR_2);
    }else if(key3){
        m_shifter.setGear(Node::GEAR_3);
    }else if(key4){
        m_shifter.setGear(Node::GEAR_4);
    }else if(key5){
        m_shifter.setGear(Node::GEAR_5);
    }else if(key6){
        m_shifter.setGear(Node::GEAR_6);
    }else if(keyN){
        m_shifter.setGear(Node::NEUTRAL_CENTER);
    }
    m_powertrain.setGear(m_shifter.getGear());

    float inputAcceleration = input.clutch>=0.95 ? 0.0f : input.acceleration*100;

    m_powertrain.setStarted(input.isStarted);
    m_powertrain.everyRefresh(inputAcceleration, input.brake*100, currentSlopeAngle);

    float velocite = m_powertrain.getSpeed() / 3.6f;

    if(qIsNaN(velocite)){
        return;
    }

    m_positionZ += velocite * dt;

    if (std::abs(velocite) > 0.1f) {
        float reverseMultiplier = (velocite < 0) ? -1.0f : 1.0f;
        m_positionX += STEERING_INPUT * velocite * m_turnSpeed * reverseMultiplier * 0.08f * dt;
    }

    float forceCentrifuge = currentCurve * velocite * FORCE_CENTRIFUGE;
    m_positionX -= forceCentrifuge * dt;

    float targetAngle = (STEERING_INPUT * velocite * STEERING_LEAN_RATIO) +
                        (currentCurve * velocite * CURVE_LEAN_RATIO);
    m_angle += (targetAngle - m_angle) * CHASSIS_ROLL_STIFFNESS * dt;

    // TODO enlever quand on veux ce faire niquer les oreilles
    m_powertrainAudioController.update();
}

int Player::getSpeed()
{
    return qIsNaN(m_powertrain.getSpeed()) ? 0 : m_powertrain.getSpeed();
}

int Player::getRevs()
{
    return m_powertrain.getRevs();
}

void Player::stop()
{
    m_powertrain.setSpeed(0);
    m_powertrain.setAcceleration(0);
    m_powertrainAudioController.stop();
    m_powertrainAudioController.handleCatastrophicFailure();
}

void Player::restart()
{
    m_powertrainAudioController.stop();
    m_powertrain.reset();
    m_positionX=0;
    m_positionZ=0;
    m_positionY=MIN_PLAYER_Y;
    m_shifter.resetPosition();
    m_powertrainAudioController.start();
}

void Player::renderHabitacle(QPainter *painter, RaceState raceState, int screenWidth, int screenHeight)
{
    QPixmap sprite;

    if(raceState == RaceState::RACING || raceState == RaceState::MOTOR_EXPLODED){
        sprite = SpriteManager::get("habitacle");
    }else if(raceState == RaceState::CRASHED){
        sprite = SpriteManager::get("habitacleCrashed");
    }else{
        return;
    }

    painter->drawPixmap(
        QRect(0, 0, screenWidth, screenHeight),
        sprite,
        sprite.rect()
    );
}

void Player::allumer()
{
    m_powertrain.setStarted(true);
}

void Player::eteindre()
{
    m_powertrain.setStarted(false);
}

float Player::getHitboxHalfWidth() const
{
    return m_hitboxHalfWidth;
}

QPointF Player::getShifterPosition() const
{
    return m_shifter.getPosition();
}
