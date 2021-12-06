#include "port.h"

/**
  * @brief 延时函数
  * @param i 延迟系数
  */
void Delay1ms(unsigned int i) {
    unsigned int j;
    for (i; i > 0; i--)
        for (j = 110; j > 0; j--);
}

/**
  * @brief 初始化
  * @retval 初始化成功返回1，失败返回0
  */
unsigned char Ds18b20Init() {
    unsigned int i;
    DS_PORT = 0;  //将总线拉低480us~960us
    i = 70;
    while (i--);         //延时642us
    DS_PORT = 1;  //然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
    i = 0;
    while (DS_PORT)  //等待DS18B20拉低总线
    {
        i++;
        if (i > 5000)  //等待>5MS
            return 0;  //初始化失败
    }
    return 1;  //初始化成功
}

/**
  * @brief 向18B20写入一个字节
  * @param dat 一个字节
  */
void Ds18b20WriteByte(unsigned char dat) {
    unsigned int i, j;
    for (j = 0; j < 8; j++) {
        DS_PORT = 0;  //每写入一位数据之前先把总线拉低1us
        i++;
        DS_PORT = dat & 0x01;  //然后写入一个数据，从最低位开始
        i = 6;
        while (i--)
            ;         //延时68us，持续时间最少60us
        DS_PORT = 1;  //然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
        dat >>= 1;
    }
}

/**
  * @brief 读取一个字节
  * @retval 一个字节
  */
unsigned char Ds18b20ReadByte() {
    unsigned char byte, bi;
    unsigned int i, j;
    for (j = 8; j > 0; j--) {
        DS_PORT = 0;  //先将总线拉低1us
        i++;
        DS_PORT = 1;  //然后释放总线
        i++;
        i++;           //延时6us等待数据稳定
        bi = DS_PORT;  //读取数据，从最低位开始读取
        /*将byte左移一位，然后与上右移7位后的bi，注意移动之后移掉那位补0。*/
        byte = (byte >> 1) | (bi << 7);
        i = 4;  //读取完之后等待48us再接着读取下一个数
        while (i--);
    }
    return byte;
}

/**
  * @brief 让18b20开始转换温度
  */
void Ds18b20ChangTemp() {
    Ds18b20Init();
    Delay1ms(1);
    Ds18b20WriteByte(0xcc);  //跳过ROM操作命令
    Ds18b20WriteByte(0x44);  //温度转换命令
    //	Delay1ms(100);	//等待转换成功，而如果你是一直刷着的话，就不用这个延时了
}

/**
  * @brief 发送读取温度命令
  */
void Ds18b20ReadTempCom() {
    Ds18b20Init();
    Delay1ms(1);
    Ds18b20WriteByte(0xcc);  //跳过ROM操作命令
    Ds18b20WriteByte(0xbe);  //发送读取温度命令
}

/**
  * @brief 读取温度
  */
int Ds18b20ReadTemp() {
    int temp = 0;
    float tp;
    unsigned char tmh, tml;

    Ds18b20ChangTemp();       //先写入转换命令
    Ds18b20ReadTempCom();     //然后等待转换完后发送读取温度命令
    tml = Ds18b20ReadByte();  //读取温度值共16位，先读低字节
    tmh = Ds18b20ReadByte();  //再读高字节
    temp = tmh;
    temp <<= 8;
    temp |= tml;

    if (temp > 0) {
        tp = temp;
        temp = tp * 0.0625 * 100 + 0.5;
        temp += 750;//补偿
    }
    return temp;
}