//
// Created by Cirno on 2020/4/28.
//

#ifndef SIMPLEPHOTOSHOP_SAVELOAD_H
#define SIMPLEPHOTOSHOP_SAVELOAD_H

#include <cstdio>
#include <cstring>
#include <cstdlib>

//C++ only used to format output
#include <iostream>
using namespace std;

struct BmpFileHeader //文件头, 14字节
{
    //unsigned short bfType;  //2字节，文件类型，标识该文件为bmp文件,判断文件是否为bmp文件，即用该值与"0x4d42"比较是否相等即可，0x4d42 = 19778
    unsigned int bfSize;   //4字节，文件大小
    unsigned short bfReserved1; //2字节，预保留位,必须设置为0
    unsigned short bfReserved2; //2字节，预保留位,必须设置为0
    unsigned int bfOffBits;    //4字节，从头到位图数据的偏移，即图像数据区的起始位置
};

struct BmpInfoHeader //信息头, 40字节
{
    unsigned int biSize;    //4字节，信息头的大小，即40
    int biWidth;    //4字节，以像素为单位说明图像的宽度
    int biHeight;    //4字节，以像素为单位说明图像的高度，同时如果为正，说明位图倒立（即数据表示从图像的左下角到右上角），如果为负说明正向
    unsigned short biPlanes;    //2字节，为目标设备说明颜色平面数，总被设置为1
    unsigned short biBitCount;  //2字节，说明比特数/像素数，8-灰度图；24-真彩色
    unsigned int biCompression;    //4字节，说明图像的压缩类型，最常用的就是0（BI_RGB），表示不压缩
    unsigned int biSizeImage;      //4字节，说明位图数据的大小，当用BI_RGB格式时，可以设置为0
    int biXPelsPerMeter;  //表示水平分辨率，单位是像素/米，有符号整数
    int biYPelsPerMeter;  //表示垂直分辨率，单位是像素/米，有符号整数
    unsigned int biClrUsed;    //说明位图使用的调色板中的颜色索引数，为0说明使用所有
    unsigned int biClrImportant;  //说明对图像显示有重要影响的颜色索引数，为0说明都重要
};

struct Palette //调色板
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char alpha; //预保留位
};

/**
 * describe the bmp image(32 bit, BGRA foramat)
 */
class BmpImage {
public:
    BmpImage() = default;
    unsigned char* pData; //Image Data pointer
    unsigned int iBufSize; //size of the image pixel data, not include the headers
    BmpFileHeader bmpFileHeader; //bmp file header
    BmpInfoHeader bmpInfoHeader; //bmp information header

    int getHeight() const {
        return height;
    }

    int getWidth() const {
        return width;
    }
    void setWidth(int newWidth){
        width = newWidth;
        bmpInfoHeader.biWidth = newWidth;
    }

    void setHeight(int newHeight) {
        height = newHeight;
        bmpInfoHeader.biHeight = newHeight;
    }
    int getChannels() const {
        return channels;
    }

    bool LoadImage(const char* path);

    void ShowBMPInfo();

protected:
    int width; //长度
    int height; //宽度
    int channels; //通道数
    Palette* palette; //调色板数据
    unsigned char* pBuf;  //buffer pointer
};

bool BmpImage::LoadImage(const char* path) {

    if (path == nullptr || *path == '\0')
        return false;

    FILE* file = fopen(path, "rb");

    if (file)
    {
        unsigned char* imageBuffer = nullptr;
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        if (length > 0)
        {
            rewind(file);
            imageBuffer = (unsigned char*)malloc(sizeof(unsigned char)*(length + 1));
            fread(imageBuffer, sizeof(unsigned char), length, file);
            imageBuffer[length] = '\0';
            pBuf = imageBuffer; //pBuf point to the hold image data, including the headers
        }
        fclose(file);

        unsigned char* pBuffer = pBuf;

        if (0x4D42 == *((unsigned short*)pBuffer))
        {
            pBuffer += 2;
            memcpy(&bmpFileHeader, pBuffer, sizeof(BmpFileHeader));//read bmp file header information
            pBuffer += sizeof(BmpFileHeader);
            memcpy(&bmpInfoHeader, pBuffer, sizeof(BmpInfoHeader));//read bmp information header

            width = bmpInfoHeader.biWidth;
            height = bmpInfoHeader.biHeight;

            channels = bmpInfoHeader.biBitCount/8;

            unsigned short PerLine = width*channels;// each row in bmp image including PerLine bytes
            short offset = PerLine % 4; // because the image format is BGRA, so the offset is always 0
            if (offset != 0)
                PerLine += (4 - offset);
            unsigned long FileBufSize = PerLine * height;
            iBufSize = FileBufSize;

            pBuf = pBuf + bmpFileHeader.bfOffBits;
            pBuffer = (unsigned char*)malloc(sizeof(unsigned char)*(iBufSize));
            memcpy((void *) pBuffer, pBuf, iBufSize);

//            pData = pBuffer;
//            for (unsigned long i = 0; i < iBufSize; i += channels)
//            {
//                // 交换R-B两个通道.
//                unsigned char temp = pData[0];
//                pData[0] = pData[2];
//                pData[2] = temp;
//                pData += channels;
//            }

            // 高度为正数，需要做垂直翻转.
            if (height > 0)
            {
                unsigned char* pBuff = (unsigned char*)malloc(sizeof(unsigned char)*(iBufSize));
                memcpy(pBuff, pBuffer, iBufSize);
                // 最后一行尾地址
                pBuff += iBufSize;
                for (int m = 0; m < height; ++m)
                {
                    pBuff -= PerLine;
                    memcpy(pBuffer, pBuff, PerLine);
                    pBuffer += PerLine;
                }
                pBuffer -= iBufSize;
                delete[] pBuff;
            }
            delete [] imageBuffer;
            pData = pBuffer;
            return true;
        }
    }
    return false;

}


void BmpImage::ShowBMPInfo() {
//    if (!is_loaded()) {
//        cerr << "Bmp File Not Loaded, Cannot Tell The Information!" << endl;
//        return;
//    }
    //cout << "文件类型: " << hex << bmpFileHeader.bfType << endl;
    cout << "文件大小: " << dec << bmpFileHeader.bfSize << endl;
    cout << "保留字1: " << dec << bmpFileHeader.bfReserved1 << endl;
    cout << "保留字2: " << dec << bmpFileHeader.bfReserved2 << endl;
    cout << "数据偏移: " << dec << bmpFileHeader.bfOffBits << endl;
    cout << "信息头大小: " << dec << bmpInfoHeader.biSize << endl;
    cout << "图片宽度: " << dec << bmpInfoHeader.biWidth << endl;
    cout << "图片高度: " << dec << bmpInfoHeader.biHeight << endl;
    cout << "图片颜色平面数: " << dec << bmpInfoHeader.biPlanes << endl;
    cout << "图片位深度: " << dec << bmpInfoHeader.biBitCount << endl;
    cout << "图片压缩类型: " << dec << bmpInfoHeader.biCompression << endl;
    cout << "位图数据大小: " << dec << bmpInfoHeader.biSizeImage << endl;
    cout << "水平分辨率: " << dec << bmpInfoHeader.biXPelsPerMeter << endl;
    cout << "垂直分辨率: " << dec << bmpInfoHeader.biYPelsPerMeter << endl;
    cout << "调色板颜色索引数: " << dec << bmpInfoHeader.biClrUsed << endl;
    cout << "重要颜色索引数: " << dec << bmpInfoHeader.biClrImportant << endl;

}

#endif //SIMPLEPHOTOSHOP_SAVELOAD_H