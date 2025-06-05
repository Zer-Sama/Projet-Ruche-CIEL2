#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <thread>

using namespace cv;
using namespace std;

Mat imgHSV, maskYel, maskBlue, maskRed, maskGreen;
int hminYel = 45, hmaxYel = 80, sminYEL = 50, smaxYEL = 250, vminYEL = 100, vmaxYEL = 250;
int hminBlue = 180, hmaxBlue = 260, sminBLUE = 150, smaxBLUE = 250, vminBLUE = 100, vmaxBLUE = 250;
int hminRed = 335, hmaxRed = 359, sminRed = 70, smaxRed = 250, vminRed = 90, vmaxRed = 250;
int hminGreen = 81, hmaxGreen = 175, sminGreen = 0, smaxGreen = 255, vminGreen = 0, vmaxGreen = 255;
const int MIN_AREA = 500;

void detectShape(Mat& mask, const string& colorName, Mat& img) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > MIN_AREA) {
            drawContours(mask, contours, (int)i, Scalar(0, 255, 0), 2);

            if (countNonZero(mask) > 0)
            {
                time_t now = time(nullptr);
                tm* localTime = localtime(&now);
                char buffer[20];
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
                cout << "\nIl y a au moins un pixel " << colorName << " dans l'image.\n" << buffer << endl;
                ofstream fichier("/home/pi/projet/ControleRuche/ConstanteReine.csv", ios::app);
                if (!fichier.is_open()) {
                    cout << "Erreur : impossible d'ouvrir le fichier." << endl;
                    return;
                }
                fichier << buffer << ";" << colorName << endl;
                fichier.close();
            }
        }
    }
}

int main()
{
    VideoCapture cap(0, CAP_V4L2);
    Mat img;

    Scalar lowerYel(hminYel / 2, sminYEL, vminYEL);
    Scalar upperYel(hmaxYel / 2, smaxYEL, vmaxYEL);
    Scalar lowerBlue(hminBlue / 2, sminBLUE, vminBLUE);
    Scalar upperBlue(hmaxBlue / 2, smaxBLUE, vmaxBLUE);
    Scalar lowerRed(hminRed / 2, sminRed, vminRed);
    Scalar upperRed(hmaxRed / 2, smaxRed, vmaxRed);
    Scalar lowerGreen(hminGreen / 2, sminGreen, vminGreen);
    Scalar upperGreen(hmaxGreen / 2, smaxGreen, vmaxGreen);

    while (true)
    {
        cap.read(img);

        cvtColor(img, imgHSV, COLOR_BGR2HSV);
        inRange(imgHSV, lowerYel, upperYel, maskYel);
        inRange(imgHSV, lowerBlue, upperBlue, maskBlue);
        inRange(imgHSV, lowerRed, upperRed, maskRed);
        inRange(imgHSV, lowerGreen, upperGreen, maskGreen);

        detectShape(maskYel, "jaune", img);
        detectShape(maskBlue, "bleu", img);
        detectShape(maskRed, "rouge", img);
        detectShape(maskGreen, "vert", img);
        waitKey(150);
    }
}
