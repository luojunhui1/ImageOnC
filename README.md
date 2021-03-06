# ImageOnC
## 1.介绍
本仓库为实现在FPGA上的车牌识别而创建，但本仓库只保有C/C++部分的代码，并未保存使用HLS工具后的代码。要特别说明的是，本项目中的代码均用C实现，其中出现的C++主要为便于OpenCV进行图像显示或者Eigen库加速矩阵运算，但均可删除或改成C中的数组而不影响其正常功能。
## 2.文件组成
```
.
├── build
├── cmake-build-debug
├── CMakeLists.txt
├── database
├── Fit.cpp
├── Han
├── include
├── Letters
├── main.cpp
├── paramLetters.txt
├── param.txt
├── README.md
└── Train.cpp
```

其中build和cmake-build-debug文件均为编译执行过程产生的文件；CMakeLists.txt用于指导编译方式；database为车牌图片文件夹；Fit.cpp原作测试网络准确性，但其内容在测试后被整合到main.cpp中，故该文件无实际意义；Han文件夹保存了用于训练汉字识别的图像；Letters中则保存了用于训练字母和数字识别的代码；main.cpp为执行的识别车牌的主函数；Train.cpp用于训练神经网络；param.txt及paramLetters.txt则保存了网络参数；include文件中保存了一写自定义的功能函数，其文件树如下：
```
.
├── Config.h
├── Eigen
├── FileProcess.h
├── ModelTrans.h
├── Net.h
├── Process.h
├── SaveLoad.h
└── unsupported
```
**Config.h**: 用于约定网络参数和一些全局变量，便于项目代码组织
**Eigen**: Eigen库代码
**unsupported**: Eigen库代码,原为使用Tensor类表示高维矩阵，但Tensor使用不便，实际未使用
**FileProcess**: 用于系统文件操作，主要是查询文件夹下的所有文件并遍历
**ModelTrans**: 用于从图像的数据矩阵中读取BGR图像并将其分割、保存
**Net.h**: 神经网络的定义、训练及使用部分
**SaveLoad**: 用于从图像路径读取bmp图像并分通道保存图像数据部分

## 3. 实际效果
数据集比较简单，能做到100%。




