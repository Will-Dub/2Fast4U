#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QObject>
#include <QMap>
#include <QString>
#include <QUrl>
#include <cmath>
#include <algorithm>

struct AudioSource {
    QString sourcePath;
    double rpm = 1000.0;
    double volume = 1.0;
};

struct DynamicAudioNode {
    QMediaPlayer* player = nullptr;
    QAudioOutput* audioOutput = nullptr;
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

    QMap<QString, DynamicAudioNode> samples;

    void init(const QMap<QString, AudioSource>& sources);

    DynamicAudioNode add(const AudioSource& source);

    static CrossFadeResult crossFade(double value, double start, double end);

    void dispose();
};

#endif // AUDIOMANAGER_H
