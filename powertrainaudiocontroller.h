#ifndef POWERTRAINAUDIOCONTROLLER_H
#define POWERTRAINAUDIOCONTROLLER_H

#include "powertrain.h"
#include "audiomanager.h"
#include <algorithm>
#include <cmath>

class PowertrainAudioController
{
private:
    Powertrain* m_powertrainPtr;
    AudioManager m_audioManager;

    double m_softLimiter;
    double m_limiter;
    double m_rpmPitchFactor;

    bool m_isPlaying = true;

public:
    PowertrainAudioController(Powertrain* powertrain);

    ~PowertrainAudioController();

    void initSounds();
    void update();
    void handleCatastrophicFailure();

    void start();
    void stop();
};

#endif // POWERTRAINAUDIOCONTROLLER_H
