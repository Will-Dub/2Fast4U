#include "powertrainaudiocontroller.h"

PowertrainAudioController::PowertrainAudioController(Powertrain* powertrainPtr)
    : m_powertrainPtr(powertrainPtr), m_softLimiter(6000.0), m_limiter(6500.0), m_rpmPitchFactor(0.2) {
}

PowertrainAudioController::~PowertrainAudioController() {}

void PowertrainAudioController::initSounds() {
    QMap<QString, AudioSource> sources;

    sources.insert("on_low", {":/audio/audio/BAC_Mono_onlow.wav", 1000.0, 0.5});
    sources.insert("off_low", {":/audio/audio/BAC_Mono_offlow.wav", 1000.0, 0.5});
    sources.insert("on_high", {":/audio/audio/BAC_Mono_onhigh.wav", 1000.0, 0.5});
    sources.insert("off_high", {":/audio/audio/BAC_Mono_offveryhigh.wav", 1000.0, 0.5});
    sources.insert("limiter", {":/audio/audio/limiter.wav", 8000.0, 0.4});

    sources.insert("tranny_on", {":/audio/audio/trany_power_high.wav", 0.0, 0.4});
    sources.insert("tranny_off", {":/audio/audio/tw_offlow_4.wav", 0.0, 0.2});
    sources.insert("snap", {":/audio/audio/explosion_cable_break.wav", 0.0, 1.5});
    sources.insert("thud", {":/audio/audio/explosion_base.wav", 0.0, 1.2});

    m_audioManager.init(sources);
}

void PowertrainAudioController::update() {
    if (!m_isPlaying || !m_powertrainPtr) return;

    double currentRevs = m_powertrainPtr->getRevs();
    double currentThrottle = m_powertrainPtr->getThrottle();
    int gearIndex = m_powertrainPtr->getGear();
    double currentGearRatio = m_powertrainPtr->getGearRatio();

    CrossFadeResult rpmFade = AudioManager::crossFade(currentRevs, 3000.0, 6500.0);
    CrossFadeResult throttleFade = AudioManager::crossFade(currentThrottle, 0.0, 1.0);

    double high = rpmFade.gain1;
    double low  = rpmFade.gain2;
    double on   = throttleFade.gain1;
    double off  = throttleFade.gain2;

    double limiterMin = m_softLimiter * 0.93;
    double limiterGain = std::clamp((currentRevs - limiterMin) / (m_limiter - limiterMin), 0.0, 1.0);

    // Apply basic engine loops
    auto applySample = [&](const QString& key, double gain, bool applyPitch = true) {
        if (!m_audioManager.samples.contains(key)) return;
        auto& node = m_audioManager.samples[key];

        if (applyPitch) {
            double cents = (currentRevs - node.rpm) * m_rpmPitchFactor;
            double pitchMultiplier = std::pow(2.0, cents / 1200.0);
            pitchMultiplier = std::clamp(pitchMultiplier, 0.1, 4.0);

            // SoLoud handles continuous speed modulation natively
            m_audioManager.engine.setRelativePlaySpeed(node.handle, pitchMultiplier);
        }

        m_audioManager.engine.setVolume(node.handle, gain * node.baseVolume);
    };

    applySample("on_low", on * low);
    applySample("off_low", off * low);
    applySample("on_high", on * high);
    applySample("off_high", off * high);
    applySample("limiter", limiterGain, false);

    // Apply Transmission
    if (m_audioManager.samples.contains("tranny_on") && m_audioManager.samples.contains("tranny_off")) {
        auto& trannyOn = m_audioManager.samples["tranny_on"];
        auto& trannyOff = m_audioManager.samples["tranny_off"];

        // Transmission On (Throttle applied)
        double trannyOnCents = (currentRevs * gearIndex * 0.05) - 100.0;
        double trannyOnRate = std::pow(2.0, trannyOnCents / 1200.0);
        trannyOnRate = std::clamp(trannyOnRate, 0.1, 4.0);

        m_audioManager.engine.setRelativePlaySpeed(trannyOn.handle, trannyOnRate);

        double trannyVol = (currentGearRatio > 0) ? (on * trannyOn.baseVolume) : 0.0;
        m_audioManager.engine.setVolume(trannyOn.handle, trannyVol);

        // Transmission Off (Coasting/Braking)
        double trannyOffCents = (currentRevs * currentGearRatio * 0.035) - 800.0;
        double trannyOffRate = std::pow(2.0, trannyOffCents / 1200.0);
        trannyOffRate = std::clamp(trannyOffRate, 0.1, 4.0);

        m_audioManager.engine.setRelativePlaySpeed(trannyOff.handle, trannyOffRate);
        m_audioManager.engine.setVolume(trannyOff.handle, currentGearRatio > 0 ? off * trannyOff.baseVolume : 0.0);
    }
}

void PowertrainAudioController::start() {
    m_isPlaying = true;

    for (auto& node : m_audioManager.samples) {
        if (node.wav) {
            // Dereference the pointer with *
            node.handle = m_audioManager.engine.play(*node.wav, 0.0);
        }
    }
}

void PowertrainAudioController::stop() {
    m_isPlaying = false;

    // Stop all active handles
    for (auto& node : m_audioManager.samples) {
        if (node.handle != 0) {
            m_audioManager.engine.fadeVolume(node.handle, 0.0f, 0.15f);

            m_audioManager.engine.scheduleStop(node.handle, 0.15f);

            node.handle = 0;
        }
    }
}

void PowertrainAudioController::handleCatastrophicFailure() {
    // 1. Trigger the "Snap" (High frequency peak)
    if (m_audioManager.samples.contains("snap")) {
        m_audioManager.engine.play(*m_audioManager.samples["snap"].wav);
    }

    // 2. Trigger the "Thrud" (Low frequency weight)
    if (m_audioManager.samples.contains("thud")) {
        SoLoud::handle h = m_audioManager.engine.play(*m_audioManager.samples["thud"].wav);
        m_audioManager.engine.setRelativePlaySpeed(h, 0.9f); // Slightly lower pitch for more "oomph"
    }

    // 3. Trigger the Debris (The "Gravel Drop")
    if (m_audioManager.samples.contains("debris")) {
        // Delay this slightly or play it at 1.0 volume to simulate parts hitting the floor
        m_audioManager.engine.play(*m_audioManager.samples["debris"].wav);
    }

    // 4. Kill the standard engine loops
    stop();
}
