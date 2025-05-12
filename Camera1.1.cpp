#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

Mat imgHSV, maskYel, maskBlue, maskRed, maskGreen, maskWhite;
int hminYel = 45, hmaxYel = 95, sminYEL = 115, smaxYEL = 250, vminYEL = 135, vmaxYEL = 250;
int hminBlue = 170, hmaxBlue = 230, sminBLUE = 70, smaxBLUE = 250, vminBLUE = 75, vmaxBLUE = 250;
int hminRed = 0, hmaxRed = 30, sminRed = 70, smaxRed = 250, vminRed = 75, vmaxRed = 250;
int hminGreen = 100, hmaxGreen = 140, sminGreen = 70, smaxGreen = 250, vminGreen = 75, vmaxGreen = 250;
int hminWhite = 290, hmaxWhite = 310, sminWhite = 0, smaxWhite = 30, vminWhite = 150, vmaxWhite = 250;
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
                ofstream fichier("ConstanteReine.csv", ios::app);
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
    //string path=("image.jpg");
    VideoCapture cap(0);
    Mat img/*=imread(path)*/;

    Scalar lowerYel(hminYel / 2, sminYEL, vminYEL);
    Scalar upperYel(hmaxYel / 2, smaxYEL, vmaxYEL);
    Scalar lowerBlue(hminBlue / 2, sminBLUE, vminBLUE);
    Scalar upperBlue(hmaxBlue / 2, smaxBLUE, vmaxBLUE);
    Scalar lowerRed(hminRed / 2, sminRed, vminRed);
    Scalar upperRed(hmaxRed / 2, smaxRed, vmaxRed);
    Scalar lowerGreen(hminGreen / 2, sminGreen, vminGreen);
    Scalar upperGreen(hmaxGreen / 2, smaxGreen, vmaxGreen);
    Scalar lowerWhite(hminWhite / 2, sminWhite, vminWhite);
    Scalar upperWhite(hmaxWhite / 2, smaxWhite, vmaxWhite);

    while (true)
    {
        cap.read(img);
        imshow("img", img);

        cvtColor(img, imgHSV, COLOR_BGR2HSV);
        inRange(imgHSV, lowerYel, upperYel, maskYel);
        inRange(imgHSV, lowerBlue, upperBlue, maskBlue);
        inRange(imgHSV, lowerRed, upperRed, maskRed);
        inRange(imgHSV, lowerGreen, upperGreen, maskGreen);
        inRange(imgHSV, lowerWhite, upperWhite, maskWhite);

        detectShape(maskYel, "jaune", img);
        detectShape(maskBlue, "bleu", img);
        detectShape(maskRed, "rouge", img);
        detectShape(maskGreen, "vert", img);
        detectShape(maskWhite, "Blanc", img);

        waitKey(150);
    }
}