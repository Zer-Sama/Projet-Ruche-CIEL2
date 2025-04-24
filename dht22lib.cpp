/**
Robust Adafruit DHT22 sensor reader

Outputs temperature and humidity to the command line

Based on:
http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/

Significant changes:
--------------------
[Robust decoding of data]
In the previous implementation, the data
(5 bytes: 2 each for humidity/temperature and 1 for a checksum)
is decoded by classifying signal states as 1 or 0 depending if the duration held by the state is
longer than 16 microseconds or not.
Sometimes, this classification is inaccurate and we get bad data, which is typically
invalidated by the checksum. To see why, here is realistic example of recorded state durations in
microseconds (1 byte only):

         Signal A: ( 3, 2, 3, 8, 8, 9, 8, 3)
         Signal B: ( 7, 7, 7,32,33,33,32, 6)
         Signal C: (21,22,22,86,84,84,85,23)
True decoded data: ( 0, 0, 0, 1, 1, 1, 1, 0)

It is clear to see that the encoded data is present in all signals, but with the previous
classification technique only signal B will be decoded correctly. I'm not sure exactly why
in some circumstances the state durations are longer/shorter. I've observed consistently
shorter durations when running this in a docker container.

The newer the decoding technique uses k-means (k=2) to cluster the signal into upper and lower
clusters and classify any states assigned to the upper cluster as a '1'. This results in all
signals A, B and C being decoded correctly.

Other changes:
- Fix undefined behaviour when reading too many bits in the read loop

*/

#include "dht22lib.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#define DEFAULT_TEXT printf("\033[0m");
#define BLACK_TEXT printf("\033[0;30m");
#define TEAL_TEXT printf("\033[36;1m");
#define RED_TEXT printf("\033[0;31m");

void twoMeansDecoder(const int (&x)[NBITS], bool (&binaryAssignment)[NBITS])
{
    // The initial values for the centroids are the minimum and maximum
    float lower = x[0];
    float upper = x[0];
    for (int elem : x) {
        if (elem < lower) lower = elem;
        if (elem > upper) upper = elem;
    }

    // Reset assignments (assign all to the lower centroid)
    for (bool &elem : binaryAssignment)
        elem = false;

    while (true) {
        // This will always converge, so won't run infinitely

        // Assignment step: assign each observation to the nearest centroid
        bool newAssignUpper[NBITS];
        for (int j = 0; j < NBITS; j++) {
            if ((abs(lower - x[j]) > abs(upper - x[j])))
                newAssignUpper[j] = true;
            else
                newAssignUpper[j] = false;
        }

        // Update step: update the centroid location with the mean of the assigned observations
        float newLower = 0;
        float newUpper = 0;
        int numUpperObservations = 0;
        for (int j = 0; j < NBITS; j++) {
            numUpperObservations += newAssignUpper[j];
            if (newAssignUpper[j])
                newUpper += x[j];
            else
                newLower += x[j];
        }
        newUpper /= numUpperObservations;
        newLower /= NBITS - numUpperObservations;

        // Check convergence: k-means has converged if no assignments have changed
        bool converged = true;
        for (int j = 0; j < NBITS; j++) {
            if (newAssignUpper[j] != binaryAssignment[j]) {
                converged = false;
                break;
            }
        }
        if (converged) return;

        // Iterate: new values are now old
        upper = newUpper;
        lower = newLower;
        for (int j = 0; j < NBITS; j++)
            binaryAssignment[j] = newAssignUpper[j];
    }
}

void splitDecoder(const int (&x)[NBITS], bool (&binaryAssignment)[NBITS])
{
    // Get minimum and maximum values of the x
    int min = x[0];
    int max = x[0];
    for (int elem : x) {
        if (elem < min) min = elem;
        if (elem > max) max = elem;
    }

    int midpoint = min + (max - min) / 2;

    for (int j = 0; j < NBITS; j++)
        binaryAssignment[j] = midpoint < x[j];
}

DhtSensor::DhtSensor(int pin) : m_pin{pin} {}

void DhtSensor::read()
{
    uint8_t lastState = HIGH;
    uint8_t stateDuration = 0;
    uint8_t stateChanges = 0;
    uint8_t bitsRead = 0;
    float humidity = BAD_VALUE;
    float temperature = BAD_VALUE;

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

    for ((stateChanges = 0), (stateDuration = 0);
        (stateChanges < MAX_TIMINGS) && (stateDuration < 255) && (bitsRead < NBITS);
        stateChanges++) {
        stateDuration = 0;
        while ((digitalRead(m_pin) == lastState) && (stateDuration < 255)) {
            stateDuration++;
            delayMicroseconds(1);
        };

        lastState = digitalRead(m_pin);

        if ((stateChanges > 2) && (stateChanges % 2 == 0)) {
            m_signalStateDurations[bitsRead] = stateDuration;
            bitsRead++;
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

    twoMeansDecoder(m_signalStateDurations, m_signalData);

    for (int j = 0; j < NBITS; j++) {
        data[j / 8] <<= 1;
        if (m_signalData[j])
            data[j / 8] |= 0x00000001;
    }

    if ((bitsRead >= NBITS) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        humidity = (float)((data[0] << 8) + data[1]) / 10.0;
        temperature = (float)((data[2] << 8) + data[3]) / 10.0;
        if (data[2] & 0x80)
            temperature *= -1;
        m_readType = GOOD;
    }
    else {
        m_readType = BAD;
    }

    m_humidity = humidity;
    m_temperature = temperature;

    // === Enregistrement CSV ===
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


void DhtSensor::print()
{
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

void DhtSensor::printSignalTitle()
{
    printf("DEBUG MODE: Displaying microseconds in each state.\n");
    printf("Rows with \033[0;31mRED\033[0m text would have been previously decoded wrongly.\n");
    for (int j{0}; j < NBITS; j++) {
        printf("%3d", j);
        if ((j != 0) && (j % 8 == 7))
            printf("║");
        else
            printf("|");
    }
    std::cout << "\n";
    for (int j{0}; j < NBITS; j++)
        printf("----");
    std::cout << "\n";
}

void DhtSensor::printSignal()
{
    // Print state duration
    // Colour correctly decoded values by the previous method as TEAL or DEFAULT
    // Colour incorrectly decoded values by the previous method, but correctly identified by the
    // new method as RED
    for (int j = 0; j < NBITS; j++) {
        if (m_signalStateDurations[j] > 16 && m_signalData[j])
            TEAL_TEXT
        else if (m_signalStateDurations[j] < 16 && m_signalData[j])
            RED_TEXT
        else if (m_signalStateDurations[j] > 16 && !m_signalData[j])
            RED_TEXT

        if (m_signalStateDurations[j] == BAD_VALUE) BLACK_TEXT
        printf("%3d", m_signalStateDurations[j]);
        DEFAULT_TEXT

        if ((j != 0) && (j % 8 == 7))
            printf("║");
        else
            printf("|");
    }
}