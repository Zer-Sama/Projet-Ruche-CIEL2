///////////////////////////////////////////////////////////
//  CSysteme.h
//  Implementation of the Class CSysteme
//  Created on:      26-mars-2025 15:05:35
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

class CSysteme
{
public:
    CSysteme();
    ~CSysteme();

    int IDStream;
    bool comparer();
    void Ecrire();
    void ModifierParametre();
    int  TraitementRequete();
    void boucle();

private:
    double HygrometrieRef;
    double MasseRef;
    double TemperatureRef;
    float m_humidity;
    float m_temperature;
};
