///////////////////////////////////////////////////////////
//  CSysteme.cpp
//  Implementation of the Class CSysteme
//  Created on:      26-mars-2025 15:05:35
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CSysteme.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

CSysteme::CSysteme():file(),gpio(){
	file.OuvrirLire(file4);
	parametres = file.parametres;
	while (true) {
		position=gpio.Lire_GPS();
		Masse = gpio.Lire_Masse();

		Comparer=comparer();

		file.OuvrirLire(file3);
		parametres = file.parametres;
		if (parametres[0] == "1") {
			file.OuvrirSup(file1,0);
			file.OuvrirSup(file2,0);
			file.OuvrirSup(file3,"0");
		}
		auto now = chrono::system_clock::now();
		time_t localTime = chrono::system_clock::to_time_t(now);

		stringstream ss;
		ss << put_time(localTime,"%Y-%m-%d %H:%M:%S");
		string timeString = ss.str();
		donnees = to_string(Masse) + position + timeString;

		file.Ouvrir(file2,donnees);
	}
}

CSysteme::~CSysteme(){

}

bool CSysteme::comparer(){
	return true;
}

void CSysteme::ModifierParametre(){
	
}

int  CSysteme::TraitementRequete(){

	return  NULL;
}