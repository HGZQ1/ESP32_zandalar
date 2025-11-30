#ifndef __BAIDU_H
#define __BAIDU_H
#include <Arduino.h>
extern String client_id;
extern String client_secret;
extern String access_token;

void gain_token(void);// 获取token
void speechSynthesis(String inputText);// 语音合成
void sendVoiceToBaidu(uint8_t* adc_data, size_t data_len);// 发送音频数据给百度
//String parse_baidu_result(String result); // 解析百度语音识别结果
#endif