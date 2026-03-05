#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) {
    QTimer *timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout, this, &GameWidget::gameLoop);

    timer->start(1000/FRAME_RATE);

    // Génère la map
    terrain.generateTerrain();

    setFocusPolicy(Qt::StrongFocus);

    // Crée le background
    sunset = QLinearGradient(0, 0, 0, height());
    sunset.setColorAt(0.0, QColor(20, 20, 110));
    sunset.setColorAt(0.5, QColor(150, 50, 150));
    sunset.setColorAt(1.0, QColor(255, 170, 0));
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left)  player.keyLeft = true;
    if (event->key() == Qt::Key_Right) player.keyRight = true;
    if (event->key() == Qt::Key_Up) player.keyUp = true;
    if (event->key() == Qt::Key_Down) player.keyDown = true;
    if (event->key() == Qt::Key_Space) player.keySpace = true;
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left)  player.keyLeft = false;
    if (event->key() == Qt::Key_Right) player.keyRight = false;
    if (event->key() == Qt::Key_Up) player.keyUp = false;
    if (event->key() == Qt::Key_Down) player.keyDown = false;
    if (event->key() == Qt::Key_Space) player.keySpace = false;
}

void GameWidget::gameLoop(){
    float dt = timer.restart() / 1000.0f;

    // Max 33 ms entre les calculs physique
    if (dt > 0.033f) dt = 0.033f;

    float previousZ = player.getPositionZ();
    int startSegment = std::max(0, static_cast<int>(previousZ / SEG_L));

    float currentCurve = 0.0f;
    float currentSlopeDelta = 0.0f;
    float terrainFriction = 1.0f;

    // Trouve la friction, curve et le slope du terrain
    if (terrain.getTotalLines() > 0) {
        int currentIndex = startSegment % terrain.getTotalLines();
        int nextIndex = (startSegment + 1) % terrain.getTotalLines();

        const Line& currentLine = terrain.getLine(currentIndex);
        const Line& nextLine = terrain.getLine(nextIndex);

        currentCurve = currentLine.curve;
        currentSlopeDelta = nextLine.y - currentLine.y;

        // Joueur à l'extérieure de la route
        float playerX = player.getPositionX();
        if (std::abs(playerX) > 1.0f * currentLine.nbLane) {
            terrainFriction = 3.5f;
        }
    }

    player.tick(dt, currentCurve, currentSlopeDelta, terrainFriction);

    // CCD
    float currentZ = player.getPositionZ();
    int endSegment = std::max(0, static_cast<int>(currentZ / SEG_L));

    float playerHitboxWidth = player.getHitboxWidth();
    float playerHalfWidth = playerHitboxWidth / 2.0f;
    float finalPlayerX = player.getPositionX();
    bool isCrashed = false;

    // Loop a travers tous les segment traversés
    for (int i = startSegment; i <= endSegment; ++i) {
        const Line& collisionLine = terrain.getLine(i % terrain.getTotalLines());

        if (!collisionLine.obstacles.isEmpty()) {
            for (const Obstacle& obstacle : collisionLine.obstacles) {
                float obstacleX = obstacle.getSpriteX();

                float obstacleHalfWidth = obstacle.getHitboxWidth() / 2.0f;
                float minimumSafeDistance = playerHalfWidth + obstacleHalfWidth;

                // Si leur distance du milieu est plus petit que leur demi combiner
                if (std::abs(finalPlayerX - obstacleX) < minimumSafeDistance) {
                    player.crash(SEG_L * 1.5f);
                    isCrashed = true;
                    break;
                }
            }
        }

        if (isCrashed) break;
    }

    update();
}

void GameWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    // Ciel
    painter.fillRect(rect(), sunset);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRect(150, 300, 100, 100));

    // Terrain
    terrain.render(painter, player, width(), height());

    // Calcul des fps
    frameCount++;
    if (!fpsTimer.isValid()) fpsTimer.start();
    if(fpsTimer.elapsed() > 1000){
        int fps = frameCount / (fpsTimer.elapsed() / 1000.0);
        currentFps = fps;

        frameCount = 0;
        fpsTimer.restart();
    }

    QTextDocument doc;
    doc.setHtml(QString("<font color=\"#f00\">%1</font>").arg(currentFps));
    doc.drawContents(&painter);

    //painter.drawEllipse(QRect(width()/2-30, height()/2-30, width()/2+30, height()/2+30));
}
