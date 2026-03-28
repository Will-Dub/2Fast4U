#ifndef VIRTUALSHIFTER_H
#define VIRTUALSHIFTER_H

#include <QPointF>
#include <QVector>
#include <QDebug>

enum class Node {
    NEUTRAL_LEFT,
    NEUTRAL_CENTER,
    NEUTRAL_RIGHT,
    GEAR_1,
    GEAR_2,
    GEAR_3,
    GEAR_4,
    GEAR_5,
    GEAR_6,
};

class VirtualShifter
{
public:
    VirtualShifter();
    void updatePosition(float x, float y);
    void resetPosition();
    void setGear(Node newNode);

    int getGear() const;
    QPointF getPosition() const;

private:
    Node m_currentNode;
    bool m_isLocked;

    void executeTransition(bool goUp, bool goDown, bool goLeft, bool goRight);
};

#endif // VIRTUALSHIFTER_H
