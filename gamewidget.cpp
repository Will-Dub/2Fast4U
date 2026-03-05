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

    int startPos = player.getPositionZ() / SEG_L;

    float currentCurve = 0.0f;
    float currentSlopeDelta = 0.0f;
    float terrainFriction = 1.0f;

    if (terrain.getTotalLines() > 0) {
        int currentIndex = startPos % terrain.getTotalLines();
        int nextIndex = (startPos + 1) % terrain.getTotalLines();

        const Line& currentLine = terrain.getLine(currentIndex);
        const Line& nextLine = terrain.getLine(nextIndex);

        currentCurve = currentLine.curve;
        currentSlopeDelta = nextLine.y - currentLine.y;

        currentSlopeDelta = nextLine.y - currentLine.y;

        // Joueur à l'extérieure de la route
        float playerX = player.getPositionX();
        if (std::abs(playerX) > 1.0f * currentLine.nbLane) {
            terrainFriction = 3.5f;
        }
    }

    player.tick(dt, currentCurve, currentSlopeDelta, terrainFriction);

    if (terrain.getTotalLines() > 0) {
        int currentZ = player.getPositionZ();
        int newSegmentIndex = currentZ / SEG_L;
        const Line& collisionLine = terrain.getLine(newSegmentIndex % terrain.getTotalLines());

        // Vérifie si la ligne à un obstacle
        if (!collisionLine.sprite.isNull()) {
            float playerX = player.getPositionX();
            float obstacleX = collisionLine.spriteX;
            float obstacleWidth = 0.4f;
            qInfo() << playerX;
            qInfo() << obstacleX;

            if (std::abs(playerX - obstacleX) < obstacleWidth) {
                player.crash(SEG_L * 1.5f);
            }
        }
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
