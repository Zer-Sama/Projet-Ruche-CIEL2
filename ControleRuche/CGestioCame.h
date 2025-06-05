#pragma once
///////////////////////////////////////////////////////////
//  CGestioCame.h
//  Implementation of the Class CGestioCame
//  Created on:      30-avr.-2025 13:08:47
//  Original author: Utilisateur
///////////////////////////////////////////////////////////

#if !defined(EA_FEB92D1C_AC1D_457c_999F_F432B91D0BAC__INCLUDED_)
#define EA_FEB92D1C_AC1D_457c_999F_F432B91D0BAC__INCLUDED_

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

class CGestioCame
{

public:
	CGestioCame();
	virtual ~CGestioCame();

	void FiltrerLesCouleur(Mat& mask, const string& colorName, Mat& img);
	void PrendreUnePhoto();

};
#endif // !defined(EA_FEB92D1C_AC1D_457c_999F_F432B91D0BAC__INCLUDED_)
