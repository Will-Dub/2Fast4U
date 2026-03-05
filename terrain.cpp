#include "terrain.h"

Terrain::Terrain() {}

void Terrain::render(QPainter &painter, Player &player, int screenWidth, int screenHeight)
{
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
    int maxy = screenHeight;
    float cameraAngle = player.getAngle();

    // Montre les 600 lignes devant
    for(int n=startPos; n<startPos+600;n++){
        Line& l = lines[n%N_LINES];

        l.project(player.getPositionX()*ROAD_W-x, camHeight, player.getPositionZ() - ((n>=N_LINES)?N_LINES*SEG_L:0), screenWidth, screenHeight, cameraAngle, player.pitch);

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
            p.Y = screenHeight;
            p.W = l.W * 1.5f;
            p.X = (screenWidth / 2.0f) + ((l.X - (screenWidth / 2.0f)) * 1.5f);
        } else {
            p = lines[(n - 1 + N_LINES) % N_LINES];
        }

        bool isDark = (n / 3) % 2;
        const QColor &grass = isDark ? GRASS_DARK : GRASS_LIGHT;
        const QColor &rumble = isDark ? RUMBLE_DARK : RUMBLE_LIGHT;
        const QColor &road = isDark ? ROAD_DARK : ROAD_LIGHT;

        drawQuad(painter, grass, 0, p.Y, screenWidth, 0, l.Y, screenWidth);
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
        l.drawSprite(painter, screenWidth, screenHeight);
    }
}

void Terrain::generateTerrain()
{
    // Load les sprites
    QPixmap testSprite;
    if (testSprite.load(":/images/test.png")) {
        QPixmapCache::insert("test_obstacle0", testSprite);
    } else {
        qWarning() << "Warning: Erreur durant l'ouverture de test.png";
    }

    for (int i = 0; i < N_LINES; i++) {
        Line line;
        line.z = i * SEG_L;

        if(i>50 && i<700) line.curve=0.02;
        line.nbLane = 3;
        if(i>0 && i < 200) line.isLineFull = true;
        if(i > 100 && i < 700) line.y = sin(i / 30.0) * 150;

        if(i == 50) {
            Obstacle obstacle("test_obstacle", 1, -5.0f, 0.05, 1);
            line.obstacles.append(obstacle);
        }

        lines.push_back(line);
    }
}

Line &Terrain::getLine(int index)
{
    return lines[index];
}

const Line &Terrain::getLine(int index) const
{
    return lines[index];
}

int Terrain::getTotalLines() const
{
    return N_LINES;
}

void Terrain::drawQuad(QPainter &painter, QColor color, int x1, int y1, int w1, int x2, int y2, int w2)
{
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
