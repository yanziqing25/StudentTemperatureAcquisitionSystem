#ifndef __LCD1602_H__
#define __LCD1602_H__

void LCD_Init();
void LCD_ShowChar(unsigned char line, unsigned char column, char char_);
void LCD_ShowString(unsigned char line, unsigned char column, char *str);
void LCD_ShowNum(unsigned char line, unsigned char column, unsigned int number, unsigned char length);

#endif