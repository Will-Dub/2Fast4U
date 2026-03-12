#include "powertrainaudiocontroller.h"

PowertrainAudioController::PowertrainAudioController(Powertrain* powertrainPtr)
    : m_powertrainPtr(powertrainPtr), m_softLimiter(6000.0), m_limiter(6500.0), m_rpmPitchFactor(0.2) {
}

PowertrainAudioController::~PowertrainAudioController() {}

void PowertrainAudioController::initSounds() {
    QMap<QString, AudioSource> sources;

    sources.insert("on_low", {"qrc:/audio/engine_on_low.wav", 3000.0, 1.0});
    sources.insert("off_low", {"qrc:/audio/engine_off_low.wav", 3000.0, 1.0});
    sources.insert("on_high", {"qrc:/audio/engine_on_high.wav", 6500.0, 1.0});
    sources.insert("off_high", {"qrc:/audio/engine_off_high.wav", 6500.0, 1.0});
    sources.insert("limiter", {"qrc:/audio/limiter.wav", 6500.0, 1.0});

    sources.insert("tranny_on", {"qrc:/audio/tranny_on.wav", 1000.0, 0.8});
    sources.insert("tranny_off", {"qrc:/audio/tranny_off.wav", 1000.0, 0.8});

    m_audioManager.init(sources);
}

void PowertrainAudioController::update() {
    // Safety check: if the physics model was destroyed, do not calculate audio.
    if (!m_powertrainPtr) return;

    double currentRevs = m_powertrainPtr->getRevs();
    double currentThrottle = m_powertrainPtr->getThrottle();
    double currentGearRatio = m_powertrainPtr->getGearRatio();

    CrossFadeResult rpmFade = AudioManager::crossFade(currentRevs, 3000.0, 6500.0);
    CrossFadeResult throttleFade = AudioManager::crossFade(currentThrottle, 0.0, 1.0);

    // Aliasing the results to match your TS variables for readability
    double high = rpmFade.gain1;
    double low  = rpmFade.gain2;
    double on   = throttleFade.gain1;
    double off  = throttleFade.gain2;

    // Translated TS ratio() function using standard clamp
    double limiterMin = m_softLimiter * 0.93;
    double limiterGain = std::clamp((currentRevs - limiterMin) / (m_limiter - limiterMin), 0.0, 1.0);

    // C++ Lambda to replace your nested TS function
    auto applySample = [&](const QString& key, double gain, bool applyPitch = true) {
        if (!m_audioManager.samples.contains(key)) return;

        auto& node = m_audioManager.samples[key];

        if (applyPitch) {
            // Standard multiplier logic for pitch shifting
            double pitchMultiplier = 1.0 + ((currentRevs - node.rpm) / node.rpm) * m_rpmPitchFactor;

            // CRITICAL: Qt will break if playbackRate is <= 0.
            pitchMultiplier = std::max(0.1, pitchMultiplier);
            node.player->setPlaybackRate(pitchMultiplier);
        }

        node.audioOutput->setVolume(gain * node.baseVolume);
    };

    // Apply main engine loops
    applySample("on_low", on * low);
    applySample("off_low", off * low);
    applySample("on_high", on * high);
    applySample("off_high", off * high);
    applySample("limiter", limiterGain, false);

    // Transmission
    if (m_audioManager.samples.contains("tranny_on") && m_audioManager.samples.contains("tranny_off")) {
        auto& trannyOn = m_audioManager.samples["tranny_on"];
        auto& trannyOff = m_audioManager.samples["tranny_off"];

        // Converting WebAudio 'cents' into a Qt PlaybackRate Multiplier
        double trannyOnCents = (currentRevs * currentGearRatio * 0.05) - 100.0;
        double trannyOnRate = std::pow(2.0, trannyOnCents / 1200.0);
        trannyOn.player->setPlaybackRate(std::max(0.1, trannyOnRate));
        trannyOn.audioOutput->setVolume(currentGearRatio > 0 ? on * trannyOn.baseVolume : 0.0);

        double trannyOffCents = (currentRevs * currentGearRatio * 0.035) - 800.0;
        double trannyOffRate = std::pow(2.0, trannyOffCents / 1200.0);
        trannyOff.player->setPlaybackRate(std::max(0.1, trannyOffRate));
        trannyOff.audioOutput->setVolume(currentGearRatio > 0 ? off * trannyOff.baseVolume : 0.0);
    }

    // Check the inverse volume (if off is 0, on should be 1.0)
    qDebug() << "Audio Tick - RPM:" << currentRevs
             << " | On_Low Vol:" << (on * low)
             << " | Off_Low Vol:" << (off * low);

    // Interrogate the actual Qt Media Player for the 'on_low' sound
    if (m_audioManager.samples.contains("on_low")) {
        auto status = m_audioManager.samples["on_low"].player->mediaStatus();
        auto error = m_audioManager.samples["on_low"].player->error();
        qDebug() << "On_Low Media Status:" << status << "| Error:" << error;
    }
}
