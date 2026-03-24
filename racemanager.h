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
    WAITING,
    RACING,
    CRASHED,
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
    RaceState m_state = RaceState::WAITING;
    double m_finishLineX = 5000.0;

    double m_elapsedTime = 0.0;
    double m_finalTime = 0.0;
public:
    explicit RaceManager(QObject *parent = nullptr);

    void update(float positionX, bool isCrashed, float deltaTime);

    RaceState getState() const;
    double getElapsedTime() const { return m_elapsedTime; }
    double getFinalTime() const { return m_finalTime; }

    void startRace();
    QList<Score> getTopThree();

private:
    void saveResult(QString nom);

signals:
    void raceFinished(double time);
    void raceCrashed();
};

#endif // RACEMANAGER_H
