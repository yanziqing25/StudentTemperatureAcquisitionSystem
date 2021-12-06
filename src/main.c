#include "port.h"
#include "at24c02.h"
#include "beep.h"
#include "ds18b20.h"
#include "lcd1602.h"
#include "rc522.h"

#define BEEP_TEMPERATURE 3720

code unsigned char const default_key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//unsigned char xuehao[16] = {0x02, 0x00, 0x01, 0x09, 0x01, 0x03, 0x02, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // 2019132120
//unsigned char xuehao[16] = {0x02, 0x00, 0x01, 0x09, 0x01, 0x03, 0x02, 0x01, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // 2019132114

void delay(unsigned int i) {
    unsigned int j;
    for (; i > 0; i--)
        for (j = 110; j > 0; j--);
}

void init_bluetooth() {
    SCON = 0x50;  //设置为工作方式1,ren=1
    TMOD = 0x20;  //设置定时器T1工作方式2
    PCON = 0x80;  //波特率加倍
    TH1 = 0xfd;   //波特率9600 * 2 = 19200
    TL1 = 0xfd;
    TR1 = 1;  //打开定时器T1

    EA = 1; // 打开中断总开关
    ES = 1; // 允许串行口中断
}

void main() {
    unsigned char g_ucTempbuf[16];
    unsigned char i, status, temp_z, temp_x;
    unsigned char student_id[10];
    unsigned int temp = 0, beep_time;
    bit get_card_id_succ = 0;

    init_bluetooth();
    PcdReset();
    PcdAntennaOff();
    PcdAntennaOn();

    LCD_Init();
    LCD_ShowString(1, 0, "Welcome to use!");
    delay(2000);
    LCD_Init();
    LCD_ShowString(1, 0, "Put the card...");
    LCD_ShowString(2, 0, "Or k4 to clean");

    while (1) {
        // K1被按下
        if (!K1 && get_card_id_succ) {
            while (!K1);  // 消抖
            LCD_Init();
            LCD_ShowString(1, 0, "Temperature:");

            // 读取温度
            while (1) {
                // K1被按下
                if (!K1 && temp) {
                    while (!K1);  // 消抖
                    LCD_Init();
                    LCD_ShowString(1, 0, "Your temp:");
                    LCD_ShowNum(1, 12, temp_z, 2);
                    LCD_ShowChar(1, 14, '.');
                    LCD_ShowNum(1, 15, temp_x, 2);
                    LCD_ShowString(2, 0, "Saving...");
                    at24c02_write_stu_tem(student_id, temp);
                    LCD_Init();
                    LCD_ShowString(1, 0, "Save successful!");
                    delay(1000);
                    break;// 重新刷卡测下一个人的体温
                }

                temp = Ds18b20ReadTemp();
                beep_time = 500;
                if (temp > BEEP_TEMPERATURE) {
                    while (beep_time--) {
                        beep(50);
                    }
                }
                temp_z = temp / 100;
                temp_x = temp % 100;
                LCD_ShowNum(2, 1, temp_z, 2);
                LCD_ShowChar(2, 3, '.');
                LCD_ShowNum(2, 4, temp_x, 2);
            }
        }        

        // K4被按下
        if (!K4) {
            while (!K4);  // 消抖
            LCD_Init();
            LCD_ShowString(1, 0, "Cleaning...");
            at24c02_clean();
            LCD_ShowString(1, 0, "Clean succeed!");
            delay(1000);
        }

        status = PcdRequest(PICC_REQALL, g_ucTempbuf);  //寻卡
        if (status != MI_OK) {
            continue;
        }

        PcdAnticoll(g_ucTempbuf);                                   //防冲撞
        PcdSelect(g_ucTempbuf);                                     //选定卡片
        PcdAuthState(PICC_AUTHENT1A, 1, default_key, g_ucTempbuf);  //验证扇区0 块1的密码A
        //PcdWrite(1, xuehao);                                        //写块1
        status = PcdRead(1, g_ucTempbuf);  //读块1

        // 读卡成功
        if (status == MI_OK) {
            get_card_id_succ = 1;  // 调试
            //保存卡号
            for (i = 0; i < 10; i++) {
                student_id[i] = g_ucTempbuf[i];
            }
            LCD_ShowString(1, 0, "s_id: ");
            for (i = 0; i < 10; i++) {
                LCD_ShowNum(1, i + 7, student_id[i], 1);
            }
            LCD_ShowString(2, 0, "Press k1 enter!");
        }
    }
}

void serial_interrupt() interrupt 4 {
    unsigned char i, length, dat[64];
    // 接收
    if (RI) {
        RI = 0;
        if(SBUF == 0x05) {
            // 发送
            length = at24c02_get_all_data(0x0a, dat);
            for (i = 0; i <= length; i++) {
                if (i == length) {
                    SBUF = 0x04;// 发送传输结束字符
                } else {
                    SBUF = dat[i];
                }
                while (!TI);
                TI = 0;
            }
        }
    }
}