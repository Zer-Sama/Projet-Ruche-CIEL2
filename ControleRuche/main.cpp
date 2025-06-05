#include "CGestioCame.h"
#include "CSysteme.h"
#include <iostream>
#include <unistd.h>
#include <thread>

using namespace std;

CGestioCame Camera;
CSysteme systeme;
bool capteur;

void timeur() {
    while (true) {
        capteur = false;
        for (int i = 0; i <= 6; i++) {
            sleep(1);
            cout << i << endl;
        }
        capteur = true;
        sleep(1);
    }
}

int main()
{

    thread Boucle(timeur);
    Boucle.detach();
    while (true) {
        if (capteur) {
            cout << "boucle capteur" << endl;
            systeme.ModifierParametre();
        }
        Camera.PrendreUnePhoto();
        sleep(0.15);
    }
}

