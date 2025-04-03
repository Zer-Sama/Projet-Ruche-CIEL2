#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

int hminYel = 20, hminBlue = 100, smin = 50, vmin = 50;
int hmaxYel = 30, hmaxBlue = 145, smax = 150, vmax = 150;
Mat imgHSV,maskYel,maskBlue;

int main()
{
//string path=("image.jpg");
VideoCapture cap(0, CAP_V4L2);
Mat img/*=imread(path)*/;
Scalar lowerYel(hminYel, smin, vmin);
Scalar upperYel(hmaxYel, smax, vmax);

Scalar lowerBlue(hminBlue, smin, vmin);
Scalar upperBlue(hmaxBlue, smax, vmax);

string heure;

while(true)
{
	cap.read(img);

	//img.convertTo(img, CV_32F);

	//resize(img, img, Size(854, 480));
	cvtColor(img, imgHSV, COLOR_BGR2HSV);
	inRange(img, lowerYel, upperYel, maskYel);
	inRange(img, lowerBlue, upperBlue, maskBlue);

	if (countNonZero(maskYel) > 0)
	{
		ofstream fichier("ConstanteReine.csv", ios::app);
		if (!fichier.is_open()) {
        		cout << "Erreur : impossible d'ouvrir le fichier." << std::endl;
        		return 1;
		}
		time_t now = time(nullptr);
		tm *localTime = localtime(&now);
		char buffer[80];
		strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
		cout << "\nIl y a au moins un pixel jaune dans l'image.\n" << buffer << endl;
		fichier << buffer <<";jaune"<<endl;
		&fichier;
		//waitKey(5000);
	}
/*	if (countNonZero(maskBlue) > 0)
        {
                ofstream fichier("ConstanteReine.csv", ios::app);
                if (!fichier.is_open()) {
                        cout << "Erreur : impossible d'ouvrir le fichier." << std::endl;
                        return 1;
                }
                time_t now = time(nullptr);
                tm *localTime = localtime(&now);
                char buffer[80];
                strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
                cout << "\nIl y a au moins un pixel bleu dans l'image.\n" << buffer << endl;
                fichier << buffer <<";bleu"<<endl;
                &fichier;
                //waitKey(5000);
        }
*/
	waitKey(1500);
}
}
