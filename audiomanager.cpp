#include "audiomanager.h"

AudioManager::AudioManager(QObject *parent) : QObject(parent) {}

AudioManager::~AudioManager() {
    dispose();
}

void AudioManager::init(const QMap<QString, AudioSource>& sources) {
    for (auto it = sources.constBegin(); it != sources.constEnd(); ++it) {
        samples.insert(it.key(), add(it.value()));
    }
}

DynamicAudioNode AudioManager::add(const AudioSource& source) {
    DynamicAudioNode node;


    node.player = new QMediaPlayer(this);
    node.audioOutput = new QAudioOutput(this);

    node.player->setAudioOutput(node.audioOutput);
    node.player->setSource(QUrl(source.sourcePath));

    node.player->setLoops(QMediaPlayer::Infinite);

    node.audioOutput->setVolume(0.0);

    node.rpm = source.rpm;
    node.baseVolume = source.volume;

    node.player->play();

    return node;
}

CrossFadeResult AudioManager::crossFade(double value, double start, double end) {
    double x = std::clamp((value - start) / (end - start), 0.0, 1.0);
    double gain1 = std::cos((1.0 - x) * 0.5 * M_PI);
    double gain2 = std::cos(x * 0.5 * M_PI);

    return {gain1, gain2};
}

void AudioManager::dispose() {
    for (auto& node : samples) {
        if (node.player) {
            node.player->stop();
        }
    }
    samples.clear();
}
