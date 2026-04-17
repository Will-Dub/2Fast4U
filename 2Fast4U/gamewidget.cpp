/*==================================================
* NOM: gamewidget.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans gamewidget.h
===================================================*/
#include "gamewidget.h"
#include <algorithm>
#include <cmath>
#include <QPainterPath>

GameWidget::GameWidget(QWidget* parent) : m_serialController(), QWidget(parent) {
    m_loopTimer.setTimerType(Qt::PreciseTimer);

    connect(&m_loopTimer, &QTimer::timeout, this, &GameWidget::gameLoop);

    m_loopTimer.start(1000 / FRAME_RATE);
    m_timer.start();

    // Génère la map
    m_terrain.generateTerrain();

    setFocusPolicy(Qt::StrongFocus);
    reloadSettings();
}

void GameWidget::setNom(const QString& nom)
{
    m_raceManager.setNom(nom);
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left)  m_player.keyLeft = true;
    if (event->key() == Qt::Key_Right) m_player.keyRight = true;
    if (event->key() == Qt::Key_Up) m_player.keyUp = true;
    if (event->key() == Qt::Key_Down) m_player.keyDown = true;
    if (event->key() == Qt::Key_Space) m_player.keySpace = true;
    if (event->key() == Qt::Key_1) m_player.key1 = true;
    if (event->key() == Qt::Key_2) m_player.key2 = true;
    if (event->key() == Qt::Key_3) m_player.key3 = true;
    if (event->key() == Qt::Key_4) m_player.key4 = true;
    if (event->key() == Qt::Key_5) m_player.key5 = true;
    if (event->key() == Qt::Key_6) m_player.key6 = true;
    if (event->key() == Qt::Key_N) m_player.keyN = true;
    if (event->key() == Qt::Key_Escape) m_isEscapePressed = true;
}

void GameWidget::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left)  m_player.keyLeft = false;
    if (event->key() == Qt::Key_Right) m_player.keyRight = false;
    if (event->key() == Qt::Key_Up) m_player.keyUp = false;
    if (event->key() == Qt::Key_Down) m_player.keyDown = false;
    if (event->key() == Qt::Key_Space) m_player.keySpace = false;
    if (event->key() == Qt::Key_1) m_player.key1 = false;
    if (event->key() == Qt::Key_2) m_player.key2 = false;
    if (event->key() == Qt::Key_3) m_player.key3 = false;
    if (event->key() == Qt::Key_4) m_player.key4 = false;
    if (event->key() == Qt::Key_5) m_player.key5 = false;
    if (event->key() == Qt::Key_6) m_player.key6 = false;
    if (event->key() == Qt::Key_N) m_player.keyN = false;
    if (event->key() == Qt::Key_Escape) m_isEscapePressed = false;
}

void GameWidget::resumeGame()
{
    m_raceManager.resumeRace();
    m_player.resume();
}

void GameWidget::pauseGame()
{
    m_raceManager.pauseRace();
    m_player.pause();
}

void GameWidget::reloadSettings()
{
    m_terrain.setMaxDrawDistance(m_settings.value("distance_affichage", 600).toInt());
    m_player.setMinPlayerY(m_settings.value("hauteur_camera", 15.0f).toFloat());
}

void GameWidget::gameLoop() {
    if (m_raceManager.getState() != RaceState::RACING) return;

    if (m_isEscapePressed) {
        m_isEscapePressed = false;
        emit pausePressed();
        return;
    }

    float dt = m_timer.restart() / 1000.0f;

    // --- Fixe les limite de dt ---
    if (dt > 0.033f) dt = 0.033f;
    if (dt < 0.001f) dt = 0.016f;

    float previousZ = m_player.getPositionZ();
    int startSegment = std::max(0, static_cast<int>(previousZ / SEG_L));

    float currentCurve = 0.0f;
    float currentSlopeAngle = 0.0f;
    float terrainFriction = 1.0f;

    // --- Trouve la friction, curve et slope du terrain ---
    if (m_terrain.getTotalLines() > 0) {
        int currentIndex = startSegment % m_terrain.getTotalLines();
        int nextIndex = (startSegment + 1) % m_terrain.getTotalLines();

        const Line& currentLine = m_terrain.getLine(currentIndex);
        const Line& nextLine = m_terrain.getLine(nextIndex);

        currentCurve = currentLine.curve;
        float currentSlopeDelta = nextLine.y - currentLine.y;
        currentSlopeAngle = qAtan(currentSlopeDelta / SEG_L) * 180.0f / 3.14159265358979323846f;

        // Joueur à l'extérieure de la route
        float playerX = m_player.getPositionX();
        if (std::abs(playerX) > 1.0f * currentLine.nbLane) {
            terrainFriction = 35.0f;
        }
    }

    InputState input = m_serialController.getState();
    m_player.tick(dt, currentCurve, currentSlopeAngle, terrainFriction, input);

    // --- CCD (Collisions) ---
    float currentZ = m_player.getPositionZ();
    int endSegment = std::max(0, static_cast<int>(currentZ / SEG_L));

    float playerHalfWidth = m_player.getHitboxHalfWidth();
    float finalPlayerX = m_player.getPositionX();
    bool isCrashed = false;

    // Loop a travers tous les segment traversés
    for (int i = startSegment; i <= endSegment; ++i) {
        const Line& collisionLine = m_terrain.getLine(i % m_terrain.getTotalLines());

        if (!collisionLine.obstacles.isEmpty()) {
            for (const Obstacle& obstacle : collisionLine.obstacles) {
                float obstacleX = obstacle.getSpriteX();

                float obstacleHalfWidth = obstacle.getHitboxWidth() / 2.0f;
                float minimumSafeDistance = playerHalfWidth + obstacleHalfWidth;

                // Si leur distance du milieu est plus petit que leur demi combiner
                if (std::abs(finalPlayerX - obstacleX) < minimumSafeDistance && obstacle.getHitboxWidth() > 0) {
                    isCrashed = true;
                    break;
                }
            }
        }

        if (isCrashed) break;
    }

    // --- Manage la race et le temps ---
    m_raceManager.update(m_player.getPositionZ(), isCrashed, m_player.getIsMotorExploded(), dt);

    // Vérifie si le joueur a perdu ou gagné
    if (m_raceManager.getState() != RaceState::RACING) {
        if (m_raceManager.getState() == RaceState::FINISHED) {
            m_player.pause();
        }
        else {
            m_player.crash();
        }
        QTimer::singleShot(2000, [=]() {
            EndType endType;

            switch (m_raceManager.getState()) {
            case RaceState::CRASHED:
                endType = EndType::Crash;
                break;
            case RaceState::MOTOR_EXPLODED:
                endType = EndType::MotorExploded;
                break;
            case RaceState::FINISHED:
                endType = EndType::Win;
                break;
            default:
                endType = EndType::Crash;
            }

            emit endGame(endType, m_raceManager.getNom(), m_raceManager.getFinalTime());
            });
        m_serialController.sendInformation(dt, 0, 0, false, true);
        update();
        return;
    }

    // --- Bouge les obstacles du terrain ---
    m_terrain.tick(m_player, dt);

    // --- Ajoute un obstacle si les muon le veulent ---
    if (m_serialController.getMuonFlag()) {
        if (rand() % 6 == 1) {
            m_terrain.generateRandomObstacle(m_player);
        }
    }

    // --- Envoie les données au arduino ---
    m_serialController.sendInformation(dt, m_player.getSpeed(), m_player.getRevs(), m_player.getIsMotorStarted());

    // --- Appel update de QWidget ---
    update();
}

void GameWidget::restartGame()
{
    m_raceManager.restartRace();
    m_player.restart();
}

void GameWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // --- Ciel ---
    QLinearGradient skyGradient(0, 0, 0, height() * 0.68f);
    skyGradient.setColorAt(0.0, QColor(20, 64, 150));
    skyGradient.setColorAt(0.45, QColor(94, 164, 218));
    skyGradient.setColorAt(1.0, QColor(177, 219, 238));
    painter.fillRect(rect(), skyGradient);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    const float pitchHorizonOffset = std::clamp(m_player.pitch, -0.16f, 0.16f) * height() * 0.5f;
    const float horizonY = height() * 0.48f + pitchHorizonOffset;
    const float parallax = std::fmod(m_player.getPositionZ() * 0.012f, 480.0f);

    painter.setPen(Qt::NoPen);

    QLinearGradient meadowGradient(0, horizonY, 0, height() / 2.0f);
    meadowGradient.setColorAt(0.0, QColor(78, 160, 86, 180));
    meadowGradient.setColorAt(1.0, QColor(34, 124, 78, 210));
    painter.setBrush(meadowGradient);
    painter.drawRect(QRectF(0, horizonY + height() * 0.05f, width(), height() * 0.10f));

    painter.setPen(Qt::NoPen);

    // --- Terrain ---
    m_terrain.render(painter, m_player, width(), height(), static_cast<float>(m_raceManager.getFinishLineZ()));

    // --- Calcul des fps ---
    m_frameCount++;
    if (!m_fpsTimer.isValid()) m_fpsTimer.start();
    if (m_fpsTimer.elapsed() > 1000) {
        int fps = m_frameCount / (m_fpsTimer.elapsed() / 1000.0);
        m_currentFps = fps;

        m_frameCount = 0;
        m_fpsTimer.restart();
    }

    // --- Dessine l'habitacle ---
    m_player.renderHabitacle(&painter, m_raceManager.getState(), width(), height());
    painter.restore();

    // --- Shifter ---
    painter.save();

    // Place le centre
    int hudOffsetX = width() - 92;
    //int hudOffsetY = height() - 80;
    int hudOffsetY = 92;
    QRectF shifterPanel(hudOffsetX - 62, hudOffsetY - 72, 124, 144);
    QLinearGradient shifterGradient(shifterPanel.topLeft(), shifterPanel.bottomRight());
    shifterGradient.setColorAt(0.0, QColor(255, 255, 255, 210));
    shifterGradient.setColorAt(1.0, QColor(180, 195, 220, 180));
    painter.setBrush(shifterGradient);
    painter.setPen(QPen(QColor(255, 255, 255, 170), 2));
    painter.drawRoundedRect(shifterPanel, 18, 18);
    painter.translate(hudOffsetX, hudOffsetY);

    // Écrit le texte
    painter.setPen(QColor(20, 27, 38));
    QFont font = painter.font();
    font.setPixelSize(14);
    font.setBold(true);
    painter.setFont(font);

    painter.drawText(-28, -45, "1");
    painter.drawText(-28, 55, "2");

    painter.drawText(-4, -45, "3");
    painter.drawText(-4, 55, "4");

    painter.drawText(20, -45, "5");
    painter.drawText(20, 55, "6");

    float scaleFactor = 40.0f;
    painter.scale(scaleFactor, -scaleFactor);

    // Fait les rails
    QPen railPen(QColor(32, 40, 58));
    railPen.setWidthF(0.07);
    railPen.setCapStyle(Qt::RoundCap);
    painter.setPen(railPen);

    painter.drawLine(QPointF(-0.6f, 0.0f), QPointF(0.6f, 0.0f)); // N
    painter.drawLine(QPointF(-0.6f, 1.0f), QPointF(-0.6f, -1.0f)); // 1-2
    painter.drawLine(QPointF(0.0f, 1.0f), QPointF(0.0f, -1.0f)); // 3-4
    painter.drawLine(QPointF(0.6f, 1.0f), QPointF(0.6f, -1.0f)); // 5-6

    // Fait le curseur
    QRadialGradient knobGradient(0, 0, 0.28f);
    knobGradient.setColorAt(0.0, QColor(255, 255, 255));
    knobGradient.setColorAt(0.45, QColor(255, 70, 95));
    knobGradient.setColorAt(1.0, QColor(137, 0, 32));
    painter.setBrush(knobGradient);
    painter.setPen(QPen(QColor(255, 255, 255), 0.025));

    QPointF cursor = m_player.getShifterPosition();
    float knobRadius = 0.2f;
    painter.drawEllipse(cursor, knobRadius, knobRadius);
    painter.restore();

    // --- Affiche les fps ---
    painter.save();
    painter.setPen(Qt::red);
    painter.drawText(10, 20, QString("FPS: %1, %2").arg(m_currentFps).arg(m_player.getIsMotorStarted() ? "Allumé" : "Éteint"));
    painter.restore();

    // --- Affiche le temps ---
    painter.save();
    qint64 totalMillis = static_cast<qint64>(m_raceManager.getElapsedTime() * 1000.0);
    QTime t(0, 0, 0);
    t = t.addMSecs(totalMillis);
    QString timeStr = t.toString("mm:ss.zzz");

    QFont timerFont("Arial", 20);
    painter.setFont(timerFont);
    painter.setPen(QColor(255, 0, 0));

    painter.drawText(0, 10, width(), 40, Qt::AlignHCenter | Qt::AlignTop, timeStr);

    painter.restore();
}
