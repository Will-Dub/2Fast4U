#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent): m_serialController("/dev/ttyACM0") {
    QTimer *loopTimer = new QTimer(this);
    loopTimer->setTimerType(Qt::PreciseTimer);

    connect(loopTimer, &QTimer::timeout, this, &GameWidget::gameLoop);

    loopTimer->start(1000/FRAME_RATE);
    m_timer.start();

    // Génère la map
    m_terrain.generateTerrain();

    setFocusPolicy(Qt::StrongFocus);

    // Crée le background
    m_sunset = QLinearGradient(0, 0, 0, height());
    m_sunset.setColorAt(0.0, QColor(20, 20, 110));
    m_sunset.setColorAt(0.5, QColor(150, 50, 150));
    m_sunset.setColorAt(1.0, QColor(255, 170, 0));
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left)  m_player.keyLeft = true;
    if (event->key() == Qt::Key_Right) m_player.keyRight = true;
    if (event->key() == Qt::Key_Up) m_player.keyUp = true;
    if (event->key() == Qt::Key_Down) m_player.keyDown = true;
    if (event->key() == Qt::Key_Space) m_player.keySpace = true;
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left)  m_player.keyLeft = false;
    if (event->key() == Qt::Key_Right) m_player.keyRight = false;
    if (event->key() == Qt::Key_Up) m_player.keyUp = false;
    if (event->key() == Qt::Key_Down) m_player.keyDown = false;
    if (event->key() == Qt::Key_Space) m_player.keySpace = false;
}

void GameWidget::gameLoop(){
    float dt = m_timer.restart() / 1000.0f;

    // Max et min de dt
    if (dt > 0.033f) dt = 0.033f;
    if (dt < 0.001f) dt = 0.016f;

    float previousZ = m_player.getPositionZ();
    int startSegment = std::max(0, static_cast<int>(previousZ / SEG_L));

    float currentCurve = 0.0f;
    float currentSlopeDelta = 0.0f;
    float terrainFriction = 1.0f;

    // Trouve la friction, curve et slope du terrain
    if (m_terrain.getTotalLines() > 0) {
        int currentIndex = startSegment % m_terrain.getTotalLines();
        int nextIndex = (startSegment + 1) % m_terrain.getTotalLines();

        const Line& currentLine = m_terrain.getLine(currentIndex);
        const Line& nextLine = m_terrain.getLine(nextIndex);

        currentCurve = currentLine.curve;
        currentSlopeDelta = nextLine.y - currentLine.y;

        // Joueur à l'extérieure de la route
        float playerX = m_player.getPositionX();
        if (std::abs(playerX) > 1.0f * currentLine.nbLane) {
            terrainFriction = 3.5f;
        }
    }

    InputState input = m_serialController.getState();
    m_player.tick(dt, currentCurve, currentSlopeDelta, terrainFriction, input);

    // CCD
    float currentZ = m_player.getPositionZ();
    int endSegment = std::max(0, static_cast<int>(currentZ / SEG_L));

    float playerHitboxWidth = m_player.getHitboxWidth();
    float playerHalfWidth = playerHitboxWidth / 2.0f;
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
                if (std::abs(finalPlayerX - obstacleX) < minimumSafeDistance) {
                    m_player.crash(SEG_L * 1.5f);
                    isCrashed = true;
                    break;
                }
            }
        }

        if (isCrashed) break;
    }

    m_terrain.tick(m_player, dt);

    update();
}

void GameWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    // Ciel
    painter.fillRect(rect(), m_sunset);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRect(150, 300, 100, 100));

    // Terrain
    m_terrain.render(painter, m_player, width(), height());

    // Calcul des fps
    m_frameCount++;
    if (!m_fpsTimer.isValid()) m_fpsTimer.start();
    if(m_fpsTimer.elapsed() > 1000){
        int fps = m_frameCount / (m_fpsTimer.elapsed() / 1000.0);
        m_currentFps = fps;

        m_frameCount = 0;
        m_fpsTimer.restart();
    }

    QTextDocument doc;
    doc.setHtml(QString("<font color=\"#f00\">%1</font>").arg(m_currentFps));
    doc.drawContents(&painter);
}
