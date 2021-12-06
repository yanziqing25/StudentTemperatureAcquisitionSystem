#ifndef __AT24C02_H__
#define __AT24C02_H__

unsigned char at24c02_read(unsigned char addr);
bit at24c02_clean();
void at24c02_write_stu_tem(unsigned char *student_id, unsigned int temp);
unsigned char at24c02_get_all_data(unsigned char sign, unsigned char *dat);

#endif