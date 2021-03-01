//#include "Process.h"
//#include "FileProcess.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <vector>

#include "Eigen/Eigen"
#include "Eigen/Core"
#include "Eigen/Dense"
#include <iostream>

#include "Net.h"
#include "FileProcess.h"
#include "Process.h"

using namespace Eigen;
using namespace std;
using namespace cv;

int main() {
    Process pro;
    Model model;

    if(!model.LoadParam())return 0;

    FileReader fileReader("../database");
    char* fileDirect;
    fileDirect = (char*)malloc(sizeof(char)*100);
    char ImagePath[100];

    char *letter;
    letter =  (char*)malloc(sizeof(char)*4);
    char *name;
    name =  (char*)malloc(sizeof(char)*20);
    char *saveImageName;
    saveImageName =  (char*)malloc(sizeof(char)*20);
    double possibility = 1;

    int count = 0;
    while(fileReader.findNext(fileDirect))
    {
        sprintf(ImagePath, "../database/%s", fileDirect);
        pro.Update(ImagePath);
//        pro.Display(fileDirect,true,count++);
        possibility = 1;
        memset(name,'\0',20);
        memset(letter,'\0',4);
        memset(saveImageName,'\0',20);

        possibility *= model.Fit(pro.area[0],BLOCK_HEIGHT,BLOCK_WIDTH,letter,true);
#if SAVE_POOL==1
        strcat(saveImageName,letter);
        strcat(saveImageName,"pool1.jpg");
        imwrite(saveImageName,pool1Src);

        memset(saveImageName,'\0',20);
        strcat(saveImageName,letter);
        strcat(saveImageName,"pool2.jpg");
        imwrite(saveImageName,pool2Src);
#endif
        strcat(name,letter);

        memset(letter,'\0',4);
        possibility *= model.Fit(pro.area[1],BLOCK_HEIGHT,BLOCK_WIDTH,letter,false);
        if(strcmp(letter,"0") == 0)letter[0] = 'O';
        strcat(name,letter);

        for(int i = 0; i < 5; i++)
        {
            memset(letter,'\0',4);
            possibility *= model.Fit(pro.letters[i],BLOCK_HEIGHT,BLOCK_WIDTH,letter,false);
#if SAVE_POOL==1
            memset(saveImageName,'\0',20);
            strcat(saveImageName,letter);
            strcat(saveImageName,"pool1.jpg");
            imwrite(saveImageName,pool1Src);

            memset(saveImageName,'\0',20);
            strcat(saveImageName,letter);
            strcat(saveImageName,"pool2.jpg");
            imwrite(saveImageName,pool2Src);
#endif
            strcat(name,letter);
        }

        cout<<"NAME: "<<name<<" POSSIBILITY: "<<1 - possibility<<endl;
        Mat src = imread(ImagePath);
        pyrDown(src,src);
        pyrDown(src,src);
        while(waitKey(20) != 27)
        {
            imshow("src",src);
        }
   }
//    Mat src = imread("/home/ljh/文档/FPGA/Test/1613009717927.jpg");
//    Mat dst,o,al;
//    pyrDown(src,dst);
//    pyrDown(dst,dst);
//    Mat channels[3];
//    split(dst,channels);
//
//    uchar lutData[256];
//    for(int i = 0; i < 256; i++)
//    {
//        lutData[i] = (i > 170)?(255):(0);
//    }
//    Mat lut(1, 256, CV_8UC1, lutData);
//    LUT(channels[0], lut, al);
//
//    for(int i = 0 ;i < dst.rows; i++)
//    {
//        for( int j = 0; j < dst.cols; j++)
//        {
//            if(al.at<uchar>(i,j) == 255)continue;
//            al.at<uchar>(i,j) = (channels[1].at<uchar>(i,j) > 120 || channels[2].at<uchar>(i,j) > 160)?(200)
//                    :((channels[1].at<uchar>(i,j) > 80 || channels[2].at<uchar>(i,j) > 100)?(180):(0));
//        }
//    }
//    Mat z = Mat(dst.rows,dst.cols,CV_8UC1);
//
//
//    vector<Mat> channels1;
//    channels1.push_back(z);
//    channels1.push_back(z);
//    channels1.push_back(al);
//    merge(channels1,o);
//
//    while (waitKey(20) != 27)
//    {
//        imshow("b",channels[0]);
//        imshow("g",channels[1]);
//        imshow("r",channels[2]);
//        imshow("o",o);
//    }
}
