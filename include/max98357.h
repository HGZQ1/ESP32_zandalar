#ifndef __MAX98357_H
#define __MAX98357_H
#include <Arduino.h>
#include <driver/i2s.h>
#define I2S_NUM           I2S_NUM_0
#define I2S_SAMPLE_RATE   13190  // 跟百度返回采样率匹配
#define I2S_BCK_IO        15
#define I2S_WS_IO         16
#define I2S_DO_IO         7
void max98357_Init(void);

#endif