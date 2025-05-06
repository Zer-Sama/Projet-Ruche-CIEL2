///////////////////////////////////////////////////////////
//  CFiles.cpp
//  Implementation of the Class CFiles
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#include "CFiles.h"
#include <iostream>
#include <fstream>
using namespace std;

CFiles::CFiles():FileName1(""),FileName2("") {

}



CFiles::~CFiles() {
	
}





void CFiles::ecrire(int IDStream, int fileIndex) {

    string fileName = (fileIndex == 1) ? FileName1 : FileName2;
    ofstream file(fileName, ios::app); // Mode append pour ajouter du contenu sans écraser
    if (file.is_open()) {
        file << IDStream << endl;
        cout << "Écriture réussie dans le fichier " << fileName << "." << endl;
        file.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier " << fileName << " pour l'écriture." << endl;
    }
}


void CFiles::fermer(int IDStream) {

    
    
}


void CFiles::lire(int fileIndex) {
    string fileName = (fileIndex == 1) ? FileName1 : FileName2;
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier " << fileName << " pour la lecture." << endl;
    }
}

   



void CFiles::ouvrir(string FileName,int fileIndex){
    if (fileIndex == 1) {
        this->FileName1 = FileName;
    }
    else {
        this->FileName2 = FileName;
    }
    ifstream file(FileName);
    if (file.is_open()) {
        cout << "Fichier " << FileName << " ouvert avec succès." << endl;
        file.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier " << FileName << "." << endl;
    }
}
