#include "racemanager.h"

RaceManager::RaceManager(QObject *parent)
    : QObject{parent}
{
    QList<Score> scores = getTopThree();

    for (int i = 0; i < scores.size(); ++i) {
        qInfo() << QString("%1. %2: %3s")
            .arg(i + 1)
            .arg(scores[i].nom)
            .arg(scores[i].temps, 0, 'f', 2);
    }
}

void RaceManager::update(float positionZ, bool isCrashed, float deltaTime)
{
    if(m_state != RaceState::RACING) return;

    switch (m_state) {
    case RaceState::WAITING:
        break;

    case RaceState::RACING:
        if (isCrashed) {
            m_state = RaceState::CRASHED;
            return;
        }

        m_elapsedTime += deltaTime;

        if (positionZ >= m_finishLineZ) {
            m_state = RaceState::FINISHED;
            m_finalTime = m_elapsedTime;
            saveResult("William");
        }
        break;

    case RaceState::CRASHED:
    case RaceState::FINISHED:
        break;
    }
}

RaceState RaceManager::getState() const
{
    return m_state;
}

void RaceManager::startRace()
{
    m_elapsedTime = 0;
    m_finalTime = 0;
    m_state = RaceState::RACING;
}

void RaceManager::saveResult(QString nom) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(path);
    QFile file(path + "/results.csv");

    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        out << nom << ","
            << QString::number(m_elapsedTime, 'f', 3) << ","
            << currentTime << "\n";

        file.close();
    }
}

QList<Score> RaceManager::getTopThree() {
    QList<Score> allScores;

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/results.csv";
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return allScores;
    }

    // Lis ligne par ligne
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        // Split les données
        QStringList parts = line.split(",");

        // Crée chaque score
        if (parts.size() >= 2) {
            Score s;
            s.nom = parts[0];
            s.temps = parts[1].toDouble();
            if(parts.size() > 2) s.date = parts[2];
            allScores.append(s);
        }
    }
    file.close();

    // Sort du plus bas au plus haut
    std::sort(allScores.begin(), allScores.end(), [](const Score& a, const Score& b) {
        return a.temps < b.temps;
    });

    // Retourne le top 3
    if (allScores.size() > 3) {
        return allScores.mid(0, 3);
    }

    return allScores;
}
