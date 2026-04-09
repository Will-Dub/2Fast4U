#include "terrain.h"

Terrain::Terrain() {
    static const QColor GRASS_LIGHT(34, 164, 83);
    static const QColor GRASS_DARK(20, 130, 72);
    static const QColor RUMBLE_LIGHT(143, 143, 143);
    static const QColor RUMBLE_DARK(121, 121, 121);
    static const QColor SHOULDER_LIGHT(160, 110, 70);
    static const QColor SHOULDER_DARK(120, 80, 50);
    static const QColor ROAD_LIGHT(72, 77, 91);
    static const QColor ROAD_DARK(57, 62, 77);
    static const QColor LINE(255, 220, 70);
    static const QColor FOG_COLOR(126, 185, 224);

    auto applyFog = [](const QColor& baseColor, const QColor& fogColor, float fogFactor) -> QColor {
        fogFactor = std::max(0.0f, std::min(1.0f, fogFactor));
        int r = baseColor.red() + (fogColor.red() - baseColor.red()) * fogFactor;
        int g = baseColor.green() + (fogColor.green() - baseColor.green()) * fogFactor;
        int b = baseColor.blue() + (fogColor.blue() - baseColor.blue()) * fogFactor;
        return QColor(r, g, b, baseColor.alpha());
        };

    for (int i = 0; i < MAX_DRAW_DISTANCE; i++) {
        float distanceRatio = (float)i / MAX_DRAW_DISTANCE;
        float fogFactor = std::pow(distanceRatio, 1.5f);
        precalcFogFactor[i] = fogFactor;

        precalcGrassLight[i] = applyFog(GRASS_LIGHT, FOG_COLOR, fogFactor);
        precalcGrassDark[i] = applyFog(GRASS_DARK, FOG_COLOR, fogFactor);
        precalcRumbleLight[i] = applyFog(RUMBLE_LIGHT, FOG_COLOR, fogFactor);
        precalcRumbleDark[i] = applyFog(RUMBLE_DARK, FOG_COLOR, fogFactor);
        precalcShoulderLight[i] = applyFog(SHOULDER_LIGHT, FOG_COLOR, fogFactor);
        precalcShoulderDark[i] = applyFog(SHOULDER_DARK, FOG_COLOR, fogFactor);
        precalcRoadLight[i] = applyFog(ROAD_LIGHT, FOG_COLOR, fogFactor);
        precalcRoadDark[i] = applyFog(ROAD_DARK, FOG_COLOR, fogFactor);

        precalcLine[i] = applyFog(LINE, FOG_COLOR, fogFactor);
        precalcEdgeLine[i] = applyFog(QColor(250, 250, 255), FOG_COLOR, fogFactor);
        precalcRoadGlow[i] = applyFog(QColor(255, 255, 255, 22), FOG_COLOR, fogFactor);
    }
}

void Terrain::render(QPainter& painter, Player& player, int screenWidth, int screenHeight)
{
    int startPos = player.getPositionZ() / SEG_L;
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
    for (int n = startPos; n < startPos + MAX_DRAW_DISTANCE; n++) {
        int index = (n % N_LINES + N_LINES) % N_LINES;
        Line& l = lines[index];

        l.project(player.getPositionX() * ROAD_W - x, camHeight, player.getPositionZ() - ((n >= N_LINES) ? N_LINES * SEG_L : 0), screenWidth, screenHeight, cameraAngle, player.pitch, cameraDepth);

        x += dx;
        dx += l.curve;

        l.clip = maxy;
        if (l.Y > maxy) continue;
        maxy = l.Y;

        if (maxy <= 0) break;

        Line p;
        if (n == startPos) {
            // Extrapole la ligne
            p = l;
            p.Y = screenHeight;
            p.W = l.W * 1.5f;
            p.X = (screenWidth / 2.0f) + ((l.X - (screenWidth / 2.0f)) * 1.5f);
        }
        else {
            p = lines[(n - 1 + N_LINES) % N_LINES];
        }

        bool isDark = (n / 3) % 2;
        int distIndex = n - startPos;

        QColor grass = isDark ? precalcGrassDark[distIndex] : precalcGrassLight[distIndex];
        QColor rumble = isDark ? precalcRumbleDark[distIndex] : precalcRumbleLight[distIndex];
        QColor shoulder = isDark ? precalcShoulderDark[distIndex] : precalcShoulderLight[distIndex];
        QColor road = isDark ? precalcRoadDark[distIndex] : precalcRoadLight[distIndex];

        drawQuad(painter, grass, 0, p.Y, screenWidth, 0, l.Y, screenWidth);
        drawQuad(painter, shoulder, p.X, p.Y, p.W * 1.42, l.X, l.Y, l.W * 1.42);
        drawQuad(painter, rumble, p.X, p.Y, p.W * 1.24, l.X, l.Y, l.W * 1.24);
        drawQuad(painter, road, p.X, p.Y, p.W, l.X, l.Y, l.W);

        bool isLine = l.isLineFull || (n / 6) % 2;
        if (isLine) {
            float widthLigneP = std::max(1.0f, (float)(p.W * 0.02));
            float widthLigneL = std::max(1.0f, (float)(l.W * 0.02));

            int nbLigne = l.nbLane - 1;
            if (nbLigne == 0) continue;

            float laneWidthP = (p.W * 2) / l.nbLane;
            float laneWidthL = (l.W * 2) / l.nbLane;

            QColor fadedLine = precalcLine[distIndex];

            for (int i = 1; i < nbLigne + 1; i++) {
                float offsetP = -p.W + (laneWidthP * i);
                float offsetL = -l.W + (laneWidthL * i);
                drawQuad(painter, fadedLine, p.X + offsetP, p.Y, widthLigneP, l.X + offsetL, l.Y, widthLigneL);
            }
        }

        if (l.isLineFull || (n / 10) % 2) {
            QColor edgeLine = precalcEdgeLine[distIndex];
            float widthEdgeP = std::max(1.0f, (float)(p.W * 0.012f));
            float widthEdgeL = std::max(1.0f, (float)(l.W * 0.012f));
            drawQuad(painter, edgeLine, p.X - p.W * 0.92f, p.Y, widthEdgeP, l.X - l.W * 0.92f, l.Y, widthEdgeL);
            drawQuad(painter, edgeLine, p.X + p.W * 0.92f, p.Y, widthEdgeP, l.X + l.W * 0.92f, l.Y, widthEdgeL);
        }
    }

    // Affiche les sprites
    /*for (int n = startPos + MAX_DRAW_DISTANCE; n > startPos; n--) {
        Line& l = lines[n % N_LINES];

        int distIndex = n - startPos;
        float fogFactor = precalcFogFactor[distIndex];

        l.drawSprite(painter, screenWidth, screenHeight, fogFactor);
    }*/
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
            line.obstacles.append(Obstacle("arbre0", 1, 6.0f, 0.025f, 2));
            line.obstacles.append(Obstacle("arbre0", 1, -6.0f, 0.025f, 2));
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
        if (i == 175) {
            line.obstacles.append(Obstacle("pole", 1, 5.0f, 0.025f, 2));
            line.obstacles.append(Obstacle("pole", 1, -5.0f, 0.025f, 2));
        }

        // 5. Grosse Montagne : Arbres denses sur les côtés
        if (i % 100 == 0 && i >= 1700 && i < 2300) {
            line.obstacles.append(Obstacle("arbre0", 1, 5.5f, 0.025f, 2));
            line.obstacles.append(Obstacle("arbre0", 1, -5.5f, 0.025f, 2));
        }

        // 6. Piège dans la vallée : Combo Roche + Bûche
        if (i == 3000) {
            line.obstacles.append(Obstacle("roche", 1, -2.0f, 0.01f, 2.5f)); // Bloque la gauche
            line.obstacles.append(Obstacle("buche", 1, 1.5f, 0.02f, 2.0f));  // Gêne la droite
        }

        //Arbre
        // -----------------------------
        // GÉNÉRATION DE LA FORÊT (Arbres 0 à 3)
        // -----------------------------
        // Moins fréquent que le gazon (1 ligne sur 6 ou 8) pour sauver le CPU
        if (i % 8 == 0) {
            // Seulement 2 ou 3 arbres maximum par côté par ligne.
            // La perspective fera le reste du travail pour rendre ça dense.
            int treeDensityPerSide = 2;

            for (int t = 0; t < treeDensityPerSide; t++) {
                // ==========================================
                // CÔTÉ GAUCHE
                // ==========================================
                // On commence plus loin de la route (10.0 au lieu de 6.0) pour éviter les collisions injustes.
                // Profondeur massive jusqu'à 210.0
                float randomOffsetLeft = -(10.0f + (rand() % 2000) / 10.0f);

                // Sélectionne au hasard arbre0, arbre1, arbre2 ou arbre3
                QString typeLeft = "arbre" + QString::number(rand() % 4);

                // L'échelle d'un arbre doit être bien plus grande que celle du gazon.
                // Modifie ces valeurs (0.025 à 0.040) selon la taille réelle de tes images PNG.
                float scaleLeft = 0.025f + (rand() % 15) / 1000.0f;

                line.obstacles.append(Obstacle(typeLeft, 1, randomOffsetLeft, scaleLeft, 2.0f));

                // ==========================================
                // CÔTÉ DROIT
                // ==========================================
                float randomOffsetRight = 10.0f + (rand() % 2000) / 10.0f;

                QString typeRight = "arbre" + QString::number(rand() % 4);
                float scaleRight = 0.025f + (rand() % 15) / 1000.0f;

                line.obstacles.append(Obstacle(typeRight, 1, randomOffsetRight, scaleRight, 2.0f));
            }
        }

        // Gazon
        if (i % 2 == 0) {
            int grassDensityPerSide = 7;

            for (int g = 0; g < grassDensityPerSide; g++) {
                // ==========================================
                // CÔTÉ GAUCHE (Indépendant)
                // ==========================================
                // On a passé le rand de 800 à 2500. 
                // Le décalage va maintenant de -6.0 jusqu'à -256.0. C'est immense.
                float randomOffsetLeft = -(6.0f + (rand() % 2500) / 10.0f);
                bool isTallLeft = (rand() % 100 < 25);

                QString typeLeft = isTallLeft ? "grass" : "grass";
                float scaleLeft = isTallLeft ? (0.009f + (rand() % 9) / 1000.0f) : (0.004f + (rand() % 3) / 1000.0f);

                line.obstacles.append(Obstacle(typeLeft, 1, randomOffsetLeft, scaleLeft, 2.0f));

                // ==========================================
                // CÔTÉ DROIT (Indépendant)
                // ==========================================
                // Décalage entre 6.0 et 256.0.
                float randomOffsetRight = 6.0f + (rand() % 2500) / 10.0f;
                bool isTallRight = (rand() % 100 < 25);

                QString typeRight = isTallRight ? "grass" : "grass";
                float scaleRight = isTallRight ? (0.009f + (rand() % 9) / 1000.0f) : (0.004f + (rand() % 3) / 1000.0f);

                line.obstacles.append(Obstacle(typeRight, 1, randomOffsetRight, scaleRight, 2.0f));
            }
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

inline void Terrain::drawQuad(QPainter& painter, const QColor& color, float x1, float y1, float w1, float x2, float y2, float w2)
{
    // Utilisation de QPointF pour éviter les conversions float -> int
    QPointF points[4] = {
        QPointF(x1 - w1, y1),
        QPointF(x2 - w2, y2),
        QPointF(x2 + w2, y2),
        QPointF(x1 + w1, y1)
    };

    // Modification du state et dessin
    painter.setBrush(color);
    painter.drawConvexPolygon(points, 4);
}
