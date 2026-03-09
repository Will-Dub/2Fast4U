#include "virtualshifter.h"

VirtualShifter::VirtualShifter() {
    m_position = QPointF(0.0f, 0.0f);

    // Fait les points de la ligne du milieu
    QPointF centerLeft(-0.6f, 0.0f);
    QPointF centerRight(0.6f, 0.0f);

    // Points de chaque gear
    QPointF gear1(-0.6f, 1.0f);
    QPointF gear2(-0.6f, -1.0f);
    QPointF gear3(0.0f, 1.0f);
    QPointF gear4(0.0f, -1.0f);
    QPointF gear5(0.6f, 1.0f);
    QPointF gear6(0.6f, -1.0f);

    // Connecte tous les points valides
    // Milieu
    m_rails.push_back({centerLeft, centerRight});

    // 1 et 2
    m_rails.push_back({gear1, gear2});

    // 3 et 4
    m_rails.push_back({gear3, gear4});

    // 5 et 6
    m_rails.push_back({gear5, gear6});
}

void VirtualShifter::updatePosition(float x, float y)
{
    QPointF joystick(x,y);

    if(joystick.manhattanLength() < 0.1f){
        return;
    }

    if (std::abs(joystick.y()) > std::abs(joystick.x())) {
        joystick.setX(0.0f);
    }
    else {
        joystick.setY(0.0f);
    }

    // Trouve les rails qui touches
    QList<Rail> railsValide;
    for(const Rail& rail: m_rails){
        if(rail.containsPoint(m_position)){
            railsValide.append(rail);
        }
    }

    // Essaie tous les mouvement et prend le plus grand
    float vitesse = 0.08f;
    QPointF nouvellePosition = m_position + (joystick * vitesse);

    float meilleurDistance = 0.0f;
    QPointF meilleurPositionFinal = m_position;

    for (const Rail& rail : railsValide) {
        // Force sur la rail
        QPointF testPos = fixToRail(nouvellePosition, rail);

        // Mesure la distance
        QPointF movementDelta = testPos - m_position;
        float distMoved = QPointF::dotProduct(movementDelta, movementDelta);

        // Prend la rail si meilleur
        if (distMoved > meilleurDistance) {
            meilleurDistance = distMoved;
            meilleurPositionFinal = testPos;
        }
    }

    // Applique la gagnante
    if (meilleurDistance > 0.0001f) {
        m_position = meilleurPositionFinal;
    }
}

QPointF VirtualShifter::getPosition() const
{
    return m_position;
}

QPointF VirtualShifter::fixToRail(const QPointF &intendedPos, const Rail &activeRail)
{
    // Trouve les barrières
    float minX = std::min(activeRail.startNode.x(), activeRail.endNode.x());
    float maxX = std::max(activeRail.startNode.x(), activeRail.endNode.x());

    float minY = std::min(activeRail.startNode.y(), activeRail.endNode.y());
    float maxY = std::max(activeRail.startNode.y(), activeRail.endNode.y());

    // Applique les barrières
    float fixedX = std::max(minX, std::min(maxX, static_cast<float>(intendedPos.x())));
    float fixedY = std::max(minY, std::min(maxY, static_cast<float>(intendedPos.y())));

    return QPointF(fixedX, fixedY);
}

QVector<Rail> VirtualShifter::getRails() const
{
    return m_rails;
}
