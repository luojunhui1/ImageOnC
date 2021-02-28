//
// Created by root on 2021/2/25.
//
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "Net.h"
#include "ModelTrans.h"
#include "FileProcess.h"

using namespace cv;
int main()
{
    ModelTrans trans;
    Net net;

    net.Init();
    if(!net.LoadParam())return 0;

    char* fileDirect;
    fileDirect = (char*)malloc(sizeof(char)*100);
    char ImagePath[100];

    char *name;
    name =  (char*)malloc(sizeof(char)*4);

    Mat src;

    FileReader fileReader("../Han_Test");
    while(fileReader.findNext(fileDirect)) {
        sprintf(ImagePath, "../Han/%s", fileDirect);
        trans.Init(ImagePath);

        memset(name, '\0', 4);
        memcpy(name, fileDirect, 3);

        net.Fit(trans.channelImage[0], trans.getHeight(), trans.getWidth(), name);

        src = imread(ImagePath);
        while (waitKey(20) !=  27)
        {
            imshow("src",src);
        }
    }

    return 0 ;
}

