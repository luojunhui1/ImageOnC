//
// Created by root on 2021/2/7.
//
//opencv only used for displaying images
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "Net.h"
#include "ModelTrans.h"
#include "FileProcess.h"

using namespace cv;

int main() {
    ModelTrans trans;
    Net net;

    net.Init();

    char* fileDirect;
    fileDirect = (char*)malloc(sizeof(char)*100);
    char ImagePath[100];

    char *name;
    name =  (char*)malloc(sizeof(char)*4);
    char *saveImageName;
    saveImageName =  (char*)malloc(sizeof(char)*20);

    printf("[BPNN] TRAIN STARTING...\n");
    int train_c = 0;
    double E = 0;
    while (train_c < LOOP_N)
    {
#ifdef TRAIN_HAN
        FileReader fileReader("../Han");
#endif

#ifdef TRAIN_LETTER
    FileReader fileReader("../Letters_Test");
#endif
        E = 0;
        while(fileReader.findNext(fileDirect)) {
#ifdef TRAIN_HAN
            sprintf(ImagePath, "../Han/%s", fileDirect);
#endif

#ifdef TRAIN_LETTER
    sprintf(ImagePath, "../Letters_Test/%s", fileDirect);
    //printf("%s\n",fileDirect);
#endif
            trans.Init(ImagePath);

            memset(name, '\0', 4);

#ifdef TRAIN_HAN
            memcpy(name, fileDirect, 3);
#endif

#ifdef TRAIN_LETTER
    memcpy(name, fileDirect, 1);
#endif

            net.InputSample(trans.channelImage[0], trans.getHeight(), trans.getWidth(), name);
            net.Conv();
            net.Pool();
            net.Train();
#if SAVE_POOL==1
            memset(saveImageName,'\0',20);
            strcat(saveImageName,name);
            strcat(saveImageName,"pool1.jpg");
            imwrite(saveImageName,pool1Src);

            memset(saveImageName,'\0',20);
            strcat(saveImageName,name);
            strcat(saveImageName,"pool2.jpg");
            imwrite(saveImageName,pool2Src);

#endif
            E += net.ek;
        }

#ifdef TRAIN_HAN
        E /= OUT_N_1;
#endif
#ifdef TRAIN_LETTER
    E /= OUT_N_3;
#endif

        train_c++;

        printf("[BPNN] LOOP %d E %lf\n", train_c, E);
        if (E < E_MIN)
            break;
    }
    net.SaveParam();

    return 0;
}
