#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

Mat imgHSV, maskYel, maskBlue;
int hminYel = 45, hmaxYel = 95, sminYEL = 115, smaxYEL = 250, vminYEL = 135, vmaxYEL = 250;
int hminBlue = 170, hmaxBlue = 230, sminBLUE = 70, smaxBLUE = 250, vminBLUE = 75, vmaxBLUE = 250;

int main()
{
	//string path=("image.jpg");
	VideoCapture cap(0);
	Mat img/*=imread(path)*/;

	namedWindow("trackbarYellow", (640, 200));
	createTrackbar("Yellow hue min", "trackbarYellow", &hminYel, 359);
	createTrackbar("Yellow hue max", "trackbarYellow", &hmaxYel, 359);
	createTrackbar("satYEL min", "trackbarYellow", &sminYEL, 255);
	createTrackbar("satYEL max", "trackbarYellow", &smaxYEL, 255);
	createTrackbar("valYEL min", "trackbarYellow", &vminYEL, 255);
	createTrackbar("valYEL max", "trackbarYellow", &vmaxYEL, 255);

	namedWindow("trackbarBlue", (640, 200));
	createTrackbar("BLUE hue min", "trackbarBlue", &hminBlue, 359);
	createTrackbar("BLUE hue max", "trackbarBlue", &hmaxBlue, 359);
	createTrackbar("satBLUE min", "trackbarBlue", &sminBLUE, 255);
	createTrackbar("satBLUE max", "trackbarBlue", &smaxBLUE, 255);
	createTrackbar("valBLUE min", "trackbarBlue", &vminBLUE, 255);
	createTrackbar("valBLUE max", "trackbarBlue", &vmaxBLUE, 255);

	while (true)
	{
		cap.read(img);
		
		Scalar lowerYel(hminYel / 2, sminYEL, vminYEL);
		Scalar upperYel(hmaxYel / 2, smaxYEL, vmaxYEL);
		Scalar lowerBlue(hminBlue / 2, sminBLUE, vminBLUE);
		Scalar upperBlue(hmaxBlue / 2, smaxBLUE, vmaxBLUE);

		cvtColor(img, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV, lowerYel, upperYel, maskYel);
		inRange(imgHSV, lowerBlue, upperBlue, maskBlue);

		imshow("img", img);
		imshow("imgYel", maskYel);
		imshow("imgBlue", maskBlue);
		imshow("imgHSV", imgHSV);

		if (countNonZero(maskYel) > 0)
		{
			time_t now = time(nullptr);
			tm* localTime = localtime(&now);
			char buffer[20];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
			cout << "\nIl y a au moins un pixel jaune dans l'image.\n" << buffer << endl;
			ofstream fichier("ConstanteReine.csv", ios::app);
			if (!fichier.is_open()) {
				cout << "Erreur : impossible d'ouvrir le fichier." << endl;
				return 1;
			}
			fichier << buffer << ";jaune" << endl;
			fichier.close();
			waitKey(5000);
		}
		if (countNonZero(maskBlue) > 0)
		{
			time_t now = time(nullptr);
			tm *localTime = localtime(&now);
			char buffer[20];
			strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
			cout << "\nIl y a au moins un pixel bleu dans l'image.\n" << buffer << endl;
			ofstream fichier("ConstanteReine.csv", ios::app);
			if (!fichier.is_open()) {
				cout << "Erreur : impossible d'ouvrir le fichier." << endl;
				return 1;
			}
			fichier << buffer <<";bleu"<<endl;
			fichier.close();
			waitKey(5000);
		}
		waitKey(100);
	}
}