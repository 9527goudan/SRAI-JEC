#安装Paho mqtt C
##1.首先，更新apt软件包索引以确保获取最新的软件包信息：
---bash
sudo apt update
---

##2.安装构建Paho MQTT C库所需的依赖项，包括构建工具、编译器和其他必要的软件包：
---bash
sudo apt install build-essential gcc make cmake git
---

##3.克隆仓库Paho Mqtt C 到本地：
---bash
git clone https://github.com/eclipse/paho.mqtt.c.git
---

##4.进入仓库目录，并创建构建目录build
---bash
cd paho.mqtt.c
mkdir build
cd build
---

##5.运行CMake生成Makefile以进行后续的构建过程：
---bash
cmake   ..
---

##6.Make构建Paho Mqtt C 库
---bash
sudo make
---

##7.将编译好的库安装到系统目录中：
---bash
sudo make install
---


#以上安装教程来自于 <https://zhuanlan.zhihu.com/p/652687730> 的分享

#Paho Mqtt C官方api文档: <https://eclipse.github.io/paho.mqtt.c/MQTTClient/html/>