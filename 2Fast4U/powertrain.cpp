#include "powertrain.h"

namespace {
    constexpr float pi = 3.14159265358979323846f;
    constexpr int minGear = 0;
    constexpr int maxGear = 6;

    int revDownCounter = 1;
    int instanceCounter = 0;

    int clampPercent(int value)
    {
        return std::clamp(value, 0, 100);
    }

    float gearRatioForGear(int gear)
    {
        float returnGearValue = gearRatioFinalDrive;

        switch (gear) {
        case 1: return returnGearValue * gearRatio1;
        case 2: return returnGearValue * gearRatio2;
        case 3: return returnGearValue * gearRatio3;
        case 4: return returnGearValue * gearRatio4;
        case 5: return returnGearValue * gearRatio5;
        case 6: return returnGearValue * gearRatio6;
        default: return 0.0f;
        }
    }

    int rpmForSpeedAndGear(float speed, int gear)
    {
        const float gearRatio = gearRatioForGear(gear);
        if (gearRatio <= 0.0f || !std::isfinite(speed)) {
            return 0;
        }

        const float tireCircumferenceInches = tireDiameter * 12.0f * pi;
        return static_cast<int>((gearRatio * speed) / (0.00152f * tireCircumferenceInches));
    }

    float slopeForce(float angleDegrees)
    {
        if (!std::isfinite(angleDegrees)) {
            return 0.0f;
        }

        const float angleRadians = angleDegrees * pi / 180.0f;
        return carWeight * std::sin(angleRadians);
    }

    float aerodynamicDragForce(float speedKmh)
    {
        if (!std::isfinite(speedKmh) || speedKmh <= 0.0f) {
            return 0.0f;
        }

        const float speedMs = speedKmh / 3.6f;
        const float dragNewtons = 0.5f * airDensity * dragCoefficient * frontalArea * speedMs * speedMs;
        return dragNewtons / newtonsPerPoundForce;
    }

    float rollingResistanceForce(float terrainFriction, float angleDegrees)
    {
        const float safeFriction = std::max(1.0f, std::isfinite(terrainFriction) ? terrainFriction : 1.0f);
        const float angleRadians = std::isfinite(angleDegrees) ? angleDegrees * pi / 180.0f : 0.0f;
        const float normalForce = carWeight * std::max(0.0f, std::cos(angleRadians));
        return rollingResistanceCoefficient * safeFriction * normalForce;
    }

    float engineBrakingForce(int revs, int gear)
    {
        const float gearRatio = gearRatioForGear(gear);
        if (gearRatio <= 0.0f || revs <= idleRevs) {
            return 0.0f;
        }

        const float rpmFactor = std::clamp((revs - idleRevs) / static_cast<float>(redLine - idleRevs), 0.25f, 1.0f);
        const float axleTorque = engineBrakingTorque * rpmFactor * gearRatio * drivetrainEfficiency;
        return axleTorque / (tireDiameter * 0.5f);
    }

    float tractionLimitedDriveForce(float axleTorque)
    {
        if (axleTorque <= 0.0f) {
            return 0.0f;
        }

        const float tireRadiusFeet = tireDiameter * 0.5f;
        const float requestedForce = axleTorque / tireRadiusFeet;
        const float maxDriveForce = carWeight * drivenWheelLoadRatio * tireGripCoefficient;
        return std::min(requestedForce, maxDriveForce);
    }

    float throttleLoad(int gasPedalPercent)
    {
        return std::clamp((gasPedalPercent - gasPedalDeadZone) / static_cast<float>(100 - gasPedalDeadZone), 0.0f, 1.0f);
    }

    float clutchSlipAllowance(int gear)
    {
        switch (gear) {
        case 1: return 1.0f;
        case 2: return 0.35f;
        case 3: return 0.12f;
        default: return 0.0f;
        }
    }

    float idleSpeedForGear(int gear)
    {
        const float gearRatio = gearRatioForGear(gear);
        if (gearRatio <= 0.0f) {
            return 0.0f;
        }

        return (idleRevs / gearRatio) * (tireDiameter * 12.0f * pi) * 0.00152f;
    }

    float launchTorqueMultiplier(float speedKmh, int gear, int gasPedalPercent)
    {
        const float throttleFactor = throttleLoad(gasPedalPercent);
        if (gear <= 0 || throttleFactor <= 0.0f) {
            return 0.0f;
        }

        const float idleGearSpeed = std::max(1.0f, idleSpeedForGear(gear));
        const float rollingFactor = std::clamp(speedKmh / idleGearSpeed, 0.0f, 1.0f);
        const float launchSlip = clutchSlipAllowance(gear) * throttleFactor;

        return std::max(rollingFactor, launchSlip);
    }

    float surfaceDriveGripMultiplier(float terrainFriction)
    {
        if (!std::isfinite(terrainFriction) || terrainFriction <= 1.0f) {
            return 1.0f;
        }

        return std::clamp(1.0f / (1.0f + ((terrainFriction - 1.0f) * 0.08f)), 0.25f, 1.0f);
    }
}

Powertrain::Powertrain() {
    this->m_revs = idleRevs;
    this->m_gear = defaultGear; //-> 0 if we implement neutral. Otherwise, set to 1;
    this->m_throttle = gasPedalDeadZone;
    this->m_speed = defaultSpeed;
    this->m_acceleration = 0;
    this->m_outputPower = 0;
    this->m_outputTorque = 0;
    this->m_redLineTickCounter = 0;
    this->m_started = true;
    this->m_isMotorExploded = false;
    this->m_gasPedalPercent = 0;
    this->m_brakePedalPercent = 0;
}
/*Powertrain::Powertrain(int revs, int gear, int throttle, int speed) {
    std::cout << "birth" << std::endl;
    this->m_revs = idleRevs;
    this->m_gear = defaultGear; //-> 0 if we implement neutral. Otherwise, set to 1;
    this->m_throttle = gasPedalDeadZone;
    this->m_speed = 0;
    this->m_acceleration = 0;
    this->m_outputPower = 0;
    this->m_outputTorque = 0;
    this->m_redLineTickCounter = 0;
    this->m_started = true;
}*/

int Powertrain::getRevs() {
    return m_revs;
}
void Powertrain::setRevs(int revs) {
    m_revs = std::clamp(revs, 0, maxRevs);
}

int Powertrain::getThrottle()
{
    return m_throttle;
}
void Powertrain::setThrottle(int throttle)
{
    m_throttle = clampPercent(throttle);
}

int Powertrain::getGear() {
    return m_gear;
}
void Powertrain::setGear(int gear) {
    m_gear = std::clamp(gear, minGear, maxGear);
}

void Powertrain::Shift(int gear) {
    const int targetGear = std::clamp(gear, minGear, maxGear);
    if (targetGear == m_gear) {
        return;
    }

    if (targetGear == 0) {
        setGear(targetGear);
        return;
    }

    const int projectedRevs = rpmForSpeedAndGear(getSpeed(), targetGear);
    const bool isDownshift = targetGear < m_gear || m_gear == 0;

    if (isDownshift && projectedRevs > moneyShiftRevThreshold)
    {
        //[Money shift implementation trigger]
        qInfo() << "KABOOM (money shift)";
        explodeMotor();
        return;
    }

    setGear(targetGear);
    if (projectedRevs > getRevs())
    {
        setRevs(projectedRevs);
    }
}

float Powertrain::getAcceleration() {
    return m_acceleration;
}

void Powertrain::setAcceleration(float acceleration) {
    if (std::isfinite(acceleration)) {
        m_acceleration = acceleration;
    }
    else {
        m_acceleration = 0.0f;
    }
}

float Powertrain::getSpeed()
{
    return m_speed;
}

void Powertrain::setSpeed(float speed)
{
    if (std::isfinite(speed)) {
        m_speed = std::max(0.0f, speed);
    }
    else {
        m_speed = 0.0f;
    }
}

void Powertrain::setStarted(bool started)
{
    if (m_isMotorExploded) {
        m_started = false;
        return;
    }

    if (started && !m_started) {
        if (getRevs() < idleRevs) {
            setRevs(idleRevs);
        }
        setThrottle(gasPedalDeadZone);
    }

    if (!started) {
        setThrottle(0);
        setAcceleration(0);
        setOutputPower(0);
        setOutputTorque(0);
        setRedLineTickCounter(0);
    }

    m_started = started;
}

void Powertrain::explodeMotor() {
    m_isMotorExploded = true;
    setSpeed(0);
    setRevs(0);
    setAcceleration(0);
    setThrottle(0);
    setOutputPower(0);
    setOutputTorque(0);
    m_started = false;
}

void Powertrain::reset() {
    m_isMotorExploded = false;
    m_started = false;
    setSpeed(0);
    setRevs(idleRevs);
    setAcceleration(0);
    setThrottle(gasPedalDeadZone);
    setOutputPower(0);
    setOutputTorque(0);
    setGear(defaultGear);
    setGasPedalPercent(0);
    setBrakePedalPercent(0);
    setRedLineTickCounter(0);
    revDownCounter = 1;
}

bool Powertrain::getStarted()
{
    return m_started;
}

int Powertrain::getRedLineTickCounter()
{
    return m_redLineTickCounter;
}

void Powertrain::setRedLineTickCounter(int redLineTickCounter)
{
    m_redLineTickCounter = redLineTickCounter;
}

float Powertrain::getEnginePower()
{
    if (getRevs() <= 0 || m_isMotorExploded) {
        return 0.0f;
    }

    const int safeRevs = std::clamp(getRevs(), idleRevs, maxRevs);
    int powerPosition = ((safeRevs - (safeRevs % 100)) / 100) - 8;
    powerPosition = std::clamp(powerPosition, 0, static_cast<int>((sizeof(powerCurve) / sizeof(powerCurve[0])) - 1));
    return powerCurve[powerPosition];
}

float Powertrain::getEngineTorque()
{
    if (getRevs() <= 0 || m_isMotorExploded) {
        return 0.0f;
    }

    return (5252 * getEnginePower() / getRevs());
}

float Powertrain::getOutputPower()
{
    return m_outputPower;
}

float Powertrain::getOutputTorque()
{
    return m_outputTorque;
}

void Powertrain::setOutputPower(float power)
{
    m_outputPower = power;
}

void Powertrain::setOutputTorque(float torque)
{
    m_outputTorque = torque;
}

int Powertrain::getGasPedalPercent()
{
    return m_gasPedalPercent;
}

void Powertrain::setGasPedalPercent(int gasPedalPercent)
{
    m_gasPedalPercent = clampPercent(gasPedalPercent);
}

int Powertrain::getBrakePedalPercent()
{
    return m_brakePedalPercent;
}

void Powertrain::setBrakePedalPercent(int brakePedalPercent)
{
    m_brakePedalPercent = clampPercent(brakePedalPercent);
}

void Powertrain::everyRefresh(int gasPedalPercent, int brakePedalPercent, float angle, float terrainFriction, bool clutchEngaged)
{
    setGasPedalPercent(gasPedalPercent);
    if (brakePedalPercent > brakePedalDeadZone)
    {
        setBrakePedalPercent(brakePedalPercent);
    }
    else
    {
        setBrakePedalPercent(0);
    }
    if (m_started)
    {
        const bool drivetrainConnected = clutchEngaged && getGear() != 0;
        //[find a way to receive pedal inputs]!!!!!
        /*
        if(-key pressed)
            pedal percent += 1;
        */

        //================ Simulation
        /*if (instanceCounter <= 100)
        {
            setGasPedalPercent((instanceCounter - (instanceCounter % 2))/2);
        }
        else if (instanceCounter > 100 && instanceCounter <= 125)
        {
            setGasPedalPercent(50 + (instanceCounter - 100));

        }
        else if (instanceCounter > 125 && instanceCounter <= 174)
        {
            setGasPedalPercent(0);
        }
        else if (instanceCounter == 175)
        {
            Shift(2);
        }
        else if (instanceCounter > 175 && instanceCounter <= 200)
        {
            setGasPedalPercent(75);
        }
        else if (instanceCounter > 200 && instanceCounter <= 300)
        {
            setGasPedalPercent(80);
        }
        else if (instanceCounter > 300 && instanceCounter <= 500)
        {
            setGasPedalPercent(0);
        }*/



        //adjusts throttle opening from how much the pedal is pressed;
        if (getGasPedalPercent() <= gasPedalDeadZone && getGasPedalPercent() >= 0)
        {
            setThrottle(gasPedalDeadZone);
        }
        else if (getGasPedalPercent() > gasPedalDeadZone && getGasPedalPercent() < 101)
        {
            setThrottle(getGasPedalPercent());
        }
        else
        {
            //error handler
            qInfo() << "ERROR: pedal percent out of range";
        }

        //sets engine revs every tick
        //rev modification trigger here
        revSetter(drivetrainConnected);
        if (drivetrainConnected && getSpeed() > 1.0f) {
            const int wheelLockedRevs = std::max(idleRevs, rpmForSpeedAndGear(getSpeed(), getGear()));
            if (wheelLockedRevs > getRevs()) {
                setRevs(wheelLockedRevs);
            }
        }

        if (getRevs() > redLine)
        {
            setRedLineTickCounter(getRedLineTickCounter() + 1);
            if (getRedLineTickCounter() > redLineTickLimit)
            {
                qInfo() << "KABOOM! (moteur explose)";
                explodeMotor();
                setStarted(false);
                return;
            }
        }
        else if (getRevs() < redLine && redLine > 0)
        {
            setRedLineTickCounter(0);
        }

        float axlePower = 0;
        float axleTorque = 0;

        if (drivetrainConnected) {
            const float throttleFactor = throttleLoad(getGasPedalPercent());
            const float launchMultiplier = launchTorqueMultiplier(getSpeed(), getGear(), getGasPedalPercent());

            //sets output power and torque at the axle.
            axlePower = (getEnginePower() * throttleFactor * launchMultiplier) / getGearRatio();
            axleTorque = getEngineTorque() * throttleFactor * launchMultiplier * getGearRatio();

            //takes away all the theoredical losses into account
            axlePower *= drivetrainEfficiency;
            axleTorque *= drivetrainEfficiency;
        }

        setOutputPower(axlePower);
        setOutputTorque(axleTorque);
        float force = tractionLimitedDriveForce(getOutputTorque()) * surfaceDriveGripMultiplier(terrainFriction);
        force -= slopeForce(angle);

        if (getSpeed() > 0.0f || force > 0.0f) {
            force -= rollingResistanceForce(terrainFriction, angle);
            force -= aerodynamicDragForce(getSpeed());
        }

        if (drivetrainConnected && getGasPedalPercent() <= gasPedalDeadZone && getSpeed() > 0.5f) {
            force -= engineBrakingForce(getRevs(), getGear());
        }

        float acceleration = (force / carWeight) * gravitationnalAcceleration; //returns acceleration in m/s^2 (4.44... converts lbs to N)
        setAcceleration(acceleration);

        //modifies acceleration, so it takes effect here when we are breaking.
        if (getBrakePedalPercent() > 0)
        {
            braking();
        }
        float vf = (getSpeed() + (getAcceleration() / refreshRate) * 3.6);

        if (drivetrainConnected) {
            float maxSpeed = ((getRevs() / getGearRatio()) * (tireDiameter * 12 * pi) * 0.00152f);
            if (vf > maxSpeed) {
                //std::cout << "limiting factor is maxSpeed: " << maxSpeed << std::endl;
                vf = maxSpeed;
            }
        }
        if (vf < 0) // stops the speed from being negative due to eccessive breaking force
        {
            vf = 0;
        }
        setSpeed(vf); //finally, sets the speed at the end of that tick.
        /*std::cout << "-------------------------------------------------------" << std::endl
            << "SECTION DEBUG VITESSE:  " << std::endl
            << "crank power:            " << getEnginePower() << std::endl
            << "crank torque:           " << getEngineTorque() << std::endl
            << "gear ratio:             " << getGearRatio() << std::endl
            << "axlePower:              " << axlePower << std::endl
            << "axleTorque:             " << axleTorque << std::endl
            << "force:                  " << force << std::endl
            << "getAcceleration:        " << getAcceleration() << std::endl
            << "maxSpeed:               " << maxSpeed << std::endl
            << "vf:                     " << vf << std::endl;*/
    }
    else
    {
        setAcceleration(0);
        setOutputPower(0);
        setOutputTorque(0);
        //qInfo() << "Moteur arrêté! Veuillez démarrer afin de continuer!";
    }

    /*std::cout
        << "======================================================="
        << "Voici les informations actuelles du véhicule:   " << instanceCounter << std::endl
        << "- Pourcentage de la pédale (touches W+ et S-):  " << getGasPedalPercent() << std::endl
        << "- Pourcentage du throttle (= pédale, min 5):    " << getThrottle() << std::endl
        << "- RPMs du moteur (contrôlé à l'interne):        " << getRevs() << std::endl
        << "- Vitesse (transmission) (touches 1 à 6):       " << getGear() << std::endl
        << "- Vitesse (km/h) (contrôlé à l'interne):        " << getSpeed() << std::endl
        << "-------------------------------------------------------" << std::endl
        << "- Pourcentage du frein(touches E+ et D-):       " << getBrakePedalPercent() << std::endl
        << "- Angle d'inclinaison:(touches F+ et G-):       " << angle << std::endl;*/
    instanceCounter++;


}

//tp
void Powertrain::braking()
{
    float brakePedalForce = brakePedalOffset + getBrakePedalPercent();
    float caliperPressure = ((brakePedalForce * brakePedalRatio)) / masterCylinderAreaOfSystem; //(result in PSI)
    float clampingForce = caliperPressure * brakePadArea; //-> ~7200 for 70 pounds of pedal (+100 from booster)
    float brakeTorque = clampingForce * frictionCoefficient; //in ft-lbs most likely?
    float brakeTorqueN = 1.35582 * brakeTorque; //converts ft-lbs to N*m
    //Done: get speedratio between wheel and brakes...??? -> ignored, done * 1 since we don't use front vs rear wheel braking effort (hoping that's what it is)
    float brakeForce = 4 * (brakeTorqueN * brakeSpeedRatio) / tireRadiusM; //force in N, *4 because 4 wheels
    float deceleration = brakeForce / (carWeight * gravitationnalAcceleration); //in m/s^2
    deceleration = std::clamp(deceleration, 0.0f, maxBrakeDeceleration);
    float newAccel = getAcceleration() - deceleration;
    setAcceleration(newAccel);
}

//TODO:
// Find how to implement this by setting new revs and potentially adding more math.
//Figure out realistic way for revs to behave and change
void Powertrain::revSetter(bool drivetrainConnected)
{
    if (getRevs() < idleRevs) {
        setRevs(idleRevs);
    }

    int revTarget = 0;
    if (getThrottle() == gasPedalDeadZone)
    {
        revTarget = 800;
        //qInfo() << "Idle revving!!!!";
    }
    else
    {//will always be from 5 to 100, cannot be over or under

        int revInstance = (maxRevs - idleRevs) / 100;
        int revHelper = revInstance * getThrottle();
        revTarget = idleRevs + (revHelper - (revHelper % 100));
    }
    /*std::cout << "----------------------------" << std::endl
            << "REV DEBUG SECTION:  " << std::endl
            << "getThrottle:    " << getThrottle() << std::endl
            << "revTarget:        " << revTarget << std::endl;*/

            //^seems to work decent, now to make it increase towards that at a good rate
    int newRevs = 0;
    if (getRevs() > revTarget + 100)
    {
        if (revDownCounter == (refreshRate / 20))
        {
            newRevs = getRevs() - 100;
            revDownCounter = 1;
        }
        else
        {
            newRevs = getRevs();
            revDownCounter++;
        }
    }
    else if (getRevs() < revTarget - 100)
    {
        //rev acceleration will be reduced based on which gear you're in, and how open the throttle is
        float revGearResistance;

        if (!drivetrainConnected || getGear() == 0) {
            revGearResistance = 2.5;
        }
        else {
            revGearResistance = 1 - (1 / getGearRatio());
        }

        /*std::cout << "getGearRatio:     " << getGearRatio() << std::endl
            << "revGearResistance:  " << revGearResistance << std::endl;*/

        newRevs = getRevs() + (revGearResistance * getThrottle());
        /*std::cout
            << "getThrottle:        " << getThrottle() << std::endl
            << "newRevs:            " << newRevs << std::endl
        << "------------------------------" << std::endl;*/
    }
    else
    {
        newRevs = getRevs();
    }
    setRevs(newRevs);
}

//returns the gear ratio depending on what gear the car is in.
float Powertrain::getGearRatio()
{
    return gearRatioForGear(getGear());
}
