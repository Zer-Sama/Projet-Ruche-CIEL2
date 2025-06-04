///////////////////////////////////////////////////////////
//  CAlerter.h
//  Implementation of the Class CAlerter
//  Created on:      26-mars-2025 15:05:33
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#if !defined(EA_8C8AA9DC_0A24_4c6f_8532_CDD59D29A2EA__INCLUDED_)
#define EA_8C8AA9DC_0A24_4c6f_8532_CDD59D29A2EA__INCLUDED_

#include <cstring>
#include <string>

using namespace std;

class CAlerter
{

public:
	CAlerter(const string& port, unsigned int baudrate);
	virtual ~CAlerter();

	bool connecter();
	void deconnecter();
	bool ecrire(double DonneesCapteur, char DonneesCamera);
	void RecupNumTel();

private:
	string message;
	string numtel;
	string port;
	unsigned int baudrate;
	int serialPort;
};
#endif // !defined(EA_8C8AA9DC_0A24_4c6f_8532_CDD59D29A2EA__INCLUDED_)
