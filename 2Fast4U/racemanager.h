#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>

enum class RaceState{
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
    double m_finishLineZ = 515.0;

    double m_elapsedTime = 0.0;
    double m_finalTime = 0.0;
public:
    explicit RaceManager(QObject *parent = nullptr);

    void update(float positionX, bool isCrashed, bool isMotorExploded, float deltaTime);

    RaceState getState() const;
    double getElapsedTime() const { return m_elapsedTime; }
    double getFinalTime() const { return m_finalTime; }

    void restartRace();
    void resumeRace();
    void pauseRace();
    QList<Score> getTopThree();

private:
    void saveResult(QString nom);
};

#endif // RACEMANAGER_H
