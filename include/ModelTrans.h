#pragma once

#include "SaveLoad.h"
#include "Config.h"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

typedef unsigned short ushort;
typedef unsigned int uint_;
typedef unsigned char uchar;

class ModelTrans: public BmpImage{
public:
    char* channelImage[3];
    int size;

    bool isLoad;
    void Init(const char* path);
    void CloneChannel(char* Channel,int channel);
    void DrawVerticalLine(char* pImage, int x, int startY, int endY, int width);
    void DrawHorizonLine(char* pImage, int y, int startX, int endX,int width);
    void Clip(char* pImage, char* pClipedImage, int ltX, int ltY, int width, int height,int imageWidth);
    void PyrDown(char* pImage,char* dst, int width, int height);
    void FreeMemroy();

    ModelTrans()
    {
        isLoad = false;
    };

    explicit ModelTrans(const char* path) : BmpImage() {
        isLoad = false;
        Init(path);
    }
};

/**
 * @brief initialize the Modeltrans object by inpput a bmp image file directory
 * @param path the directory of a bmp image
 */
void ModelTrans::Init(const char *path)
{
    if(LoadImage(path))
    {
        unsigned char* pBuffer = nullptr;
        pBuffer = pData;
        size = width*height;

        channelImage[0] = (char*)malloc(sizeof(char)*(iBufSize));
        channelImage[1] = (char*)malloc(sizeof(char)*(iBufSize));
        channelImage[2] = (char*)malloc(sizeof(char)*(iBufSize));

        char *bChannelImage,*gChannelImage,*rChannelImage;
        bChannelImage = channelImage[0];
        gChannelImage = channelImage[1];
        rChannelImage = channelImage[2];

        for (int i = 0; i < size; ++i)
        {
            switch (channels) {
            case 4:
            case 3:
                memcpy(rChannelImage, pBuffer + 2, 1 );
                rChannelImage++;
            case 2:
                memcpy(gChannelImage, pBuffer + 1, 1 );
                gChannelImage++;
            case 1:
                memcpy(bChannelImage, pBuffer, 1 );
                bChannelImage++;
            pBuffer += channels;
            }
//            memcpy(bChannelImage, pBuffer, 1 );
//            bChannelImage++;
//            pBuffer++;
//
//            memcpy(gChannelImage, pBuffer, 1 );
//            gChannelImage++;
//            pBuffer ++;
//
//            memcpy(rChannelImage, pBuffer, 1 );
//            rChannelImage++;
//            pBuffer++;
//
//            //BGRA format, 4 channels
//            pBuffer++;
        }

        isLoad = true;
    }

}

/**
 * @brief get different channel data for bmp image, the bmp data format is BGRA, and you should allocate memory for bChannel
 * before calling this function
 * @param pChannel pointer to the channel data
 * @param channel which channel data wanted
 */
void ModelTrans::CloneChannel(char* pChannel,int channel)//channel: 0 blue 1 green 2 red
{
    memcpy(pChannel,channelImage[channel],size);
}

/**
 * @brief call this function when you need to draw a vertical line on your single channel image
 * @param pImage the pointer point to the single channel image
 * @param x x index of the vertical linr
 * @param startY start row of the vertical line
 * @param endY end row of the vertical line
 * @param width width of the single channel image
 * @param channel not used
 */
void ModelTrans::DrawVerticalLine(char* pImage, int x, int startY, int endY,int width = 0)
{
    if(width == 0)width = this->width;
    for(int i = startY; i < endY; i++)
    {
        pImage[i*width + x] = static_cast<char>(255);
    }
}

/**
 * @brief draw a horizontal line on a single channel image
 * @param pImage the pointer reference to a single channel image
 * @param y the y coordinate index of the horizontal line
 * @param startX the start x coordinate index of the horizontal line
 * @param endX the end x coordinate index of the horizontal line
 * @param width the width of the original image
 */
void ModelTrans::DrawHorizonLine(char* pImage, int y, int startX, int endX,int width = 0)
{
    if(width == 0)width = this->width;
    for(int i = startX; i < endX; i++)
    {
        pImage[y*width + i] = static_cast<char>(255);
    }
}

/**
 * @brief clip numbers or letters from image
 * @param pImage original image pointer
 * @param pClipedImage pointer to the memery which stored clipped image
 * @param ltX the x coordinate of left top point
 * @param ltY the y coordinate of left top point
 * @param width_ the width of the clipped image, which should  be set as 72
 * @param height_ the height of the clipped image, which should  be set as 144
 * @param imageWidth the width of the original image
 */
void ModelTrans::Clip(char* pImage, char* pClipedImage, int ltX, int ltY, int width_, int height_,int imageWidth = 0)
{
    if(imageWidth == 0)imageWidth = width;
    if(imageWidth < ltX + width_)
        perror("Clip Error, image width < leftTop X + width_");
//
//    pClipedImage = (char*)malloc(sizeof(char)*(width_*height_));

    int rbY = ltY + height_;

    char* curP = pClipedImage;
    char* curPImage = pImage;

    curPImage += ltY*imageWidth + ltX;
    for(int i = ltY; i < rbY; i++)
    {
        memcpy(curP, curPImage, width_);
        curP += width_;
        curPImage += imageWidth;
    }
}

/**
 * @brief reduce the image size to 1/3, this function only useful when the width and height is interger of mutiples 3
 * @param pImage original image
 * @param dst target image
 */
void ModelTrans::PyrDown(char* pImage,char* dst, int width = 72, int height = 144)
{
    int newWidth = width/3;
    int curIndex = 0;
    for(int i = 0; i < height; i += 3)
    {
        curIndex = i*width;
        for(int j = 0; j < width; j += 3)
        {
            dst[(i/3)*newWidth + j/3] = pImage[curIndex]*pryDownKernel[0][0] + pImage[curIndex + 1]*pryDownKernel[0][1] +
                                        pImage[curIndex + 2]*pryDownKernel[0][2] + pImage[curIndex + width]*pryDownKernel[1][0] + pImage[curIndex + width + 1]*pryDownKernel[1][1] +
                                        pImage[curIndex + width + 2]*pryDownKernel[1][2] + pImage[curIndex + 2*width]*pryDownKernel[2][0] + pImage[curIndex + 2*width + 1]*pryDownKernel[2][1]+
                                        pImage[curIndex + 2*width + 2]*pryDownKernel[2][2];
            curIndex += 3;
        }
    }
}

void ModelTrans::FreeMemroy()
{
    delete [] channelImage[0];
    delete [] channelImage[1];
    delete [] channelImage[2];
    delete [] pData;
}
