#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace cv;
using namespace std;

Mat imgHSV, maskYel, maskBlue;
int hminYel = 45, hmaxYel = 95, sminYEL = 115, smaxYEL = 250, vminYEL = 135, vmaxYEL = 250;
int hminBlue = 170, hmaxBlue = 230, sminBLUE = 70, smaxBLUE = 250, vminBLUE = 75, vmaxBLUE = 250;
const int MIN_AREA = 500;
 
void detectShape(Mat& mask, const string& colorName, Mat& img) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > MIN_AREA) {
            drawContours(img, contours, (int)i, Scalar(0, 255, 0), 2);

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

    while (true)
    {
        cap.read(img);
        imshow("img", img);

        Scalar lowerYel(hminYel / 2, sminYEL, vminYEL);
        Scalar upperYel(hmaxYel / 2, smaxYEL, vmaxYEL);
        Scalar lowerBlue(hminBlue / 2, sminBLUE, vminBLUE);
        Scalar upperBlue(hmaxBlue / 2, smaxBLUE, vmaxBLUE);

        cvtColor(img, imgHSV, COLOR_BGR2HSV);
        inRange(imgHSV, lowerYel, upperYel, maskYel);
        inRange(imgHSV, lowerBlue, upperBlue, maskBlue);

        detectShape(maskYel, "jaune", img);
        detectShape(maskBlue, "bleu", img);
        imshow("imgY", maskYel);
        imshow("imgB", maskBlue);

        waitKey(150);
    }
}