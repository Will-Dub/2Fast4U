/*==================================================
* NOM: spritemanager.cpp
* AUTEUR(S): William Dubuc, Samuel Guertin
* Date: Avril 2026
* Description: Contient les implémentations des méthodes et de la classe déclaré dans spritemanager.h
===================================================*/

#include "spritemanager.h"


const QPixmap &SpriteManager::get(const QString &id)
{
    static QHash<QString, QPixmap> sprites;

    if (sprites.isEmpty()) {
        loadSprite(sprites, "arbre00", ":/images/arbre0.png");
        loadSprite(sprites, "arbre10", ":/images/arbre1.png");
        loadSprite(sprites, "arbre20", ":/images/arbre2.png");
        loadSprite(sprites, "arbre30", ":/images/arbre3.png");
        loadSprite(sprites, "buche0", ":/images/buche.png");
        loadSprite(sprites, "pole0", ":/images/pole.png");
        loadSprite(sprites, "roche0", ":/images/roche.png");
        loadSprite(sprites, "habitacle", ":/images/habitacle.png");
        loadSprite(sprites, "habitacleCrashed", ":/images/habitacleCrashed.png");
        loadSprite(sprites, "grass0", ":/images/grass.png");
        loadSprite(sprites, "tallGrass0", ":/images/tallGrass.png");
        loadSprite(sprites, "cerf0", ":/images/cerf0.png");
        loadSprite(sprites, "cerf1", ":/images/cerf1.png");
        loadSprite(sprites, "cerf2", ":/images/cerf2.png");
        loadSprite(sprites, "cerf3", ":/images/cerf3.png");
        loadSprite(sprites, "cerf4", ":/images/cerf4.png");
        loadSprite(sprites, "cerf5", ":/images/cerf5.png");
        loadSprite(sprites, "cerf6", ":/images/cerf6.png");
        loadSprite(sprites, "cerf7", ":/images/cerf7.png");

        static const QColor FOG_COLOR(126, 185, 224); // Ta couleur de brouillard

        // On prend une copie des cl�s pour ne pas crasher le QHash en ajoutant des �l�ments
        QStringList baseKeys = sprites.keys();

        for (const QString& key : baseKeys) {
            // Ne pas appliquer de brouillard sur le HUD/UI de la voiture
            if (key == "habitacle" || key == "habitacleCrashed") continue;

            QPixmap baseSprite = sprites[key];

            // G�n�re 10 niveaux de brouillard
            for (int i = 0; i < 10; ++i) {
                float fogFactor = i / 10.0f;

                QPixmap tintedPixmap(baseSprite.size());
                tintedPixmap.fill(Qt::transparent);

                QPainter p(&tintedPixmap);
                p.drawPixmap(0, 0, baseSprite);
                p.setCompositionMode(QPainter::CompositionMode_SourceAtop);

                QColor currentFog = FOG_COLOR;
                currentFog.setAlphaF(fogFactor);
                p.fillRect(tintedPixmap.rect(), currentFog);
                p.end();

                // Sauvegarde avec la cl� de brouillard (ex: "arbre00_fog5")
                QString fogKey = key + "_fog" + QString::number(i);
                sprites[fogKey] = tintedPixmap;
            }
        }
    }

    if (!sprites.contains(id)) {
        qWarning() << "CRITICAL: Le sprite n'existe pas " << id;
    }

    return sprites[id];
}

void SpriteManager::loadSprite(QHash<QString, QPixmap> &map, const QString &id, const QString &path)
{
    QPixmap pixmap;
    if (pixmap.load(path)) {
        map[id] = pixmap;
    } else {
        qWarning() << "CRITICAL: Impossible d'ouvrir: " << path;
    }
}
