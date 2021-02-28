//
// Created by root on 2021/2/2.
//

#ifndef FPGA_NET_H
#define FPGA_NET_H

#include "Config.h"

#include "Eigen/Eigen"
#include "Eigen/Core"
#include "Eigen/Dense"
#include "unsupported/Eigen/CXX11/Tensor"

//using c++ cout for output
#include <iostream>

//opencv for display
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>

using namespace Eigen;
using namespace std;
using namespace cv;

Mat pool1Src;
Mat pool2Src;

#define D   IN_N
#define Q   HIDDEN_N

#ifdef TRAIN_HAN
    #define L   OUT_N_1
#endif
#ifdef TRAIN_LETTER
        #define L   OUT_N_3
    #endif

class Net
{
public:
    Matrix<float, BLOCK_HEIGHT, BLOCK_WIDTH> image;
    Matrix<float, PADDROW, PADDCOL> pad;
    Matrix<Matrix<float,BLOCK_HEIGHT,BLOCK_WIDTH>,CONV_CHANNEL,1> convData;
    Matrix<Matrix<float,POOL_N1_H,POOL_N1_W>,CONV_CHANNEL,1> pool1;
    Matrix<float,POOL_N1_H,POOL_N1_W> accum;
    Matrix<float,POOL_N2_H,POOL_N2_W> pool2;

    Matrix<Matrix<float,CONVKS,CONVKS>,CONV_CHANNEL,1> convKernel;


    Matrix<float, D, Q> v;
    Matrix<float, Q, L> w;

    Matrix<float, D, 1> r;
    Matrix<float, L, 1> o;

    Matrix<float, L, 1> g;
    Matrix<float, Q, 1> e;

    Matrix<float, D, 1> x;
    Matrix<float, Q, 1> b;
    Matrix<float, L, 1> y;
    Matrix<float, L, 1> yc;

    double ek{};

public:
    Net();
    void Init();
    void InputSample(const char *p, int row, int col, const char *name);
//    void Padding(int horizonPadding, int verticalPadding, int type);

    void Conv();
    void Pool();
    void Train();
    bool SaveParam();
};

class Model: public Net
{
private:
    Matrix<float, D, Q> v1;
    Matrix<float, Q, OUT_N_1> w1;

    Matrix<float, D, 1> r1;
    Matrix<float, OUT_N_1, 1> o1;

    Matrix<float, D, Q> v2;
    Matrix<float, Q, OUT_N_3> w2;

    Matrix<float, D, 1> r2;
    Matrix<float, OUT_N_3, 1> o2;

    Matrix<float, OUT_N_1, 1> out1;
    Matrix<float, OUT_N_3, 1> out2;

public:
    Model();
    bool LoadParam();
    double Fit(const char *p, int row, int col, char* name, bool type);
};


Net::Net()
= default;

void Net::InputSample(const char *p, int row, int col, const char *name)
{
    if(row != BLOCK_HEIGHT || col != BLOCK_WIDTH)
        perror("row != BLOCK_HEIGHT || col != BLOCK_WIDTH");
    int index = 0;
    pad = MatrixXf::Zero(PADDROW,PADDCOL);

    for(int i = 0; i < row; i++)
    {
        for(int j = 0; j < col; j++)
        {
            image(i,j) = static_cast<uchar>(*(p + index));
            pad(i + 1, j + 1) = image(i,j);
            index++;
        }
    }

    //It's Stupid to use .block() to assign value for Matrix


    for(int i = 0; i < L; i++)
    {
#ifdef TRAIN_HAN
        y[i] = (strcmp(hanNames[i], name) == 0)?(1):0;
#endif

#ifdef TRAIN_LETTER
    y[i] = (strcmp(letterNames[i], name) == 0)?(1):0;
#endif
    }
}


void Net::Init()
{
    v = MatrixXf::Random(D,Q);/*[-1,1]*/
    w = MatrixXf::Random(Q,L);

    r = ArrayXf::Random(D,1);
    o = ArrayXf::Random(L,1);

    g = ArrayXf::Random(L,1);
    e = ArrayXf::Random(Q,1);

    x = ArrayXf::Random(D,1);
    b = ArrayXf::Random(Q,1);
    y = ArrayXf::Random(L,1);
    yc = ArrayXf::Random(L,1);

    Matrix<float,3,3> cur;

    cur<<-1.0f,0.0f,1.0f,-2.0f,0.0f,2.0f,1.0f,0.0f,-1.0f;
    convKernel(0,0) = cur;//X Direction Sobel

    cur<<1.0f,2.0f,1.0f,0.0f,0.0f,0.0f,-1.0f,-2.0f,-1.0f;
    convKernel(1,0) = cur;//Y Direction Sobel

    cur<<-1.0f,-1.0f,-1.0f,-1.0f,9.0f,-1.0f,-1.0f,-1.0f,-1.0f;
    convKernel(2,0) = cur;//Sharpen Kernel

    cur<<-1.0f,0.0f,1.0f,-1.0f,0.0f,1.0f,-1.0f,0.0f,1.0f;
    convKernel(3,0) = cur;//Right Direction Prewitt

    cur<<1.0f,0.0f,-1.0f,1.0f,0.0f,-1.0f,1.0f,0.0f,-1.0f;
    convKernel(4,0) = cur;//Left Direction Prewitt

    cur<<0.0f,1.0f,0.0f,1.0f,-4.0f,1.0f,0.0f,1.0f,0.0f;
    convKernel(5,0) = cur;//4 - adjacency Laplace

    cur<<1.0f,1.0f,1.0f,1.0f,-8.0f,1.0f,1.0f,1.0f,1.0f;
    convKernel(6,0) = cur;//8 - adjacency Laplace

    cur<<0.11f,0.11f,0.11f,0.11f,0.12f,0.11f,0.11f,0.11f,0.11f;
    convKernel(7,0) = cur;//average kernel

    accum = MatrixXf::Zero(BLOCK_HEIGHT/MAXPOOLKS,BLOCK_WIDTH/MAXPOOLKS);
}

void Net::Conv()
{
    MatrixXf cur1,cur2,cur3;

    for(int k = 0 ;k < CONV_CHANNEL; k++)
    {
        cur2 = convKernel(k,0);
        for(int i = 0; i <= BLOCK_HEIGHT - CONVKS; i++)
        {
            for(int j = 0; j <= BLOCK_WIDTH  - CONVKS; j++)
            {
                if(i < 0 || j < 0)
                    assert("Conv Error!");
                cur1 = pad.block(i,j,CONVKS,CONVKS);
                cur3 = cur1.array()*cur2.array();
                //FUCK YOU EIGEN
                (convData(k,0))(i,j) = cur3.sum();

                //To-Do::Added offset activation function

            }
        }
    }
}

void Net::Pool()
{
    // BLOCK_HEIGHT and MAXPOOLKS are even numbers
    accum = MatrixXf::Zero(BLOCK_HEIGHT/MAXPOOLKS,BLOCK_WIDTH/MAXPOOLKS);

    for(int k = 0 ;k < CONV_CHANNEL; k++)
    {
        for(int i = 0; i < POOL_N1_H; i++)
        {
            for(int j = 0; j < POOL_N1_W; j++)
            {
                if(i < 0 || j < 0)
                    assert("Pool Error!");
                pool1(k,0)(i,j) = (convData(k,0).block(i*2,j*2,MAXPOOLKS,MAXPOOLKS)).mean();
            }
        }
        accum += pool1(k,0);
    }
//    eigen2cv(accum,pool1Src);
//    normalize(pool1Src,pool1Src,0,255,NORM_MINMAX);

    for(int i = 0; i < POOL_N2_H; i++)
    {
        for(int j = 0; j < POOL_N2_W; j++)
        {
            if(i < 0 || j < 0)
                assert("Pool Error!");
            pool2(i,j) = (accum.block(i*2,j*2,MAXPOOLKS,MAXPOOLKS)).maxCoeff();
            //pool2(i,j + POOL_N2_W/2) = (accum.block(i*2,j*2,MAXPOOLKS,MAXPOOLKS)).maxCoeff();
        }
    }

//    eigen2cv(pool2,pool2Src);
//    normalize(pool2Src,pool2Src,0,255,NORM_MINMAX);
}

void Net::Train()
{
    //pool2.normalize();// use sigmod for hidden layer
    MatrixXf cur = pool2;
    cur.resize(D,1);

    /* Compute b[h], hidden layer output */
    for (size_t h = 0; h < Q; h++) {
        double alpha_h = v.col(h).dot(cur.col(0));
//        double alpha_h = 0;
//        for (size_t i = 0; i < D; i++)
//            alpha_h += v(i,h) * cur(i,0);
        b(h,0) = ACTIVE(alpha_h - r(h,0));
    }

    /* Compute yc[j] ,output layer output */
    for (size_t j = 0; j < L; j++) {
        double beta_j = 0;
        beta_j = w.col(j).dot(b.col(0));
//        for (size_t h = 0; h < Q; h++)
//            beta_j += w[h][j] * b[h];
        yc(j,0) = exp(beta_j - o(j,0));
    }
    /*softmax activation*/
    float SUM = yc.sum();
    for (size_t j = 0; j < L; j++)
    {
        yc(j,0) = yc(j,0)/SUM;
    }

    ek = 0;

    /* Compute g[j] and E ,g[i] is the output layer point loss, Ek is the output layer loss*/
    for (size_t j = 0; j < L; j++) {
        g[j] =  (yc[j] - y[j]);// update b for output layer, when update the b, plus g
        ek -= y[j]*log(yc[j]); //
    }

    /* Compute e[h] , e[h] is the hidden layer loss*/
    for (size_t h = 0; h < Q; h++) {
        double temp = 0;
        temp = w.row(h).dot(g);
        e[h] = b[h] * (1 - b[h]) * temp;
    }

    /* Update v[i][h], w[h][j], r[h], o[j] */
    for (size_t i = 0; i < D; i++)
        for (size_t h = 0; h < Q; h++)
            v(i,h) -= LEARN_RATE1 * e[h] * x[i];
    for (size_t h = 0; h < Q; h++)
        for (size_t j = 0; j < L; j++)
            w(h,j) -= LEARN_RATE2 * g[j] * b[h];

    for (size_t h = 0; h < Q; h++)
        r[h] -= ((LEARN_RATE1) * e[h]);

    for (size_t j = 0; j < L; j++)
        o[j] -= ((LEARN_RATE2) * g[j]);
}

bool Net::SaveParam()
{
    FILE *out = NULL;
#ifdef TRAIN_HAN
    out = fopen(SAVE_PARAM_PATH, "w+");
#endif

#ifdef TRAIN_LETTER
    out = fopen(SAVE_PARAM_PATH1, "w+");
#endif

    if (out == NULL) {
        fprintf(stderr, "[BPNN] OPEN FILE %s FAILED.\n", SAVE_PARAM_PATH);
        return false;
    }

    fprintf(out, "#BPNN PARAM GENERATED AUTOMATICALLY IN %d SECONDS SINCE JABUARY 1, 1970\n", (int)time(NULL));
    fprintf(out, "#!!DO NOT MODIFY!!\n");
    fprintf(out, "D=%d\n", D);
    fprintf(out, "Q=%d\n", Q);
    fprintf(out, "L=%d\n", L);
    for (size_t i = 0; i < D; i++)
        for (size_t h = 0; h < Q; h++)
            fprintf(out, "%lf\n", v(i,h));
    for (size_t h = 0; h < Q; h++)
        for (size_t j = 0; j < L; j++)
            fprintf(out, "%lf\n", w(h,j));
    for (size_t h = 0; h < Q; h++)
        fprintf(out, "%lf\n", r[h]);
    for (size_t j = 0; j < L; j++)
        fprintf(out, "%lf\n", o[j]);

    fclose(out);

    return true;
}

Model::Model()
{
    Init();
}
bool Model::LoadParam()
{
#define BUFFER_SIZE     128
    FILE *in = nullptr;
    in = fopen(SAVE_PARAM_PATH, "r");
    if (in == nullptr) {
        fprintf(stderr, "[BPNN] OPEN FILE %s FAILED.\n", SAVE_PARAM_PATH);
        return false;
    }

    char buffer[BUFFER_SIZE];
    for (size_t i = 0; (i < 5) && (fgets(buffer, BUFFER_SIZE, in) != nullptr); i++) {
        if (buffer[0] == '#')
            continue;
        else if (buffer[0] == 'D' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != D) {
                goto cleanup;
            }
        } else if (buffer[0] == 'Q' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != Q) {
                goto cleanup;
            }
        } else if (buffer[0] == 'L' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != OUT_N_1) {
                goto cleanup;
            }
        }
    }

    for (size_t i = 0; i < D; i++)
        for (size_t h = 0; h < Q; h++) {
            if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
                v1(i,h) = strtod(buffer, nullptr);
            } else {
                goto cleanup;
            }
        }
    for (size_t h = 0; h < Q; h++)
        for (size_t j = 0; j < OUT_N_1; j++) {
            if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
                w1(h,j) = strtod(buffer, nullptr);
            } else {
                goto cleanup;
            }
        }
    for (size_t h = 0; h < Q; h++) {
        if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
            r1[h] = strtod(buffer, nullptr);
        } else {
            goto cleanup;
        }
    }
    for (size_t j = 0; j < OUT_N_1; j++) {
        if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
            o1[j] = strtod(buffer, nullptr);
        } else {
            goto cleanup;
        }
    }

    fclose(in);

    in = fopen(SAVE_PARAM_PATH1, "r");
    if (in == nullptr) {
        fprintf(stderr, "[BPNN] OPEN FILE %s FAILED.\n", SAVE_PARAM_PATH1);
        return false;
    }

    for (size_t i = 0; (i < 5) && (fgets(buffer, BUFFER_SIZE, in) != nullptr); i++) {
        if (buffer[0] == '#')
            continue;
        else if (buffer[0] == 'D' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != D) {
                goto cleanup;
            }
        } else if (buffer[0] == 'Q' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != Q) {
                goto cleanup;
            }
        } else if (buffer[0] == 'L' && buffer[1] == '=') {
            if (strtol(buffer + 2, nullptr, 0) != OUT_N_3) {
                goto cleanup;
            }
        }
    }

    for (size_t i = 0; i < D; i++)
        for (size_t h = 0; h < Q; h++) {
            if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
                v2(i,h) = strtod(buffer, nullptr);
            } else {
                goto cleanup;
            }
        }
    for (size_t h = 0; h < Q; h++)
        for (size_t j = 0; j < OUT_N_3; j++) {
            if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
                w2(h,j) = strtod(buffer, nullptr);
            } else {
                goto cleanup;
            }
        }
    for (size_t h = 0; h < Q; h++) {
        if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
            r2[h] = strtod(buffer, nullptr);
        } else {
            goto cleanup;
        }
    }
    for (size_t j = 0; j < OUT_N_3; j++) {
        if (fgets(buffer, BUFFER_SIZE, in) != nullptr) {
            o2[j] = strtod(buffer, nullptr);
        } else {
            goto cleanup;
        }
    }

    fclose(in);
    return true;

    cleanup:
    fprintf(stderr, "[BPNN] BPNN PARAM FILE NOT FIT!\n");
    fclose(in);
    return false;
}

double Model::Fit(const char *p, int row, int col, char* name,bool type)
{
    name[0] = '\0';
    InputSample(p,row,col,name);
    double max;

    Conv();
    Pool();

    MatrixXf cur = pool2;
    cur.resize(D,1);
    if(type)
    {
        for (size_t h = 0; h < Q; h++) {
            double alpha_h = v1.col(h).dot(cur.col(0));
            b(h,0) = ACTIVE(alpha_h - r1(h,0));
        }

        /* Compute out[j] */
        for (size_t j = 0; j < OUT_N_1; j++) {
            double beta_j = 0;
            beta_j = w1.col(j).dot(b.col(0));
//        for (size_t h = 0; h < Q; h++)
//            beta_j += w[h][j] * b[h];
            out1(j,0) = exp(beta_j - o1(j,0));
        }
        /*softmax activation*/
        float SUM = out1.sum();
        for (size_t j = 0; j < OUT_N_1; j++)
        {
            out1(j,0) = out1(j,0)/SUM;
        }
        MatrixXd::Index maxRow, maxCol;
        max = out1.maxCoeff(&maxRow,&maxCol);
        sprintf(name, "%s", hanNames[maxRow]);
    }else
    {
        for (size_t h = 0; h < Q; h++) {
            double alpha_h = v2.col(h).dot(cur.col(0));
            b(h,0) = ACTIVE(alpha_h - r2(h,0));
        }

        /* Compute out[j] */
        for (size_t j = 0; j < OUT_N_3; j++) {
            double beta_j = 0;
            beta_j = w2.col(j).dot(b.col(0));
//        for (size_t h = 0; h < Q; h++)
//            beta_j += w[h][j] * b[h];
            out2(j,0) = exp(beta_j - o2(j,0));
        }
        /*softmax activation*/
        float SUM = out2.sum();
        for (size_t j = 0; j < OUT_N_3; j++)
        {
            out2(j,0) = out2(j,0)/SUM;
        }
        MatrixXd::Index maxRow, maxCol;
        max = out2.maxCoeff(&maxRow,&maxCol);
        sprintf(name, "%s", letterNames[maxRow]);
    }
    return (1 - max);
}
#endif //FPGA_NET_H
