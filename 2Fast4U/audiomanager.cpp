#include "audiomanager.h"
#include <QDebug>

AudioManager::AudioManager(QObject *parent) : QObject(parent) {
    // Initialize the audio engine. It automatically picks the best backend.
    engine.init();
}

AudioManager::~AudioManager() {
    dispose();
}

void AudioManager::init(const QMap<QString, AudioSource>& sources) {
    for (auto it = sources.constBegin(); it != sources.constEnd(); ++it) {
        add(it.key(), it.value());
    }
}

void AudioManager::add(const QString& key, const AudioSource& source) {
    DynamicAudioNode node;
    node.rpm = source.rpm;
    node.baseVolume = source.volume;

    // Allocate the Wav object on the heap
    node.wav = new SoLoud::Wav();

    QString cleanPath = source.sourcePath;
    cleanPath.replace("qrc:/", ":/");

    QFile file(cleanPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();

        // Use arrow syntax (->) because wav is now a pointer
        node.wav->loadMem((unsigned char*)data.data(), data.size(), true, false);
        node.wav->setLooping(true);

        samples.insert(key, node);
    } else {
        qWarning() << "Failed to load audio file:" << cleanPath;
        delete node.wav; // Clean up the memory if the file fails to load
    }
}

CrossFadeResult AudioManager::crossFade(double value, double start, double end) {
    double x = std::clamp((value - start) / (end - start), 0.0, 1.0);
    double gain1 = std::cos((1.0 - x) * 0.5 * M_PI);
    double gain2 = std::cos(x * 0.5 * M_PI);
    return {gain1, gain2};
}

void AudioManager::dispose() {
    engine.deinit();

    // You must manually free the heap memory before clearing the map
    for (auto& node : samples) {
        if (node.wav) {
            delete node.wav;
            node.wav = nullptr;
        }
    }
    samples.clear();
}
