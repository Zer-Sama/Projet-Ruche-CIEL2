///////////////////////////////////////////////////////////
//  CSysteme.cpp
//  Implementation of the Class CSysteme
//  Created on:      26-mars-2025 15:05:35
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CSysteme.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace std;

CSysteme::CSysteme():file(),gpio(){
	
	cout << "constructeur csysteme ok" << endl;

}

CSysteme::~CSysteme(){

}

bool CSysteme::comparer()
{
	if (parametres[4]=="true")
	{
		if (DHT22[0]< stod(parametres[1]) && DHT22[0] > stod(parametres[0]))
		{
			//Calerter
			cout << "message pour l'apiculteur température de " << DHT22[0] << endl;
			return true;
		}
		if (DHT22[1] > stod(parametres[2]))
		{
			//Calerter
			cout << "message pour l'apiculteur hygrometire de " << DHT22[1] << endl;
			return true;
		}
	}
	return false;
}

void CSysteme::ModifierParametre(){

	cout << "debut boucle csysteme ok" << endl;

		file.OuvrirLire(file4);
		parametres = file.parametres;
		cout << "avant capteur" << endl;
		//position = gpio.Lire_GPS();
		cout << "capteur GPS" << endl;
		Masse = gpio.Lire_Masse();
		cout << "capteur Masse" <<Masse<< endl;
		gpio.Lire_DHT22();
		DHT22[0] = Get_DHT22_Temp();
		DHT22[1] = Get_DHT22_Hygro();
		cout << "capteur DHT22" << endl;

		//Comparer = comparer();
		//cout << "comparer ok" << endl;
		file.OuvrirLire(file3);
		marqueur = file.parametres;
		if (parametres[0] == "1") {
			file.OuvrirSup(file1, 0);
			file.OuvrirSup(file2, 0);
			file.OuvrirSup(file3, "0");
			cout << "sup ok" << endl;
		}
		
		auto maintenant = chrono::system_clock::now();
		time_t tempsMaintenant = std::chrono::system_clock::to_time_t(maintenant);
		tm* tmMaintenant = std::localtime(&tempsMaintenant);
		stringstream ss;
		ss << put_time(tmMaintenant, "%Y-%m-%d %H:%M:%S");
		donnees = ss.str() + "," + to_string(DHT22[0]) + "," + to_string(DHT22[1]) + "," + to_string(Masse) + "," + position;

		file.Ouvrir(file2, donnees);
		cout << donnees << endl;

}

int  CSysteme::TraitementRequete(){

	return  0;
}

double CSysteme::Get_DHT22_Temp() {
	double temp = gpio.valDHT22[0];
	return temp;
}

double CSysteme::Get_DHT22_Hygro() {
	double hygro = gpio.valDHT22[1];
	return hygro;
}
