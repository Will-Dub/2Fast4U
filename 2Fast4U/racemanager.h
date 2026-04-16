#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>

enum class RaceState {
    PAUSED,
    RACING,
    CRASHED,
    MOTOR_EXPLODED,
    FINISHED
};

struct Score {
    QString nom;
    double temps;
    QString date;
};

class RaceManager : public QObject
{
    Q_OBJECT
        RaceState m_state = RaceState::PAUSED;
    QString m_nom = "";
    double m_finishLineZ = 100000.0;

    double m_elapsedTime = 0.0;
    double m_finalTime = 0.0;
public:
    explicit RaceManager(QObject* parent = nullptr);

    void update(float positionX, bool isCrashed, bool isMotorExploded, float deltaTime);

    RaceState getState() const;
    double getElapsedTime() const { return m_elapsedTime; }
    double getFinalTime() const { return m_finalTime; }
    double getFinishLineZ() const { return m_finishLineZ; }
    QString getNom() const { return m_nom; }

    void setNom(const QString& nom);

    void restartRace();
    void resumeRace();
    void pauseRace();
    static QList<Score> getTop();

private:
    void saveResult();
};

#endif // RACEMANAGER_H
