#include <Arduino.h>
#include "base64.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "cJSON.h"
#include <ArduinoJson.h>
#include "max9814.h"
#include "max98357.h"
#include "baiduApi.h"
#include "dp.h"

#define key 18      

String wifi_ssid="zandaer";//WiFi名称
String wifi_pass="123456788";//WiFi密码
void setup() {

    Serial.begin(115200);
    pinMode(key,INPUT_PULLUP);
    
    uint8_t count=0;
    WiFi.disconnect(true);
    WiFi.begin(wifi_ssid,wifi_pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        count++;
        if(count>=75){
            Serial.printf("\r\n-- wifi connect fail! --");
            break;
        }
        vTaskDelay(200);
    }
    Serial.printf("\r\n-- wifi connect success! --\r\n");
    max9814_Init();
    max98357_Init();
    gain_token();
}



void loop() {
    start_Recording1();
    
    //if(digitalRead(key)==0) //按键按下
    //{
        //start_Recording();
        //sendVoiceToBaidu((uint8_t *)adc_data,32000);
        //while (!digitalRead(key));
        //Serial.printf("结束录音\r\n");
    //}
}
