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

using namespace std;
using namespace HX711;

CGPIO::CGPIO(){

}



CGPIO::~CGPIO(){

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



string CGPIO::Lire_GPS(){

    const char* portname = "/dev/ttyUSB1";
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

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        cerr << "Erreur lors de la configuration des attributs du port serie." << endl;
    }

    char buf[256];
    string sentence;
    while (true) {
        int n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            buf[n] = '\0';
            sentence += buf;

            // Chercher la fin d'une phrase NMEA
            size_t end = sentence.find('\n');
            while (end != string::npos) {
                string nmeaSentence = sentence.substr(0, end);
                if (nmeaSentence.find("$GPGGA") != string::npos) {
                    vector<string> parts = split(nmeaSentence, ',');
                    if (parts.size() >= 6) {
                        return parts[2],parts[4];
                    }
                }
                sentence.erase(0, end + 1);
                end = sentence.find('\n');
            }
        }
    }

    close(fd);
}

double CGPIO::Lire_Masse() {
    SimpleHX711 hx(5, 6, 44744, 67100);

    for (;;) {
        Mass masse = hx.weight(25);
        float poids = masse.getValue();
        Masse = poids;
    };
}

void CGPIO::Lire_DHT22() {

}