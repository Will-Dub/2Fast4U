#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <cmath>
#include <algorithm>

// Include SoLoud
#include "soloud.h"
#include "soloud_wav.h"

struct AudioSource {
    QString sourcePath;
    double rpm = 1000.0;
    double volume = 1.0;
};

struct DynamicAudioNode {
    SoLoud::Wav* wav = nullptr;
    SoLoud::handle handle = 0; // The active playback ID
    double rpm = 1000.0;
    double baseVolume = 1.0;
};

struct CrossFadeResult {
    double gain1;
    double gain2;
};

class AudioManager: public QObject
{
    Q_OBJECT
public:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();

    SoLoud::Soloud engine;
    QMap<QString, DynamicAudioNode> samples;

    void init(const QMap<QString, AudioSource>& sources);
    void add(const QString& key, const AudioSource& source);
    static CrossFadeResult crossFade(double value, double start, double end);
    void dispose();
};

#endif // AUDIOMANAGER_H
