#include "powertrain.h"

int revDownCounter = 1;
int instanceCounter = 0;

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
    this->m_pedalPercent = 0;
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
    m_revs = revs;
}

int Powertrain::getThrottle()
{
    return m_throttle;
}
void Powertrain::setThrottle(int throttle)
{
    m_throttle = throttle;
}

int Powertrain::getGear() {
    return m_gear;
}
void Powertrain::setGear(int gear) {
    m_gear = gear;
}

void Powertrain::Shift(int gear) {
    if (gear < m_gear && getRevs() > moneyShiftRevThreshold)
    {
        //[Money shift implementation trigger]
        qInfo() << "KABOOM (money shift)";
        setStarted(false);
        return;
    }
    else
    {
        setGear(gear);
    }
}

float Powertrain::getAcceleration() {
    return m_acceleration;
}

void Powertrain::setAcceleration(float acceleration) {
    m_acceleration = acceleration;
}

float Powertrain::getSpeed()
{
    return m_speed;
}

void Powertrain::setSpeed(float speed)
{
    m_speed = speed;
}

void Powertrain::setStarted(bool started)
{
    if (started == false && m_started == true)
    {
        setSpeed(0);
        setRevs(0);
        setAcceleration(0);
        setGear(defaultGear);
        setThrottle(0);
        setOutputPower(0);
        setOutputTorque(0);
    }
    if (started == true && m_started == false)
    {
        setSpeed(defaultSpeed);
        setRevs(idleRevs);
        setGear(defaultGear);
        setThrottle(gasPedalDeadZone);
    }
    m_started = started;

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
    int powerPosition = ((getRevs() - (getRevs() % 100)) / 100)-8;
    return powerCurve[powerPosition];
}

float Powertrain::getEngineTorque()
{
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

int Powertrain::getPedalPercent()
{
    return m_pedalPercent;
}

void Powertrain::setPedalPercent(int pedalPercent)
{
    m_pedalPercent = pedalPercent;
}



void Powertrain::everyRefresh(int pedalPercent)
{
    setPedalPercent(pedalPercent);

    if(m_started)
    {
        //[find a way to receive pedal inputs]!!!!!
        /*
        if(-key pressed)
            pedal percent += 1;
        */

        //================ Simulation
        /*if (instanceCounter <= 100)
        {
            setPedalPercent((instanceCounter - (instanceCounter % 2))/2);
        }
        else if (instanceCounter > 100 && instanceCounter <= 125)
        {
            setPedalPercent(50 + (instanceCounter - 100));

        }
        else if (instanceCounter > 125 && instanceCounter <= 174)
        {
            setPedalPercent(0);
        }
        else if (instanceCounter == 175)
        {
            Shift(2);
        }
        else if (instanceCounter > 175 && instanceCounter <= 200)
        {
            setPedalPercent(75);
        }
        else if (instanceCounter > 200 && instanceCounter <= 300)
        {
            setPedalPercent(80);
        }
        else if (instanceCounter > 300 && instanceCounter <= 500)
        {
            setPedalPercent(0);
        }*/

        //adjusts throttle opening from how much the pedal is pressed;
        if (getPedalPercent() <= gasPedalDeadZone && getPedalPercent() >= 0)
        {
            setThrottle(gasPedalDeadZone);
        }
        else if (getPedalPercent() > gasPedalDeadZone && getPedalPercent() < 101)
        {
            setThrottle(getPedalPercent());
        }
        else
        {
            //error handler
            qInfo() << "ERROR: pedal percent out of range";
        }

        //sets engine revs every tick
        //rev modification trigger here
        revSetter();
        if (getRevs() > redLine)
        {
            setRedLineTickCounter(getRedLineTickCounter() + 1);
            if (getRedLineTickCounter() > redLineTickLimit)
            {
                qInfo() << "KABOOM! (moteur explose)";
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

        if (getGear() != 0) {
            //sets output power and torque at the axle.
            axlePower = (getEnginePower() / getGearRatio());
            axleTorque = (getEngineTorque() * getGearRatio());

            //takes away all the theoredical losses into account
            axlePower *= drivetrainEfficiency;
            axleTorque *= drivetrainEfficiency;
        }

        setOutputPower(axlePower);
        setOutputTorque(axleTorque);
        float force = (getOutputTorque() / tireDiameter); //power in feet
        float acceleration = (force / carWeight) * 9.8; //returns acceleration in m/s^2 (4.44... converts lbs to N)

        setAcceleration(acceleration);

        float vf = (getSpeed() + (getAcceleration()/refreshRate)*3.6);

        if (getGear() != 0) {
            float maxSpeed = ((getRevs() / getGearRatio()) * (tireDiameter * 12 * M_PI) * 0.00152);
            if (vf > maxSpeed) {
                vf = maxSpeed;
            }
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
        qInfo() << "Moteur arrêté! Veuillez démarrer afin de continuer!";
    }

    /*qInfo() << "======================================================="
              << "Voici les informations actuelles du véhicule: " << instanceCounter
              << "- Pourcentage de la pédal (touches W+ et S-): " << getPedalPercent()
              << "- Pourcentage du throttle (= pédale, min 5):" << getThrottle()
              << "- RPMs du moteur (contrôlé à l'interne):    " << getRevs()
              << "- Vitesse (transmission) (touches 1 à 6):   " << getGear()
              << "- Vitesse (km/h) (contrôlé à l'interne):    " << getSpeed();*/
    instanceCounter++;


}

//TODO:
// Find how to implement this by setting new revs and potentially adding more math.
//Figure out realistic way for revs to behave and change
void Powertrain::revSetter()
{
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
    if(getRevs() > revTarget+100)
    {
        if(revDownCounter == (refreshRate/20))
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
    else if(getRevs() < revTarget+100)
    {
        //rev acceleration will be reduced based on which gear you're in, and how open the throttle is
        float revGearResistance;

        if (getGear() == 0) {
            revGearResistance = 2.5;
        } else {
            revGearResistance = 1 - (1 / getGearRatio());
        }

        /*std::cout << "getGearRatio:     " << getGearRatio() << std::endl
            << "revGearResistance:  " << revGearResistance << std::endl;*/

        newRevs = getRevs() + (revGearResistance*getThrottle());
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

    float power = getEnginePower();
    float torque = getEngineTorque();
}

//returns the gear ratio depending on what gear the car is in.
float Powertrain::getGearRatio()
{
    float returnGearValue = gearRatioFinalDrive;
    if (getGear() == 1)
    {
        return (returnGearValue * gearRatio1);
    }
    else if (getGear() == 2)
    {
        return (returnGearValue * gearRatio2);
    }
    else if (getGear() == 3)
    {
        return (returnGearValue * gearRatio3);
    }
    else if (getGear() == 4)
    {
        return (returnGearValue * gearRatio4);
    }
    else if (getGear() == 5)
    {
        return (returnGearValue * gearRatio5);
    }
    else if (getGear() == 6)
    {
        return (returnGearValue * gearRatio6);
    }
    return 0;
}
