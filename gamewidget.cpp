#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent) {
    QTimer *timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout, this, &GameWidget::gameLoop);

    timer->start(1000/FRAME_RATE);

    // Load les sprites
    if (!testSprite.load(":/images/test.png")) {
        qWarning() << "Échec à ouvrir le sprite";
    }

    // Génère la map
    for (int i = 0; i < N_LINES; i++) {
        Line line;
        line.z = i * SEG_L;

        if(i>50 && i<700) line.curve=0.02;
        line.nbLane = 3;
        if(i>0 && i < 200) line.isLineFull = true;
        if(i > 100 && i < 700) line.y = sin(i / 30.0) * 150;

        if(i%100==0) { line.spriteX=-2.5; line.sprite=testSprite; }
        /*if(i>100 && i<700) line.curve=0.5;
        if(i > 1100) line.curve = -0.7;
        if(i>0 && i < 200) line.isLineFull = true;

        if(i%100==0) { line.spriteX=-2.5; line.sprite=testSprite; }

        line.nbLane = 3;
        if(i > 100 && i < 700) line.y = sin(i / 30.0) * 1500;
        if(i > 400 && i < 700) line.y = sin(i / 90.0) * 15000;*/

        lines.push_back(line);
    }

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

    if (N_LINES > 0) {
        int currentIndex = startPos % N_LINES;
        int nextIndex = (startPos + 1) % N_LINES;

        currentCurve = lines[currentIndex].curve;

        float currentY = lines[currentIndex].y;
        float nextY = lines[nextIndex].y;
        currentSlopeDelta = nextY - currentY;
    }

    player.tick(dt, currentCurve, currentSlopeDelta);

    update();
}

void GameWidget::drawQuad(QPainter &painter, QColor color, int x1, int y1, int w1, int x2, int y2, int w2){
    // Crée les quatre points
    QPoint points[4] = {
        QPoint(x1 - w1, y1),
        QPoint(x2 - w2, y2),
        QPoint(x2 + w2, y2),
        QPoint(x1 + w1, y1)
    };

    // Dessine
    painter.setBrush(color);
    painter.drawConvexPolygon(points, 4);
}

void GameWidget::paintEvent(QPaintEvent *event){
    QPainter painter(this);

    // Ciel
    painter.fillRect(rect(), sunset);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QRect(150, 300, 100, 100));

    static const QColor GRASS_LIGHT(16, 200, 16);
    static const QColor GRASS_DARK(0, 154, 0);
    static const QColor RUMBLE_LIGHT(143, 143, 143);
    static const QColor RUMBLE_DARK(121, 121, 121);
    static const QColor ROAD_LIGHT(107, 107, 107);
    static const QColor ROAD_DARK(105, 105, 105);
    static const QColor LINE(255, 204, 0);

    int startPos = player.getPositionZ()/SEG_L;
    float percent = (player.getPositionZ() - (startPos * SEG_L)) / (float)SEG_L;

    Line currentLine = lines[startPos % N_LINES];
    float currentY = currentLine.y;
    float nextY = lines[(startPos + 1) % N_LINES].y;

    float rawSlope = nextY - currentY;
    float camHeight = player.getPositionY() + (currentY + rawSlope * percent);

    int lookahead = 15;
    float lookAheadY = lines[(startPos + lookahead) % N_LINES].y;

    float actualSlope = (lookAheadY - currentY) / (lookahead * SEG_L);

    float pitchStrength = 1.0f;
    float targetPitch = actualSlope * pitchStrength;

    player.pitch += (targetPitch - player.pitch) * 0.1f;

    float x = 0;
    float dx = -(currentLine.curve * percent);
    int maxy = height();
    float cameraAngle = player.getAngle();

    // Montre les 600 lignes devant
    for(int n=startPos; n<startPos+600;n++){
        Line& l = lines[n%N_LINES];

        l.project(player.getPositionX()*ROAD_W-x, camHeight, player.getPositionZ() - ((n>=N_LINES)?N_LINES*SEG_L:0), width(), height(), cameraAngle, player.pitch);

        x+=dx;
        dx+=l.curve;

        l.clip = maxy;
        if (l.Y>maxy) continue;
        maxy = l.Y;

        if (maxy <= 0) break;

        Line p;
        if (n == startPos) {
            // Extrapole la ligne
            p = l;
            p.Y = height();
            p.W = l.W * 1.5f;
            p.X = (width() / 2.0f) + ((l.X - (width() / 2.0f)) * 1.5f);
        } else {
            p = lines[(n - 1 + N_LINES) % N_LINES];
        }

        bool isDark = (n / 3) % 2;
        const QColor &grass = isDark ? GRASS_DARK : GRASS_LIGHT;
        const QColor &rumble = isDark ? RUMBLE_DARK : RUMBLE_LIGHT;
        const QColor &road = isDark ? ROAD_DARK : ROAD_LIGHT;

        drawQuad(painter, grass, 0, p.Y, width(), 0, l.Y, width());
        drawQuad(painter, rumble, p.X, p.Y, p.W*1.2, l.X, l.Y, l.W*1.2);
        drawQuad(painter, road, p.X, p.Y, p.W, l.X, l.Y, l.W);

        bool isLine = l.isLineFull || (n / 6) % 2;
        if(isLine){
            float widthLigneP = std::max(1.0f, (float)(p.W * 0.02));
            float widthLigneL = std::max(1.0f, (float)(l.W * 0.02));

            int nbLigne = l.nbLane - 1;
            if(nbLigne == 0) continue;

            float laneWidthP = (p.W * 2) / l.nbLane;
            float laneWidthL = (l.W * 2) / l.nbLane;

            for(int i = 1; i<nbLigne+1; i++){
                float offsetP = -p.W + (laneWidthP * i);
                float offsetL = -l.W + (laneWidthL * i);
                drawQuad(painter, LINE, p.X+offsetP, p.Y, widthLigneP, l.X+offsetL, l.Y, widthLigneL);
            }
        }
    }

    // Affiche les sprites
    for(int n=startPos+600; n>startPos;n--){
        Line& l = lines[n%N_LINES];
        l.drawSprite(painter, width(), height());
    }

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
