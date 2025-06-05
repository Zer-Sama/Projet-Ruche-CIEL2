///////////////////////////////////////////////////////////
//  CSysteme.h
//  Implementation of the Class CSysteme
//  Created on:      26-mars-2025 15:05:35
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#if !defined(EA_66810725_71FC_4ecc_9432_505B2075A971__INCLUDED_)
#define EA_66810725_71FC_4ecc_9432_505B2075A971__INCLUDED_

#include "CAlerter.h"
#include "CFiles.h"
#include "CGPIO.h"
#include <cstring>

using namespace std;

class CSysteme
{

public:
	CSysteme();
	~CSysteme();
	int IDStream;
	bool comparer();
	void ModifierParametre();
	int  TraitementRequete();
	double Get_DHT22_Temp();
	double Get_DHT22_Hygro();

	string NumTel;
	double TemperatureCapteur;
	double HygrometrieCapteur;
	double MasseCapteur;


private:

	CFiles file;
	CGpio gpio;

	double HygrometrieRefMin;
	double HygrometrieRefMax;
	double TemperatureRefMax;
	double TemperatureRefMin;
	bool ConstenteAlerte;
	bool ReineAlerte;

	string position;
	double Masse;
	double DHT22[2];
	bool Comparer;
	string donnees;

	vector<string> parametres;
	vector<string> marqueur;

	string file1 = "ConstanteReine.csv";
	string file2 = "constante_ruche.csv";
	string file3 = "marqueur.txt";
	string file4 = "parametreruche.txt";

}; 
#endif // !defined(EA_66810725_71FC_4ecc_9432_505B2075A971__INCLUDED_)
