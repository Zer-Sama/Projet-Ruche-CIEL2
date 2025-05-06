///////////////////////////////////////////////////////////
//  CGPIO.cpp
//  Implementation of the Class CGPIO
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CGPIO.h"
#include "CSysteme.h"

CGPIO::CGPIO(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " pin\n";
        return;
    }

    int pin;
    try {
        pin = std::stoi(argv[1]);
    }
    catch (std::invalid_argument&) {
        std::cerr << "Invalid argument: " << argv[1] << "\n";
        return;
    }

    if (wiringPiSetupGpio() == -1) {
        std::cout << "wiringPi setup failed\n";
        return;
    }

    m_sensor = new DhtSensor(pin);  // Création du capteur

#ifdef DEBUG
    DhtSensor::printSignalTitle();
#endif
}

CGPIO::~CGPIO() {
    delete m_sensor;  // Libération de la mémoire
}

void CGPIO::Lire(double& temperature, double& humidity) {
    if (!m_sensor) {
        std::cerr << "Sensor non initialisé\n";
        temperature = humidity = -1;
    }


    for (int i = 0; i < 1000; i++) {
        m_sensor->read();

#ifdef DEBUG
        m_sensor->printSignal();
#endif
        m_sensor->print();

    }

    temperature = m_sensor->m_temperature;
    humidity = m_sensor->m_humidity;
}
