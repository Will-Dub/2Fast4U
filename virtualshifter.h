#ifndef VIRTUALSHIFTER_H
#define VIRTUALSHIFTER_H

#include <QPointF>
#include <QVector>
#include <QDebug>

struct Rail {
    QPointF startNode;
    QPointF endNode;

    // WTFFFF a voir si ça marche
    // Checks if point P is resting on this rail segment
    bool containsPoint(const QPointF& P) const {
        QPointF AB = endNode - startNode;
        QPointF AP = P - startNode;

        // Calculate the squared length of the rail
        float lengthSquared = QPointF::dotProduct(AB, AB);

        if (lengthSquared == 0.0f) {
            // The rail is just a single dot. Check distance to startNode.
            QPointF diff = P - startNode;
            return QPointF::dotProduct(diff, diff) < 0.0001f;
        }

        // Project vector AP onto AB to find the normalized distance 't' along the rail
        // t = (AP • AB) / (AB • AB)
        float t = QPointF::dotProduct(AP, AB) / lengthSquared;

        // Clamp 't' between 0.0 and 1.0 to ensure we don't check infinitely past the end nodes
        t = std::max(0.0f, std::min(1.0f, t));

        // Find the exact closest point on the segment to our cursor
        QPointF closestPoint = startNode + (AB * t);

        // Calculate the squared distance from the cursor to that closest point
        QPointF distanceVec = P - closestPoint;
        float distanceSquared = QPointF::dotProduct(distanceVec, distanceVec);

        // If the distance is nearly zero, the cursor is on the rail
        // We use 0.0001f as our epsilon squared (meaning a distance of 0.01)
        return distanceSquared < 0.01f;
    }
};

class VirtualShifter
{
public:
    VirtualShifter();
    void updatePosition(float x, float y);
    QPointF getPosition() const;
    QPointF fixToRail(const QPointF& intendedPos, const Rail& activeRail);
    QVector<Rail> getRails() const;

private:
    QVector<Rail> m_rails;
    QPointF m_position;
};

#endif // VIRTUALSHIFTER_H
