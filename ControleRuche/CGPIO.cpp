///////////////////////////////////////////////////////////
//  CGPIO.cpp
//  Implementation of the Class CGPIO
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CGPIO.h"
#include <iostream>
#include <fcntl.h>   // File control definitions
#include <unistd.h>  // UNIX standard function definitions
#include <termios.h> // POSIX terminal control definitions
#include <string>
#include <cstring>
#include <vector>
#include <hx711/common.h>
#include <sstream>
#include <cctype>
#include <wiringPi.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <utility>

#define MAX_TIMINGS 85
#define NBITS 40
#define BAD_VALUE 999

using namespace std;
using namespace HX711;

CGpio::CGpio() {

}



CGpio::~CGpio() {

}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    for (char ch : s) {
        if (ch == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}



pair<string, string> CGpio::Lire_GPS() {
    cout << "debut GPS" << endl;
    const char* portname = "/dev/ttyUSB0";
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        cerr << "Erreur lors de l'ouverture du port serie." << endl;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        cerr << "Erreur lors de la lecture des attributs du port serie." << endl;
    }

    cfsetospeed(&tty, B9600); // Definir la vitesse de communication (9600 bauds est courant pour les GPS)
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits par byte
    tty.c_iflag &= ~IGNBRK; // desactiver l'ignorance des breaks
    tty.c_lflag = 0; // mode non canonique
    tty.c_oflag = 0; // pas de traitement de sortie
    tty.c_cc[VMIN] = 1; // bloquer jusqu'à ce qu'un caractere soit reçu
    tty.c_cc[VTIME] = 5; // timeout de 0.5 secondes

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // desactiver le controle de flux
    tty.c_cflag |= (CLOCAL | CREAD); // ignorer les lignes de controle de modem, activer le recepteur
    tty.c_cflag &= ~(PARENB | PARODD); // pas de parite
    tty.c_cflag &= ~CSTOPB; // un bit de stop
    tty.c_cflag &= ~CRTSCTS; // pas de controle de flux materiel

    cout << "mil GPS" << endl;

    char buf[256];
    string sentence;

    int n = read(fd, buf, sizeof(buf) - 1);  // une seule lecture
    if (n > 0) {
        buf[n] = '\0';
        sentence += buf;

        size_t end = sentence.find('\n');
        while (end != string::npos) {
            string nmeaSentence = sentence.substr(0, end);
            if (nmeaSentence.find("$GPGGA") != string::npos) {
                vector<string> parts = split(nmeaSentence, ',');
                if (parts.size() >= 6) {
                    return make_pair(parts[2], parts[4]);
                }
            }
            sentence.erase(0, end + 1);
            end = sentence.find('\n');
        }
    }

    cout << "fin GPS" << endl;
    close(fd);
    return make_pair("", "");
}

double CGpio::Lire_Masse() {
    SimpleHX711 hx(5, 6, 44744, 67100);

    Mass masse = hx.weight(25);
    float poids = masse.getValue();
    return poids;
}

void CGpio::Lire_DHT22() {
    int pin = 4; 
    int data[5] = { 0, 0, 0, 0, 0 };
    int signalDurations[NBITS] = { 0 };
    bool signalBits[NBITS] = { false };
    cout << "DHT 1" << endl;
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "wiringPi setup failed\n";
    }

    // Initialisation
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(10);
    digitalWrite(pin, LOW);
    delay(18);
    digitalWrite(pin, HIGH);
    delayMicroseconds(40);
    pinMode(pin, INPUT);

    cout << "DHT 2" << endl;

    // Lecture des signaux
    uint8_t lastState = HIGH;
    int bitIdx = 0;
    for (int i = 0; i < MAX_TIMINGS; i++) {
        int count = 0;
        while (digitalRead(pin) == lastState && count < 255) {
            count++;
            delayMicroseconds(1);
        }
        lastState = digitalRead(pin);

        if ((i >= 4) && (i % 2 == 0) && bitIdx < NBITS) {
            signalDurations[bitIdx++] = count;
        }
    }

    cout << "DHT 3" << endl;

    if (bitIdx < NBITS) {
        std::cerr << "Erreur : lecture incomplète\n";

    }

    // Deux-moyennes (decodage binaire)
    float lower = signalDurations[0], upper = signalDurations[0];
    for (int i = 1; i < NBITS; ++i) {
        if (signalDurations[i] < lower) lower = signalDurations[i];
        if (signalDurations[i] > upper) upper = signalDurations[i];
    }

    cout << "DHT 4" << endl;

    for (int i = 0; i < NBITS; ++i)
        signalBits[i] = std::abs(signalDurations[i] - upper) < std::abs(signalDurations[i] - lower);

    // Conversion des bits en octets
    for (int i = 0; i < NBITS; ++i) {
        data[i / 8] <<= 1;
        if (signalBits[i]) data[i / 8] |= 1;
    }

    cout << "DHT 5" << endl;

    // Vérification de la somme de contrôle
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        double humidity = ((data[0] << 8) + data[1]) / 10.0;
        double temperature = ((data[2] << 8) + data[3]) / 10.0;
        if (data[2] & 0x80) temperature = -temperature;
        cout << "DHT 5.5" << endl;
        // Affichage
        //std::cout << "Température : " << temperature << " °C\n";
        //std::cout << "Humidité : " << humidity << " %\n";
        valDHT22[0] = temperature;
        valDHT22[1] = humidity;
    }
    else {
        std::cerr << "Checksum invalide\n";
    }

    cout << "DHT 6" << endl;

   // return valDHT22;
}