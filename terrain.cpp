#include "terrain.h"

Terrain::Terrain() {}

void Terrain::render(QPainter &painter, Player &player, int screenWidth, int screenHeight)
{
    static const QColor GRASS_LIGHT(8, 150, 0);
    static const QColor GRASS_DARK(0, 124, 0);
    static const QColor RUMBLE_LIGHT(143, 143, 143);
    static const QColor RUMBLE_DARK(121, 121, 121);
    static const QColor ROAD_LIGHT(107, 107, 107);
    static const QColor ROAD_DARK(105, 105, 105);
    static const QColor LINE(255, 204, 0);

    int startPos = player.getPositionZ()/SEG_L;
    float percent = (player.getPositionZ() - (startPos * SEG_L)) / (float)SEG_L;

    int safeStart = (startPos % N_LINES + N_LINES) % N_LINES;
    int safeNext = ((startPos + 1) % N_LINES + N_LINES) % N_LINES;

    Line currentLine = lines[safeStart];
    float currentY = currentLine.y;
    float nextY = lines[safeNext].y;

    // Hauteur de la caméra
    float rawSlope = nextY - currentY;
    float camHeight = player.getPositionY() + (currentY + rawSlope * percent);

    // Angle de la caméra
    int lookahead = 15;
    float lookAheadY = lines[(startPos + lookahead) % N_LINES].y;
    float actualSlope = (lookAheadY - currentY) / (lookahead * SEG_L);

    float pitchStrength = 1.0f;
    float targetPitch = actualSlope * pitchStrength;

    player.pitch += (targetPitch - player.pitch) * 0.1f;

    // Depth de la caméra
    float baseDepth = 0.8f;
    float speedFactor = player.getSpeed() / 300;

    // Augmente le FOV dépendamment de la vitesse
    float cameraDepth = baseDepth - (speedFactor * 0.2f);

    // Tournages
    float x = 0;
    float dx = -(currentLine.curve * percent);
    int maxy = screenHeight;
    float cameraAngle = player.getAngle();

    // Montre les 600 lignes devant
    for(int n=startPos; n<startPos+600;n++){
        int index = (n % N_LINES + N_LINES) % N_LINES;
        Line& l = lines[index];

        l.project(player.getPositionX()*ROAD_W-x, camHeight, player.getPositionZ() - ((n>=N_LINES)?N_LINES*SEG_L:0), screenWidth, screenHeight, cameraAngle, player.pitch, cameraDepth);

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
    lines.clear();

    int mountainStart = 100;
    int mountainLength = 300;
    float mountainHeight = 500.0f; // baisse un peu la hauteur pour éviter les gaps

    for (int i = 0; i < N_LINES; i++) {
        Line line;
        line.z = i * SEG_L;
        line.y = 0.0f;
        line.curve = 0.0f;
        line.nbLane = 3;
        line.isLineFull = false;

        // -----------------------------
        // SECTION 1 : départ plat
        // -----------------------------
        if (i >= 0 && i < 200) {
            line.y = 0.0f;
            line.curve = 0.0f;
            line.isLineFull = true;
        }

        // -----------------------------
        // SECTION 2 : montée douce
        // -----------------------------
        else if (i >= 200 && i < 500) {
            float t = float(i - 200) / float(500 - 200);
            line.y = 300.0f * sin(M_PI * 0.5f * t); // monte progressivement
            line.curve = 0.0f;
        }

        // -----------------------------
        // SECTION 3 : virage à droite en haut
        // -----------------------------
        else if (i >= 500 && i < 900) {
            float t = float(i - 500) / float(900 - 500);
            line.y = 300.0f;
            line.curve = 0.015f + 0.01f * t;
        }

        // -----------------------------
        // SECTION 4 : descente douce
        // -----------------------------
        else if (i >= 900 && i < 1300) {
            float t = float(i - 900) / float(1300 - 900);
            line.y = 300.0f * cos(M_PI * 0.5f * t); // redescend vers 0
            line.curve = 0.02f;
        }

        // -----------------------------
        // SECTION 5 : ligne droite plate
        // -----------------------------
        else if (i >= 1300 && i < 1700) {
            line.y = 0.0f;
            line.curve = 0.0f;
        }

        // -----------------------------
        // SECTION 6 : grosse montagne
        // -----------------------------
        else if (i >= 1700 && i < 2300) {
            float t = float(i - 1700) / float(2300 - 1700);
            float x = 2.0f * t - 1.0f;
            line.y = 500.0f * (1.0f - x * x); // parabole inversée
            line.curve = -0.01f;
        }

        // -----------------------------
        // SECTION 7 : virage à gauche
        // -----------------------------
        else if (i >= 2300 && i < 2800) {
            float t = float(i - 2300) / float(2800 - 2300);
            line.y = 150.0f;
            line.curve = -0.03f * t;
        }

        // -----------------------------
        // SECTION 8 : vallée
        // -----------------------------
        else if (i >= 2800 && i < 3400) {
            float t = float(i - 2800) / float(3400 - 2800);
            float x = 2.0f * t - 1.0f;
            line.y = -250.0f * (1.0f - x * x); // creux
            line.curve = -0.02f;
        }

        // -----------------------------
        // SECTION 9 : remontée légère
        // -----------------------------
        else if (i >= 3400 && i < 3900) {
            float t = float(i - 3400) / float(3900 - 3400);
            line.y = -250.0f + 250.0f * t;
            line.curve = 0.01f;
        }

        // -----------------------------
        // SECTION 10 : longue fin droite
        // -----------------------------
        else if (i >= 3900 && i < N_LINES) {
            line.y = 0.0f;
            line.curve = 0.0f;
        }

        if(i == 25){
            line.obstacles.append(Obstacle("roche", 1, 3.0f, 0.01f, 2.5f));
        }

        if (i == 50){
            line.obstacles.append(Obstacle("buche", 1, 0.0f, 0.02f, 2.0f));
        }

        if (i % 150 == 0 && i > 100 && i < 500) {
            line.obstacles.append(Obstacle("arbre", 1, 6.0f, 0.025f, 2));
            line.obstacles.append(Obstacle("arbre", 1, -6.0f, 0.025f, 2));
        }

        // 2. Danger dans la montée/virage : Roches proches de la piste
        if (i == 600 || i == 800) {
            float posX = (i == 600) ? 2.5f : -2.5f;
            line.obstacles.append(Obstacle("roche", 1, posX, 0.01f, 2.5f));
        }

        // 3. Bûche en plein milieu de la descente (Section 4)
        if (i == 1100) {
            line.obstacles.append(Obstacle("buche", 1, 0.0f, 0.02f, 2.0f));
        }

        // 4. LES POLES (Section 5) - Intouchés
        if (i == 1500) {
            line.obstacles.append(Obstacle("pole", 1, 5.0f, 0.025f, 2));
            line.obstacles.append(Obstacle("pole", 1, -5.0f, 0.025f, 2));
        }

        // 5. Grosse Montagne : Arbres denses sur les côtés
        if (i % 100 == 0 && i >= 1700 && i < 2300) {
            line.obstacles.append(Obstacle("arbre", 1, 5.5f, 0.025f, 2));
            line.obstacles.append(Obstacle("arbre", 1, -5.5f, 0.025f, 2));
        }

        // 6. Piège dans la vallée : Combo Roche + Bûche
        if (i == 3000) {
            line.obstacles.append(Obstacle("roche", 1, -2.0f, 0.01f, 2.5f)); // Bloque la gauche
            line.obstacles.append(Obstacle("buche", 1, 1.5f, 0.02f, 2.0f));  // Gêne la droite
        }

        lines.push_back(line);
    }
}

void Terrain::tick(Player &player, float dt)
{
    int startPos = player.getPositionZ()/SEG_L;

    for(int n=startPos; n<startPos+600;n++){
        int index = (n % N_LINES + N_LINES) % N_LINES;
        Line& l = lines[index];

        for(Obstacle& obstacle : l.obstacles){
            obstacle.update(dt);
        }
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
