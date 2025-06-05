///////////////////////////////////////////////////////////
//  CFiles.cpp
//  Implementation of the Class CFiles
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CFiles.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

CFiles::CFiles(){

}

CFiles::~CFiles() {
	
}

void CFiles::Ouvrir(string NomFichier,string donnees) {
	ofstream outFile(NomFichier,ios::app);
	if (!outFile.is_open()) {
		cerr << "Erreur: Impossible d'ouvrir le fichier!" << endl;
	}
	else {
		ecriredonnees(outFile,donnees);
		fermer(outFile);
	}
}

void CFiles::OuvrirSup(string NomFichier, string donnees) {
	ofstream outFile(NomFichier);
	if (!outFile.is_open()) {
		cerr << "Erreur: Impossible d'ouvrir le fichier!" << endl;
	}
	else {
		ecriredonnees(outFile,donnees);
		fermer(outFile);
	}
}

void CFiles::OuvrirLire(string NomFichier) {
	ifstream outFile(NomFichier);
	if (!outFile.is_open()) {
		cerr << "Erreur: Impossible d'ouvrir le fichier!" << endl;
	}
	else {
		lire(outFile);
		outFile.close();
	}
}

void CFiles::fermer(ofstream &IDStream) {
	IDStream.close();
}

void CFiles::ecriredonnees(ofstream &idstream, string donnees) {

	idstream << donnees << endl;
}

vector<string> CFiles::lire(ifstream &idstream) {
	string line;

	// Lire la première ligne du fichier
	if (getline( idstream , line)) {
		stringstream ss(line);
		string param;

		// Diviser la ligne en paramètres séparés par des virgules
		while (getline(ss, param, ',')) {
			parametres.push_back(param);
		}
	}
	return parametres;
}