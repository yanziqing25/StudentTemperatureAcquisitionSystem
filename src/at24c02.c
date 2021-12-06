#include "port.h"

/**
  * @brief 延时10us
  */
void Delay10us() {
    unsigned char a, b;
    for (b = 1; b > 0; b--)
        for (a = 2; a > 0; a--);
}

/**
  * @brief 写数据延迟函数
  */
void write_delay() {
    unsigned char i;
    for (i = 110; i > 0; i--);
}

/*******************************************************************************
* 函数名         : I2cStart()
* 函数功能		 : 起始信号：在SCL时钟信号在高电平期间SDA信号产生一个下降沿
* 输入           : 无
* 输出         	 : 无
* 备注           : 起始之后SDA和SCL都为0
*******************************************************************************/
void I2cStart() {
    EEPROM_SDA = 1;
    Delay10us();
    EEPROM_SCL = 1;
    Delay10us();  //建立时间是SDA保持时间>4.7us
    EEPROM_SDA = 0;
    Delay10us();  //保持时间是>4us
    EEPROM_SCL = 0;
    Delay10us();
}

/*******************************************************************************
* 函数名         : I2cStop()
* 函数功能		 : 终止信号：在SCL时钟信号高电平期间SDA信号产生一个上升沿
* 输入           : 无
* 输出         	 : 无
* 备注           : 结束之后保持SDA和SCL都为1；表示总线空闲
*******************************************************************************/
void I2cStop() {
    EEPROM_SDA = 0;
    Delay10us();
    EEPROM_SCL = 1;
    Delay10us();  //建立时间大于4.7us
    EEPROM_SDA = 1;
    Delay10us();
}

/*******************************************************************************
* 函数名         : I2cSendByte(unsigned char dat)
* 函数功能		 : 通过I2C发送一个字节。在SCL时钟信号高电平期间，保持发送信号SDA保持稳定
* 输入           : num
* 输出         	 : 0或1。发送成功返回1，发送失败返回0
* 备注           : 发送完一个字节SCL=0,SDA=1
*******************************************************************************/
unsigned char I2cSendByte(unsigned char dat) {
    unsigned char a = 0, b = 0;  //最大255，一个机器周期为1us，最大延时255us。
    for (a = 0; a < 8; a++)      //要发送8位，从最高位开始
    {
        EEPROM_SDA = dat >> 7;  //起始信号之后SCL=0，所以可以直接改变SDA信号
        dat = dat << 1;
        Delay10us();
        EEPROM_SCL = 1;
        Delay10us();  //建立时间>4.7us
        EEPROM_SCL = 0;
        Delay10us();  //时间大于4us
    }
    EEPROM_SDA = 1;
    Delay10us();
    EEPROM_SCL = 1;
    while (EEPROM_SDA)  //等待应答，也就是等待从设备把SDA拉低
    {
        b++;
        if (b > 200)  //如果超过2000us没有应答发送失败，或者为非应答，表示接收结束
        {
            EEPROM_SCL = 0;
            Delay10us();
            return 0;
        }
    }
    EEPROM_SCL = 0;
    Delay10us();
    return 1;
}

/*******************************************************************************
* 函数名         : I2cReadByte()
* 函数功能		   : 使用I2c读取一个字节
* 输入           : 无
* 输出         	 : dat
* 备注           : 接收完一个字节SCL=0,SDA=1.
*******************************************************************************/
unsigned char I2cReadByte() {
    unsigned char a = 0, dat = 0;
    EEPROM_SDA = 1;  //起始和发送一个字节之后SCL都是0
    Delay10us();
    for (a = 0; a < 8; a++)  //接收8个字节
    {
        EEPROM_SCL = 1;
        Delay10us();
        dat <<= 1;
        dat |= EEPROM_SDA;
        Delay10us();
        EEPROM_SCL = 0;
        Delay10us();
    }
    return dat;
}

/**
  * @brief 往24c02的一个地址写入一个数据
  * @param addr 地址，范围:0~255
  * @param dat 需要写入的一个数据，范围:0~255
  */
void at24c02_write(unsigned char addr, unsigned char dat) {
    I2cStart();
    I2cSendByte(0xa0);  //发送写器件地址
    I2cSendByte(addr);  //发送要写入内存地址
    I2cSendByte(dat);   //发送数据
    I2cStop();
    write_delay();// 第二次写数据时需要延时!!
}

/**
  * @brief 读取24c02的一个地址的一个数据
  * @param addr 地址，范围:0~255
  * @retval 读取到的一个数据
  */
unsigned char at24c02_read(unsigned char addr) {
    unsigned char num;
    I2cStart();
    I2cSendByte(0xa0);  //发送写器件地址
    I2cSendByte(addr);  //发送要读取的地址
    I2cStart();
    I2cSendByte(0xa1);    //发送读器件地址
    num = I2cReadByte();  //读取数据
    I2cStop();
    return num;
}

/**
  * @brief 清空at24c02存储空间(把所有地址都设为0)
  * @retval 1。清空完毕时返回1
  */
bit at24c02_clean() {
    unsigned char i = 0;
    while (1) {
        at24c02_write(i, 0x00);
        write_delay();
        if (i == 255) break;
        i++;
    }
    return 1;
}

/**
  * @brief 寻找at24c02存储空间中可写的位置
  * @param sign 地址尾标号字符
  * @retval 位置标号
  */
unsigned char at24c02_find_last_write(unsigned char sign) {
    unsigned char i = 0, flag = 0;
    while (1) {
        if (at24c02_read(i) == sign) {
            flag = i + 1;
        }
        if (i == 255) break;
        i++;
    }
    return flag;
}

/**
  * @brief 往24c02的地址中写入学生学号和温度
  * @param student_id 十位学生学号
  * @param temp 四位十进制温度
  */
void at24c02_write_stu_tem(unsigned char *student_id, unsigned int temp) {
    unsigned char i, flag;

    flag = at24c02_find_last_write(0x0a);
    for (i = 0; i < 10; i++) {
        if (student_id[i] < 0x30 || student_id[i] > 0x39) {
            at24c02_write(flag++, student_id[i] + 0x30);
        } else {
            at24c02_write(flag++, student_id[i]);
        }
    }
    at24c02_write(flag++, 0x03);

    at24c02_write(flag++, temp / 1000 + 0x30);
    at24c02_write(flag++, temp / 100 % 10 + 0x30);
    at24c02_write(flag++, temp / 10 % 10 + 0x30);
    at24c02_write(flag++, temp % 10 + 0x30);
    at24c02_write(flag, 0x0a);
}

/**
  * @brief 读24c02的地址中有效部分
  * @param sign 地址尾标号字符
  * @param dat 返回的数据字符串
  * @retval 数据长度
  */
unsigned char at24c02_get_all_data(unsigned char sign, unsigned char *dat) {
    unsigned char i, flag;
    flag = at24c02_find_last_write(sign);
    for (i = 0; i < flag; i++, *dat++) {
        *dat = at24c02_read(i);
    }
    return flag;
}