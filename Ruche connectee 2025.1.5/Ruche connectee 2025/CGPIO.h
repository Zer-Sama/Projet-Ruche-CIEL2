///////////////////////////////////////////////////////////
//  CGPIO.h
//  Implementation of the Class CGPIO
//  Created on:      26-mars-2025 15:05:34
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#if !defined(EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_)
#define EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_

#include <cstring>
#include <hx711/common.h>

using namespace std;
using namespace HX711;

class CGPIO
{

public:
	CGPIO();
	virtual ~CGPIO();

	string Lire_GPS();
	double Lire_Masse();
	void Lire_DHT22();

	

private:
	int BrocheGPIO;
	

};
#endif // !defined(EA_CAC7FA92_6D51_4c0f_9992_E3C444C35812__INCLUDED_)