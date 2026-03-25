#include "virtualshifter.h"

VirtualShifter::VirtualShifter() {
    m_currentNode = Node::NEUTRAL_CENTER;
    m_isLocked = false;
}

void VirtualShifter::updatePosition(float x, float y)
{
    // Deadzone pour le reset
    if (std::abs(x) < 0.3f && std::abs(y) < 0.3f) {
        m_isLocked = false;
        return;
    }

    // Check si locked
    if (m_isLocked) return;

    // Trouve la direction
    bool goUp = false, goDown = false, goLeft = false, goRight = false;

    if (std::abs(y) > std::abs(x)) {
        if (y > 0.6f) goUp = true;
        if (y < -0.6f) goDown = true;
    } else {
        if (x > 0.6f) goRight = true;
        if (x < -0.6f) goLeft = true;
    }

    // Change de node et lock
    if (goUp || goDown || goLeft || goRight) {
        executeTransition(goUp, goDown, goLeft, goRight);
        m_isLocked = true;
    }
}

void VirtualShifter::resetPosition()
{
    m_currentNode = Node::NEUTRAL_CENTER;
}

int VirtualShifter::getGear() const
{
    switch (m_currentNode) {
        case Node::GEAR_1: return 1;
        case Node::GEAR_2: return 2;
        case Node::GEAR_3: return 3;
        case Node::GEAR_4: return 4;
        case Node::GEAR_5: return 5;
        case Node::GEAR_6: return 6;
        default: return 0;
    }
}

QPointF VirtualShifter::getPosition() const
{
    switch (m_currentNode) {
        case Node::NEUTRAL_LEFT: return QPointF(-0.6f,  0.0f);
        case Node::NEUTRAL_CENTER: return QPointF( 0.0f,  0.0f);
        case Node::NEUTRAL_RIGHT: return QPointF( 0.6f,  0.0f);
        case Node::GEAR_1: return QPointF(-0.6f,  1.0f);
        case Node::GEAR_2: return QPointF(-0.6f, -1.0f);
        case Node::GEAR_3: return QPointF( 0.0f,  1.0f);
        case Node::GEAR_4: return QPointF( 0.0f, -1.0f);
        case Node::GEAR_5: return QPointF( 0.6f,  1.0f);
        case Node::GEAR_6: return QPointF( 0.6f, -1.0f);
    }
    return QPointF(0.0f, 0.0f);
}

void VirtualShifter::executeTransition(bool goUp, bool goDown, bool goLeft, bool goRight)
{
    // This is the hardcoded metal gate. You define exactly where the cursor is allowed to go.
    switch (m_currentNode) {

    // Barre neutre
    case Node::NEUTRAL_CENTER:
        if (goLeft) m_currentNode = Node::NEUTRAL_LEFT;
        else if (goRight) m_currentNode = Node::NEUTRAL_RIGHT;
        else if (goUp) m_currentNode = Node::GEAR_3;
        else if (goDown) m_currentNode = Node::GEAR_4;
        break;

    case Node::NEUTRAL_LEFT:
        if (goRight) m_currentNode = Node::NEUTRAL_CENTER;
        else if (goUp) m_currentNode = Node::GEAR_1;
        else if (goDown) m_currentNode = Node::GEAR_2;
        break;

    case Node::NEUTRAL_RIGHT:
        if (goLeft) m_currentNode = Node::NEUTRAL_CENTER;
        else if (goUp) m_currentNode = Node::GEAR_5;
        else if (goDown) m_currentNode = Node::GEAR_6;
        break;

    // Chaque gear
    case Node::GEAR_1:
        if (goDown) m_currentNode = Node::NEUTRAL_LEFT;
        break;
    case Node::GEAR_2:
        if (goUp) m_currentNode = Node::NEUTRAL_LEFT;
        break;

    case Node::GEAR_3:
        if (goDown) m_currentNode = Node::NEUTRAL_CENTER;
        break;
    case Node::GEAR_4:
        if (goUp) m_currentNode = Node::NEUTRAL_CENTER;
        break;

    case Node::GEAR_5:
        if (goDown) m_currentNode = Node::NEUTRAL_RIGHT;
        break;
    case Node::GEAR_6:
        if (goUp) m_currentNode = Node::NEUTRAL_RIGHT;
        break;
    }
}

