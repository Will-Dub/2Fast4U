#include "spritemanager.h"


const QPixmap &SpriteManager::get(const QString &id)
{
    static QHash<QString, QPixmap> sprites;

    if (sprites.isEmpty()) {
        loadSprite(sprites, "arbre0", ":/images/arbre.png");
        loadSprite(sprites, "buche0", ":/images/buche.png");
        loadSprite(sprites, "pole0", ":/images/pole.png");
        loadSprite(sprites, "roche0", ":/images/roche.png");
        loadSprite(sprites, "habitacle", ":/images/habitacle.png");
        loadSprite(sprites, "habitacle", ":/images/habitacleCrash.png");
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
