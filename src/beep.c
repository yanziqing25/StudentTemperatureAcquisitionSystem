#include "port.h"

/**
  * @brief 蜂鸣器发声
  * @param sound 音色
  */
void beep(unsigned int sound) {
    BEEP = ~BEEP;
    while (sound--);
}