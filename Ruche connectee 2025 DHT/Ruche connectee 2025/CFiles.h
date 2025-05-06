///////////////////////////////////////////////////////////
//  CFiles.h
//  Implementation of the Class CFiles
//  Created on:      26-mars-2025 15:05:33
//  Original author: Utilisateur
///////////////////////////////////////////////////////////


#if !defined(EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_)
#define EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_



class CFiles
{

public:
	CFiles();
	virtual ~CFiles();
	void ecrire(int IDStream, int fileIndex);
	void fermer(int IDStream);
	void lire(int fileIndex);
	void ouvrir(string FileName,int fileIndex);

private:
	string FileName1;
	string FileName2;
	int IDStream;
	
	
};
#endif // !defined(EA_2A63CD98_346B_4a6c_A0B4_899AAFBCBAFE__INCLUDED_)
