///////////////////////////////////////////////////////////
//  CFiles.h
//  Implementation of the Class CFiles
//  Created on:      26-mars-2025 15:05:33
//  Original author: Utilisateur
///////////////////////////////////////////////////////////


#if !defined(EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_)
#define EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_

#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

using namespace std;

class CFiles
{

public:
	CFiles();
	virtual ~CFiles();
	void ecriredonnees(ofstream &idstream, string donnees);
	void fermer(ofstream& IDStream);
	vector<string> lire(ifstream& idstream);
	void Ouvrir(string NomFichier,string donnees);
	void OuvrirSup(string NomFichier,string donnees);
	void OuvrirLire(string NomFichier);

	vector<string> parametres;

private:
	
};
#endif // !defined(EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_)
