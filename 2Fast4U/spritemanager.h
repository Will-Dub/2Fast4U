#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <QPixmap>
#include <QHash>
#include <QString>
#include <QDebug>

class SpriteManager {
public:
    static const QPixmap& get(const QString& id);

private:
    static void loadSprite(QHash<QString, QPixmap>& map, const QString& id, const QString& path);
};

#endif // SPRITEMANAGER_H
