#include "max9814.h"
uint16_t adc_data[DATA_LEN];    //16000个数据，8K采样率，即2秒，录音时间为2秒，想要实现更长时间的语音识别，就要改这个数组大小
                                //和下面data_json数组的大小，改大一些。

uint8_t adc_start_flag=0;       //开始标志
uint8_t adc_complete_flag=0;    //完成标志
hw_timer_t * timer = NULL;
uint32_t num=0;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void start_Recording(void){
    Serial.printf("开始录音\r\n\r\n");
    adc_start_flag=1;
    timerStart(timer);

    // time1=micros();
    while(!adc_complete_flag)  //等待采集完成
    {
    ets_delay_us(10);
    }
    // time2=micros()-time1;

    timerStop(timer);
    adc_complete_flag=0;        //清标志
}
void IRAM_ATTR onTimer(){

    portENTER_CRITICAL_ISR(&timerMux);
    if(adc_start_flag==1)
    {
        adc_data[num]=analogRead(ADC);
        num++;
        if(num>=DATA_LEN)
        {
            adc_complete_flag=1;
            adc_start_flag=0;
            num=0;
        }
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}
void max9814_Init(void){
    pinMode(ADC,ANALOG);     
    timer = timerBegin(0, 80, true);    //  80M的时钟 80分频 1M
    timerAlarmWrite(timer, 125, true);  //  1M  计125个数进中断  8K
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmEnable(timer);
    timerStop(timer);   //先暂停
}