///////////////////////////////////////////////////////////
//  CSysteme.cpp
//  Implementation of the Class CSysteme
//  Created on:      26-mars-2025 15:05:35
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CSysteme.h"
#include "CGPIO.h"

CSysteme::CSysteme(){

}



CSysteme::~CSysteme(){

}








bool CSysteme::comparer(){

	return  NULL;
}





void CSysteme::Ecrire(){

}


void CSysteme::ModifierParametre(){

}


int  CSysteme::TraitementRequete(){

	return  NULL;
}

void CSysteme::boucle(){
    char* args[] = { (char*)"programme", (char*)"4" };  // exemple : pin GPIO 4
    CGPIO gpio(2, args);  // argc = 2, argv = args

    double temperature, humidity;
    gpio.Lire(temperature, humidity);

    std::cout << "Température : " << temperature << ", Humidité : " << humidity << "\n";
}