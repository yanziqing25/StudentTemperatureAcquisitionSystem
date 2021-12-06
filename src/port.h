#ifndef __PORT_H__
#define __PORT_H__

#include <reg52.h>

/* 端口定义 */
//RC522
sbit MF522_NSS = P1 ^ 7;  // RC500片选 SDA
sbit MF522_SCK = P1 ^ 6;  // SCK
sbit MF522_SI = P1 ^ 5;   // MOSI
sbit MF522_SO = P1 ^ 4;   // MISO
sbit MF522_RST = P1 ^ 3;  // RST

//LCD1602
sbit LCD_RS = P2 ^ 6;
sbit LCD_RW = P2 ^ 5;
sbit LCD_EN = P2 ^ 7;
#define LCD_DataPort P0

// AT24C02
sbit EEPROM_SCL = P2 ^ 1;
sbit EEPROM_SDA = P2 ^ 0;

//DS18B20
sbit DS_PORT = P3 ^ 7;

//BEEP
sbit BEEP = P2 ^ 5;

//KEYBOARD
sbit K1 = P3 ^ 1;
sbit K2 = P3 ^ 0;
sbit K3 = P3 ^ 2;
sbit K4 = P3 ^ 3;

#endif