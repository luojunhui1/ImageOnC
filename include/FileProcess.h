//
// Created by root on 2021/2/2.
//

#ifndef FPGA_FILEPROCESS_H
#define FPGA_FILEPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

/**structure in dirent.h for reading directories**/
//struct dirent
//{
//    long d_ino; /* inode number 索引节点号 */
//    off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
//    unsigned short d_reclen; /* length of this d_name 文件名长 */
//    unsigned char d_type; /* the type of d_name 文件类型 */
//    char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
//}

class FileReader
{
public:
    DIR *dir;
    struct dirent *ptr;
public:
    FileReader(char* baseFileDir);
    ~FileReader();
    bool findNext(char* fileName);
};

FileReader::FileReader(char *basePath) {
    if ((dir = opendir(basePath)) == NULL) {
        perror("Open dir error...");
        exit(1);
    }
}

bool FileReader::findNext(char* fileName)
{
    if((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
        {
            /*even for reading the same directory,sometimes d_name may be . or .., just ignore it then it can work well*/
            //printf("File Reader end by . or ..");
            if((ptr=readdir(dir)) == NULL)return false;
        }
        if(ptr->d_type == 8 || ptr->d_type == 10)    ///file or link file
        {
            //printf("In FileReader: %s\n",ptr->d_name);
            memset(fileName,'\0',sizeof(fileName));
            strcpy(fileName,ptr->d_name);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            printf("Base File Directory including sub-directory");
            return false;
        }
        return true;
    }
    return false;
}
FileReader::~FileReader()
{
    closedir(dir);
}
//int main(void)
//{
//    DIR *dir;
//    char basePath[1000];
//
//    ///get the current absoulte path
//    memset(basePath,'\0',sizeof(basePath));
//    getcwd(basePath, 999);
//    printf("the current dir is : %s\n",basePath);
//
//    ///get the file list
//    memset(basePath,'\0',sizeof(basePath));
//    strcpy(basePath,"./XL");
//    readFileList(basePath);
//    return 0;
//}
#endif //FPGA_FILEPROCESS_H
