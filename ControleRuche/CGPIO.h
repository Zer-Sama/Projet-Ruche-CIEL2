///////////////////////////////////////////////////////////
//  CGPIO.h
//  Implementation of the Class CGPIO
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#if !defined(EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_)
#define EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_

#include <cstring>
#include <hx711/common.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <algorithm>
#include <csignal>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#define MAX_TIMINGS 85
#define NBITS 40
#define BAD_VALUE 999

using namespace std;
//using namespace HX711;

class CGpio
{

public:
    CGpio();
    virtual ~CGpio();

    pair<string, string> Lire_GPS();
    double Lire_Masse();
    void Lire_DHT22();
    double valDHT22[2];


private:
    int BrocheGPIO;
    

    class DhtSensor {
    public:
        DhtSensor(int pin);
        void read();
        void print();
        static void printSignalTitle();
        void printSignal();

        float m_temperature{ BAD_VALUE };
        float m_humidity{ BAD_VALUE };

    private:
        int m_pin;
        int m_signalStateDurations[NBITS];
        bool m_signalData[NBITS];

        enum ReadType { NONE, GOOD, BAD, ERROR };
        ReadType m_readType{ NONE };
    };

    static void twoMeansDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]);
    static void splitDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]);

    DhtSensor* m_sensor = nullptr;
};

#endif // !defined(EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_)
