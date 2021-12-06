#include "port.h"

/**
  * @brief LCD1602延时函数，12MHz调用可延时1ms
  */
void LCD_Delay() {
    unsigned char i, j;

    i = 2;
    j = 239;
    do {
        while (--j);
    } while (--i);
}

/**
  * @brief LCD1602写命令
  * @param command 要写入的命令
  */
void LCD_WriteCommand(unsigned char command) {
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DataPort = command;
    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

/**
  * @brief LCD1602写数据
  * @param data_ 要写入的数据
  */
void LCD_WriteData(unsigned char data_) {
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DataPort = data_;
    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

/**
  * @brief LCD1602设置光标位置
  * @param line 行位置，范围：1~2
  * @param column 列位置，范围：1~16
  */
void LCD_SetCursor(unsigned char line, unsigned char column) {
    if (line == 1) {
        LCD_WriteCommand(0x80 | (column - 1));
    } else if (line == 2) {
        LCD_WriteCommand(0x80 | (column - 1 + 0x40));
    }
}

/**
  * @brief LCD1602初始化函数
  */
void LCD_Init() {
    LCD_WriteCommand(0x38);  //八位数据接口，两行显示，5*7点阵
    LCD_WriteCommand(0x0c);  //显示开，光标关，闪烁关
    LCD_WriteCommand(0x06);  //数据读写操作后，光标自动加一，画面不动
    LCD_WriteCommand(0x01);  //光标复位，清屏
}

/**
  * @brief 在LCD1602指定位置上显示一个字符
  * @param line 行位置，范围：1~2
  * @param column 列位置，范围：1~16
  * @param char_ 要显示的字符
  */
void LCD_ShowChar(unsigned char line, unsigned char column, char char_) {
    LCD_SetCursor(line, column);
    LCD_WriteData(char_);
}

/**
  * @brief 在LCD1602指定位置开始显示所给字符串
  * @param line 起始行位置，范围：1~2
  * @param column 起始列位置，范围：1~16
  * @param string 要显示的字符串
  */
void LCD_ShowString(unsigned char line, unsigned char column, unsigned char *str) {
    unsigned char i;
    LCD_SetCursor(line, column);
    for (i = 0; str[i] != '\0'; i++) {
        LCD_WriteData(str[i]);
    }
}

/**
  * @brief 计算x的y次方
  * @param x x
  * @param y y
  * @retval x的y次方
  */
int LCD_Pow(int x, int y) {
    unsigned char i;
    int result = 1;
    for (i = 0; i < y; i++) {
        result *= x;
    }
    return result;
}

/**
  * @brief 在LCD1602指定位置开始显示所给数字
  * @param line 起始行位置，范围：1~2
  * @param column 起始列位置，范围：1~16
  * @param number 要显示的数字，范围：0~65535
  * @param length 要显示数字的长度，范围：1~5
  */
void LCD_ShowNum(unsigned char line, unsigned char column, unsigned int number, unsigned char length) {
    unsigned char i;
    LCD_SetCursor(line, column);
    for (i = length; i > 0; i--) {
        LCD_WriteData(number / LCD_Pow(10, i - 1) % 10 + '0');
    }
}