#ifndef __MAX9814_H
#define __MAX9814_H
#include <Arduino.h>
#define ADC 4      
#define DATA_LEN 16000
extern uint16_t adc_data[DATA_LEN];    //16000个数据，8K采样率，即2秒，录音时间为2秒，想要实现更长时间的语音识别，就要改这个数组大小
                                //和下面data_json数组的大小，改大一些。
extern uint8_t adc_start_flag;       //开始标志
extern uint8_t adc_complete_flag;    //完成标志
void max9814_Init(void);
void start_Recording(void);
#endif