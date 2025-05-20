///////////////////////////////////////////////////////////
//  CGPIO.h
//  Implementation of the Class CGPIO
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#pragma once

#include "CSysteme.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <algorithm>
#include <csignal>
#include <iostream>

#define MAX_TIMINGS 85  // Takes 84 state changes to transmit data
#define NBITS 40        // Total number of bits of data
#define BAD_VALUE 999

class CGPIO
{
public:
    CGPIO(int argc, char** argv);
    ~CGPIO();

    void Lire(double& temperature, double& humidity);  // Cette méthode appellera la méthode Lire de DhtSensor

    static void twoMeansDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]);
    void splitDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]);

    enum DhtReadType {
        GOOD,
        BAD,
        ERROR,
        NONE
    };

    class DhtSensor
    {
    public:
        int m_pin;
        float m_humidity{ BAD_VALUE };
        float m_temperature{ BAD_VALUE };
        DhtReadType m_readType{ NONE };

        int m_signalStateDurations[NBITS];
        bool m_signalData[NBITS];

        DhtSensor(int pin);
        void read();
        void print();
        static void printSignalTitle();
        void printSignal();
    };

private:
    DhtSensor* m_sensor = nullptr;  // Le membre sensor, qui est un objet DhtSensor
};
