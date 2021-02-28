//
// Created by root on 2021/2/4.
//

#ifndef FPGA_CONFIG_H
#define FPGA_CONFIG_H

#define CLIP_WIDTH 72
#define CLIP_HEIGHT 144
#define CLIP_AREA_START_X 20
#define CLIP_LETTERS_START_X 202
#define CLIP_START_Y 23

#define BLOCK_WIDTH 24
#define BLOCK_HEIGHT 48

#define POOL_N1_H 24
#define POOL_N1_W 12

#define POOL_N2_H 12
#define POOL_N2_W 6
#define POOL_N2_CHANNEL 1


#define PADDROW 50
#define PADDCOL 26

#define IN_N 72           /* INPUT NODE */
#define OUT_N_1 32           /* OUTPUT_NODE for area[0]*/
#define OUT_N_2 25           /* OUTPUT_NODE for area[1]*/
#define OUT_N_3 35           /* OUTPUT_NODE for area[2]*/
#define HIDDEN_N 64          /* HIDDEN_NODE */

/*for this order of magnitude,  need to store 166,000 float data at least, that is, 646KB memory*/

#define CONVKS 3
#define CONV_CHANNEL 8

#define MAXPOOLKS 2

#define ACTIVE(X) (1/(1+exp(-(X))))
#define SOFTMAX(X,SUM) (X/SUM)

#define LOOP_N 300        /* LOOP NUMBER */
#define E_MIN 0.00001    /* Cumulative error */
#define LEARN_RATE1 0.3
#define LEARN_RATE2 0.4

static float pryDownKernel[3][3] = {{0,0,0},{0,1,0},{0,0,0}};

const char hanNames[32][20] = {"桂","鲁","陕","皖","黑","鄂","津","滇","新","冀","贵","台","赣","京","渝","吉","沪","琼","豫"
                                ,"蒙","粤","青","辽","湘","闽","宁","晋","浙","藏","甘","苏","川"};

const char letterNames[35][20] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","I","J","K",
                                   "L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z"};

#define SAVE_PARAM_PATH "/home/ljh/文档/FPGA/Test/param.txt"
#define SAVE_PARAM_PATH1 "/home/ljh/文档/FPGA/Test/paramLetters.txt"

#define TRAIN_LETTER 1
//#define TRAIN_HAN 1
#endif //FPGA_CONFIG_H
