#pragma once
#ifndef POWERTRAIN
#define POWERTRAIN

/*
    changed in this file:
    -set refreshrate from 60 to 100
    -removed gas pedal percent from call stack in the everyRefresh function
    -added pedalPercent as an int in the everyRefresh function, and changed how it interacts with it to take keyboard inputs
    -added a section that shows all the things I want in the console while running
    -added global locale to make cout print actual symbols (maybe)
    -added refreshTrigger function (sanity)

    -added const to global variables to stop it from breaking everything
    -changed redlinetickcounter to a class member so it could be changed
    -rebuilding rev setter sytem entirely
    -set initial gear to 1 (dum dum)
    -set initial speed to 1 so it would start moving
    -added engine started or not
    -added money shift and over-rev explosions

*/

#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>


int const refreshRate = 100; //number of executions per second
int const idleRevs = 800;   //sets the idle (do not set lower than 800, the powercurve is not defined past that)
int const moneyShiftRevThreshold = 6000;  //sets the rev limit before shifting will break the gearbox
int const redLine = 7000;   //sets the redline
int const maxRevs = 8000;   //sets the max number of revs (do not go higher than 8000, the powercurve is not defined past that)
int const redLineTimeLimit = 1; //sets the number of seconds past redline before engine breaks
int const redLineTickLimit = (refreshRate*redLineTimeLimit); //sets the number of ticks past redline before engine breaks
int const gasPedalDeadZone = 5; //used to adjust throttle % for idle, but also means the beginning of the pedal is a dead zone (does nothing)
float const revAccelerationConstant = 0.2; //set arbitrarily, change this to affect how fast the revs change
float const drivetrainEfficiency = 0.82; //equivalent to %, refers to loss of power through the powertrain.
float const tireDiameter = 1.25; //set in feet (cuz ft-lb so force is easily found in lbs)
float const carWeight = 2815; //in pounds, based on Subaru BRZ 2022
int const defaultGear = 1; //sets the gear by default
float const gearRatio1 = 3.63; //gear ratio for first gear
float const gearRatio2 = 2.19; //' '
float const gearRatio3 = 1.54; //' '
float const gearRatio4 = 1.21; //' '
float const gearRatio5 = 1.00; //' '
float const gearRatio6 = 0.77; //' '
float const gearRatioFinalDrive = 4.20; //sets the final drive axle ratio
int const defaultSpeed = 0; //sets the starting speed, set to 1 for debug

class Powertrain
{
public:

    Powertrain();

    int getGear();
    void setGear(int gear);
    int getRevs();
    void setRevs(int revs);
    int getThrottle();
    void setThrottle(int throttle);

    float getAcceleration();
    void setAcceleration(float acceleration);
    float getSpeed();
    void setSpeed(float speed);

    void setStarted(bool started);
    bool getStarted();

    int getRedLineTickCounter();
    void setRedLineTickCounter(int redLineTickCounter);


    void Shift(int gear);            //used to change gears, with checks for money shifts etc.
    void everyRefresh();   //called every 'tick' to adjust the speed dynamically
    void revSetter();                   //sets the revs every tick, adjusting it based on throttle opening.
    float getGearRatio();               //returns the gear ratio of the gearbox;
    float getEnginePower();             //returns horsepower depending on the RPM.
    float getEngineTorque();            //return torque, calculated from horsepower.
    float getOutputPower();
    float getOutputTorque();
    void setOutputPower(float power);
    void setOutputTorque(float torque);


private:
    int m_gear;
    int m_revs;
    int m_throttle;
    float m_speed;
    float m_acceleration;
    float m_outputPower;    //IMPORTANT!!!! this is the power at the AXLE, NOT at the wheels
    float m_outputTorque;   //IMPORTANT!!!! this is the torque at the AXLE, NOT at the wheels
    int m_redLineTickCounter;
    bool m_started;
};
Powertrain refreshTrigger();


const float powerCurve[73] = {
    61.52, 71.38, 83.8, 95.97, 105.34, 114.59, 133.07, 152.63, 156.61, 166.09, 171.17, 176.94,
    184.49, 193.2, 201.91, 210.7, 219.63, 228.57, 239.0, 250.61, 263.58, 278.51, 293.44, 307.07,
    318.68, 329.22, 336.37, 343.51, 351.61, 360.9, 369.98, 378.54, 387.19, 396.12, 410.26, 425.36,
    436.58, 446.08, 454.49, 460.08, 467.71, 477.39, 482.09, 486.53, 490.63, 494.02, 496.8, 499.59,
    501.65, 507.23, 515.65, 515.9, 514.6, 513.29, 511.32, 508.32, 505.32, 502.44, 501.48, 500.51,
    498.53, 495.43, 492.64, 488.12, 485.76, 481.2, 478.0, 473.22, 465.81, 458.37, 451.3, 442.91, 433.82
};

//=========Power Curve=========
/*
Pos     RPM		    HP
0       800	        61.52
1	    900	        71.38
2	    1000	    83.8
3	    1100	    95.97
4	    1200	    105.34
5	    1300	    114.59
6	    1400	    133.07
7	    1500	    152.63
8	    1600	    156.61
9	    1700	    166.09
10	    1800	    171.17
11	    1900	    176.94
12	    2000	    184.49
13	    2100	    193.2
14	    2200	    201.91
15	    2300	    210.7
16	    2400	    219.63
17	    2500	    228.57
18	    2600	    239.0
19	    2700	    250.61
20	    2800	    263.58
21	    2900	    278.51
22	    3000	    293.44
23	    3100	    307.07
24	    3200	    318.68
25	    3300	    329.22
26	    3400	    336.37
27	    3500	    343.51
28	    3600	    351.61
29	    3700	    360.9
30	    3800	    369.98
31	    3900	    378.54
32	    4000	    387.19
33	    4100	    396.12
34	    4200	    410.26
35	    4300	    425.36
36	    4400	    436.58
37	    4500	    446.08
38	    4600	    454.49
39	    4700	    460.08
40	    4800	    467.71
41	    4900	    477.39
42	    5000	    482.09
43	    5100	    486.53
44	    5200	    490.63
45	    5300	    494.02
46	    5400	    496.8
47	    5500	    499.59
48	    5600	    501.65
49	    5700	    507.23
50	    5800	    515.65
51	    5900	    515.9
52	    6000	    514.6
53	    6100	    513.29
54	    6200	    511.32
55	    6300	    508.32
56	    6400	    505.32
57	    6500	    502.44
58	    6600	    501.48
59	    6700	    500.51
60	    6800	    498.53
61	    6900	    495.43
62	    7000	    492.64
63	    7100	    488.12
64	    7200	    485.76
65	    7300	    481.2
66	    7400	    478.0
67	    7500	    473.22
68	    7600	    465.81
69	    7700	    458.37
70	    7800	    451.3
71	    7900	    442.91
72	    8000	    433.82
*/


/*
/////TO DO:
-Implement engine's limits, strength,  etc. -> learn more for this...
-Implement driver errors (money shifting[~~]), over revving[~~], etc.)
-Implement engine brake
-Implement ACTUAL brakes
-Implement gravity resistance while uphill and acceleration on downhill
-Implement resistance while turning


-Convert throttle into engine revs ->
    https://www.physicsforums.com/threads/how-does-the-throttle-affect-the-rpm-of-an-engine.832029/
    https://ijcrt.org/papers/IJCRT1892376.pdf
    https://github.com/CrankcaseAudio/REVSimulators
    https://www.omnicalculator.com/everyday-life/rpm
    https://www.reddit.com/r/cars/comments/z7pmdp/making_a_game_about_cars_how_could_i_determine_a/
    https://github.com/Engine-Simulator/engine-sim-community-edition/blob/master/tutorial/01_getting_started/01_getting_started.md


    Figured it out: adjust revs to match «percentage» of throttle, while having a non-linear delta,
    and have the variation of the pedal be an acceleration of revs (positive or negative)
    \_> possible to do, lot of work though.

    **THINK it's done.


/////fun stuff
-Implement reverse
-Implement sound based on revs


/////Done:
-Implement output acceleration    <-working on this.
    https://physics.stackexchange.com/questions/29719/finding-acceleration-for-a-car-after-finding-torque
    F = T/diameter (F -> force and T -> Torque)
    using F = ma, a = F/m (m = weight of the car)
-Implement speed of the car
-Implement gear ratios
    https://www.transmissiondigest.com/math-for-mechanics/
    https://industrialgearboxsupply.com/blog/how-to-calculate-gearbox-torque-step-by-step
    T_out = T_engine * (AxleRatio*GearRatio) * Efficiency
    P_out = P_engine * (1/(AxleRatio*GearRatio)) * Efficiency
-Interpret gas pedal into throttle -> throttle map (can be linear in % of pedal to % of throttle + enough for idle)
-Code can use gear ratios
-Implement a power curve to convert revs into HP -> use power curve [<- working here]
    john deer: (115 @2400) https://partslookup.frontierpower.com/vendorcd/deereeascd/Acrobat/misc/engapp/PCurvesInd/4045HFC04_E.pdf
    BRZ maybe? https://www.br-performance.be/en-be/chiptuning/1-cars/51-subaru/5849-brz/11781-2022/14175-2-4-d-4s/
    decent tool: http://www.leany.com/DesktopDyno/Page2.html
    YES!!!!: https://maximumtwist.com/dyno-database
    \_> now to pick one: https://maximumtwist.com/dyno-runs/d1e7aa5c-7404-4b78-8ee3-11c04d9649d6
    \_> combined with the john deer (+20 to 60) for the lower RPMs
    \_> stats written here: https://usherbrooke-my.sharepoint.com/:x:/r/personal/gues2290_usherbrooke_ca/_layouts/15/Doc.aspx?sourcedoc=%7BDFBC399E-54BD-4796-903D-F9C9F7EE41BC%7D&file=Book.xlsx&action=editNew&mobileredirect=true&wdOrigin=WAC.EXCEL.HOME-BUTTON%2CAPPHOME-WEB.BANNER.NEWBLANK&wdPreviousSession=4557574e-d15e-f15f-3701-d50754d59a97&wdPreviousSessionSrc=Wac&ct=1770933495884







=============================================
Notes on math to calculate torque, power, etc:
    yt vid: https://youtu.be/S23AOeGP_Os

    ***T is tau
    ***w is omega

    Horsepower:
        1 Hp = 746W = 746J/1s = 0.746kW = 550ft*lbs/s = 33,000ft*lbs/min

        Power = Work/Time = (Force * displacement)/time
        Work = F * displacement -> N * m - > lbs*ft

        1 lb = 4.45N and 1 m = 3.2808 ft


        1J = 1N * 1m = 0.7373ft*lbs

    Torque:
        T = F*[gamma (rotation angulaire)] = F*rayon
        F -> N et [gamma] -> m. Donc peut aussi être lbs*ft

    ---------------------------------------------
        P = work/time = (F * d)/t
        d = V*t (V-> Linear velocity)
        d/t = V
        P = F*V

        P = T * w <-(tau * omega)
        T = F * r <-(force * radius)
        V = w * r <-(angular velocity * radius)

        T/r = F
        P = T/r * (w*r) = T*w

        P -> Watts, T -> N*m, w -> rad/s

    **conversion starts here
        1 rev/min * (2*pi)rad/1rev * 1min/60s = w
        \_> therefore: (RPM * 2Pi)/60 = w

        1000W/1 * 1Hp/746W
        \_> therefore: P(in W)/746 = Hp -> P(in watts) = 746Hp

        100N*m/1 * 0.7373ft*lbs/1N*m
        \_> therefore: T(in N*m) * 0.7373 = T(in ft*lbs) -> T(in N*m) = T(in ft*lbs)/0.7373

        P = T * w -> P(in W) = T(in N*m) * w(in rad/s)
        \_> using formulas: 746Hp = T(in ft*lbs)/0.7373 * 2pi*RPM/60
        \_> multiply both sides by (1/746): 1Hp = T(in ft*lbs)/0.7373 * 2pi*RPM/60 * (1/746)
        \_> simplifying: Hp = T*RPM*2pi/33,000
        \_> simplifying: Hp = T*RPM/5252 -> relation between Hp, torque and RPMs

    ---------------------------------------------
    exercises:

        1- Hp = ? for T=500 ft*lbs @2500RPM?
            \_> 500*2500/5252 = 238.004569... -> 238Hp

        2- @? RPM will a 133.3Hp engine generate a torque of 200ft*lbs?
            \_> 5252*133.3Hp/200 = 3500

        3- Torque = ? for a 500Hp engine @3000RPM?
            \_> 875.333... ft*lbs


=============================================
Gear ratios (based on 2022 Subaru BRZ's gearbox):

Neutral:                [none]      -> if we implement this
1st:                    3.63:1
2nd:                    2.19:1
3rd:                    1.54:1
4th:                    1.21:1
5th:                    1.00:1
6th:                    0.77:1
rvrs:                   3.44:1      -> if we implement this
final drive axle ratio: 4.10:1      -> if we implement this
*/


#endif //POWERTRAIN