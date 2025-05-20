///////////////////////////////////////////////////////////
//  CGPIO.cpp
//  Fusionné avec dht22lib.cpp
//  Créé le:      26-mars-2025 15:05:34
///////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <wiringPi.h>
#include "CGPIO.h"

#define MAX_TIMINGS 85
#define NBITS 40
#define BAD_VALUE -1000.0f

#define DEFAULT_TEXT printf("\033[0m");
#define BLACK_TEXT printf("\033[0;30m");
#define TEAL_TEXT printf("\033[36;1m");
#define RED_TEXT printf("\033[0;31m");

class CGPIO {
public:
    CGPIO(int argc, char** argv);
    ~CGPIO();
    void Lire(double& temperature, double& humidity);

private:
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

    DhtSensor* m_sensor;
};

// ==== Implémentation ====

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

    m_sensor = new DhtSensor(pin);

#ifdef DEBUG
    DhtSensor::printSignalTitle();
#endif
}

CGPIO::~CGPIO() {
    delete m_sensor;
}

void CGPIO::Lire(double& temperature, double& humidity) {
    if (!m_sensor) {
        std::cerr << "Sensor non initialisé\n";
        temperature = humidity = -1;
        return;
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

CGPIO::DhtSensor::DhtSensor(int pin) : m_pin{ pin } {}

void CGPIO::DhtSensor::read() {
    uint8_t lastState = HIGH, stateDuration = 0, stateChanges = 0, bitsRead = 0;
    float humidity = BAD_VALUE, temperature = BAD_VALUE;
    int data[5] = { 0, 0, 0, 0, 0 };

    for (int& elem : m_signalStateDurations)
        elem = BAD_VALUE;

    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, HIGH);
    delay(10);
    digitalWrite(m_pin, LOW);
    delay(18);
    digitalWrite(m_pin, HIGH);
    delayMicroseconds(40);
    pinMode(m_pin, INPUT);

    for ((stateChanges = 0), (stateDuration = 0); stateChanges < MAX_TIMINGS && stateDuration < 255 && bitsRead < NBITS; stateChanges++) {
        stateDuration = 0;
        while ((digitalRead(m_pin) == lastState) && (stateDuration < 255)) {
            stateDuration++;
            delayMicroseconds(1);
        }

        lastState = digitalRead(m_pin);
        if ((stateChanges > 2) && (stateChanges % 2 == 0)) {
            m_signalStateDurations[bitsRead++] = stateDuration;
        }
    }

    for (int elem : m_signalStateDurations) {
        if (elem == BAD_VALUE) {
            m_humidity = BAD_VALUE;
            m_temperature = BAD_VALUE;
            m_readType = ERROR;
            return;
        }
    }

    CGPIO::twoMeansDecoder(m_signalStateDurations, m_signalData);

    for (int j = 0; j < NBITS; j++) {
        data[j / 8] <<= 1;
        if (m_signalData[j]) data[j / 8] |= 0x01;
    }

    if ((bitsRead >= NBITS) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        humidity = (float)((data[0] << 8) + data[1]) / 10.0;
        temperature = (float)((data[2] << 8) + data[3]) / 10.0;
        if (data[2] & 0x80) temperature *= -1;
        m_readType = GOOD;
    }
    else {
        m_readType = BAD;
    }

    m_humidity = humidity;
    m_temperature = temperature;

    if (m_readType == GOOD) {
        static int id_counter = 0;
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* ltm = std::localtime(&now_time);

        std::ostringstream timeStream;
        timeStream << std::put_time(ltm, "%Y-%m-%d %H:%M:%S");
        std::string formattedTime = timeStream.str();

        std::ofstream csvFile("constante_ruche.csv", std::ios::app);
        if (csvFile.is_open()) {
            csvFile << id_counter++ << "," << formattedTime << "," << m_temperature << "," << m_humidity << "\n";
            csvFile.close();
        }
        else {
            std::cerr << "Erreur lors de l'ouverture du fichier CSV\n";
        }
    }
}

void CGPIO::DhtSensor::print() {
    switch (m_readType) {
    case GOOD:
        printf("%-3.1f *C  Humidity: %-3.1f%%\n", m_temperature, m_humidity);
        break;
    case BAD:
        printf("Failed checksum\n");
        break;
    case ERROR:
        printf("ERROR, failed reading data\n");
        break;
    case NONE:
        printf("No attempt made to read data\n");
        break;
    default:
        std::cerr << "This shouldn't happen\n";
    }
}

void CGPIO::DhtSensor::printSignalTitle() {
    printf("DEBUG MODE: Displaying microseconds in each state.\n");
    printf("Rows with \033[0;31mRED\033[0m text would have been previously decoded wrongly.\n");
    for (int j = 0; j < NBITS; j++) {
        printf("%3d", j);
        printf((j % 8 == 7) ? "║" : "|");
    }
    std::cout << "\n";
    for (int j = 0; j < NBITS; j++) printf("----");
    std::cout << "\n";
}

void CGPIO::DhtSensor::printSignal() {
    for (int j = 0; j < NBITS; j++) {
        if (m_signalStateDurations[j] > 16 && m_signalData[j])
            TEAL_TEXT
        else if ((m_signalStateDurations[j] < 16 && m_signalData[j]) ||
            (m_signalStateDurations[j] > 16 && !m_signalData[j]))
            RED_TEXT
        else if (m_signalStateDurations[j] == BAD_VALUE)
            BLACK_TEXT

            printf("%3d", m_signalStateDurations[j]);
        DEFAULT_TEXT
            printf((j % 8 == 7) ? "║" : "|");
    }
    std::cout << "\n";
}

void CGPIO::twoMeansDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]) {
    float lower = x[0], upper = x[0];
    for (int elem : x) {
        if (elem < lower) lower = elem;
        if (elem > upper) upper = elem;
    }

    for (bool& elem : binaryAssignment)
        elem = false;

    while (true) {
        bool newAssignUpper[NBITS];
        for (int j = 0; j < NBITS; j++)
            newAssignUpper[j] = (abs(lower - x[j]) > abs(upper - x[j]));

        float newLower = 0, newUpper = 0;
        int numUpper = 0;
        for (int j = 0; j < NBITS; j++) {
            if (newAssignUpper[j]) {
                newUpper += x[j];
                numUpper++;
            }
            else {
                newLower += x[j];
            }
        }

        newUpper /= numUpper;
        newLower /= NBITS - numUpper;

        bool converged = true;
        for (int j = 0; j < NBITS; j++) {
            if (newAssignUpper[j] != binaryAssignment[j]) {
                converged = false;
                break;
            }
        }

        if (converged) return;

        upper = newUpper;
        lower = newLower;
        for (int j = 0; j < NBITS; j++)
            binaryAssignment[j] = newAssignUpper[j];
    }
}

void CGPIO::splitDecoder(const int(&x)[NBITS], bool(&binaryAssignment)[NBITS]) {
    int min = x[0], max = x[0];
    for (int elem : x) {
        if (elem < min) min = elem;
        if (elem > max) max = elem;
    }
    int midpoint = min + (max - min) / 2;
    for (int j = 0; j < NBITS; j++)
        binaryAssignment[j] = midpoint < x[j];
}
