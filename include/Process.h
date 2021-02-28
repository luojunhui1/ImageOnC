//
// Created by root on 2021/1/30.
//

#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include "ModelTrans.h"
#include "Config.h"
//#include "Net.h"

//opencv only used for displaying images
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

class Process
{
public:
    ModelTrans trans;

    char* area[2];
    char* letters[5];

    bool inited;
public:
    Process()
    {
        inited = false;
    };

    Process(const char* path)
    {
        trans.Init(path);
        inited = true;
    }

    void Update(const char* path);
    void Init();
    void Display(const char* name, bool save);
};

/**
 * @brief initialize the pointers *area and *letters, which stores the data of the clipped image, formatted as 24*48
 */
void Process::Init()
{
    /*clone G channel data to gChannel pointer*/
    /*ATTENTION!!! because */
//    char* gChannel;
//    gChannel = (char*)malloc(sizeof(char)*(trans.size));
//    trans.CloneChannel(gChannel,1);

    /*temporary variable*/
    char* area_[2];
    char* letters_[5];

    /*clip single channel image(gChannel) and allocate memory for area_[2] and letters_[5]*/
    area_[0] = (char*)malloc(sizeof(char)*(CLIP_HEIGHT*CLIP_WIDTH));
    if(area_[0] == nullptr)
        perror("Error When Allocate Memory for *area_");
    trans.Clip(trans.channelImage[1],area_[0],CLIP_AREA_START_X,CLIP_START_Y,CLIP_WIDTH,CLIP_HEIGHT);

    area_[1] = (char*)malloc(sizeof(char)*(CLIP_HEIGHT*CLIP_WIDTH));
    if(area_[1] == nullptr)
        perror("Error When Allocate Memory for *area_");
    trans.Clip(trans.channelImage[1],area_[1],CLIP_AREA_START_X + CLIP_WIDTH,CLIP_START_Y,CLIP_WIDTH,CLIP_HEIGHT);

    for(int i = 0; i < 5; i++)
    {
        letters_[i] = (char*)malloc(sizeof(char)*(CLIP_HEIGHT*CLIP_WIDTH));
        if(letters_[i] == nullptr)
            perror("Error When Allocate Memory for *letters_");
    }
    int letterClipX = CLIP_LETTERS_START_X;
    for(int i = 0; i < 5; i++)
    {
        trans.Clip(trans.channelImage[1],letters_[i],letterClipX,CLIP_START_Y,CLIP_WIDTH,CLIP_HEIGHT);
        letterClipX += CLIP_WIDTH;
    }

//    delete [] gChannel;

    /*format the letters image to 24*48*/
    area[0] = (char*)malloc(sizeof(char)*(BLOCK_WIDTH*BLOCK_HEIGHT));
    area[1] = (char*)malloc(sizeof(char)*(BLOCK_WIDTH*BLOCK_HEIGHT));
    if(area[0] == nullptr || area[1] == nullptr)
        perror("Error When Allocate Memory for *area");

    trans.PyrDown(area_[0],area[0],CLIP_WIDTH,CLIP_HEIGHT);
    trans.PyrDown(area_[1],area[1],CLIP_WIDTH,CLIP_HEIGHT);

    for(int i = 0; i < 5; i++)
    {
        letters[i] = (char*)malloc(sizeof(char)*(BLOCK_WIDTH*BLOCK_HEIGHT));
        if(letters[i] == nullptr)
            perror("Error When Allocate Memory for *letters");
        trans.PyrDown(letters_[i],letters[i],CLIP_WIDTH,CLIP_HEIGHT);
    }

    /*free memory*/
    delete [] area_[0];
    delete [] area_[1];

    for(int i = 0; i < 5; i++)
    {
        delete [] letters_[i];
    }
}

void Process::Display(const char* name = "", bool save = false)
{
    Mat area0 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,area[0]);
    imshow("area0",area0);

    Mat area1 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,area[1]);
    imshow("area1",area1);

    Mat letter0 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,letters[0]);
    imshow("letter0",letter0);

    Mat letter1 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,letters[1]);
    imshow("letter1",letter1);

    Mat letter2 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,letters[2]);
    imshow("letter2",letter2);

    Mat letter3 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,letters[3]);
    imshow("letter3",letter3);

    Mat letter4 = Mat(BLOCK_HEIGHT,BLOCK_WIDTH,CV_8UC1,letters[4]);
    imshow("letter4",letter4);

    while (waitKey(20) != 27){};

    char* saveDir;
    saveDir = (char*)malloc(sizeof(char)*30);

    if(save)
    {
        char* han = (char*)malloc(sizeof(char)*4);

        memset(han,'\0',4);
        memcpy(han,name,3);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%s.bmp", han);
        imwrite(saveDir,area0);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[3]);
        imwrite(saveDir,area1);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[4]);
        imwrite(saveDir,letter0);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[5]);
        imwrite(saveDir,letter1);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[6]);
        imwrite(saveDir,letter2);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[7]);
        imwrite(saveDir,letter3);

        memset(saveDir,'\0',30);
        sprintf(saveDir, "../Letters/%c.bmp", name[8]);
        imwrite(saveDir,letter4);
    }
}

void Process::Update(const char* path)
{
    /*free memory*/
    if(inited)
    {
        delete [] area[0];
        delete [] area[1];

        for(int i = 0; i < 5; i++)
        {
            delete [] letters[i];
        }
        trans.FreeMemroy();
    }

    trans.Init(path);
    Init();

    if(!inited)inited = true;
}
#endif //TEST_PROCESS_H
