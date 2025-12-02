#include "baiduApi.h"

#include "max98357.h"
#include "HTTPClient.h"
#include <ArduinoJson.h>
#include "base64.h"
//百度api密钥
String client_id = "";
String client_secret = "";
String access_token="";

// 获取token并解析JSON提取access_token
void gain_token(void) {
    int httpCode;

    HTTPClient http_client;
    // 拼接URL
    String token_url = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials";
    token_url += "&client_id=" + client_id;
    token_url += "&client_secret=" + client_secret;

    http_client.begin(token_url);
    http_client.addHeader("User-Agent", "ESP32");

    httpCode = http_client.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String resp = http_client.getString();
            Serial.println("Token response:");
            Serial.println(resp);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, resp);
            if (!error) {
                if (doc["access_token"].is<const char*>()) {
                    access_token = doc["access_token"].as<String>();
                    Serial.print("Parsed access_token: ");
                    Serial.println(access_token);
                } else {
                    Serial.println("No access_token found in JSON");
                }
            } else {
                Serial.print("JSON parse error: ");
                Serial.println(error.c_str());
            }
        } else {
            Serial.printf("HTTP response code: %d\n", httpCode);
        }
    }
    else {
        Serial.printf("[HTTP] GET failed, error: %s\n", http_client.errorToString(httpCode).c_str());
    }
    http_client.end();
}

// 从网络请求百度TTS并播放
void speechSynthesis(String inputText) {
    HTTPClient http;
    String payload = "tex=" + inputText + 
                    "&lan=zh&cuid=hugokkl&ctp=1&spd=5&pit=5&vol=5&per=4196&aue=4&tok=" + access_token;

    http.begin("https://tsn.baidu.com/text2audio");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(payload);
    if (httpCode == HTTP_CODE_OK) {
        WiFiClient * stream = http.getStreamPtr();

        uint8_t buffer[512];
        size_t bytesRead = 0;

        while (http.connected() && (bytesRead = stream->available()) > 0) {
            size_t len = stream->readBytes(buffer, sizeof(buffer));
            if (len > 0) {
                size_t bytesWritten = 0;
                i2s_write(I2S_NUM, buffer, len, &bytesWritten, portMAX_DELAY);
            }
        }
    } else {
        Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

String parseBaiduResponse(const String& payload) {
    JsonDocument  doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print("JSON解析失败: ");
        Serial.println(error.c_str());
        return;
    }

    if (doc["result"].is<JsonArray>()) {
        const char* result = doc["result"][0];  // result 是数组，取第一个元素
        Serial.print("识别结果: ");
        Serial.println(result);
        
    } else {
        Serial.println("未找到 result 字段");
    }
}

void sendVoiceToBaidu(uint8_t* adc_data, size_t data_len) {
    
    String data_json = "{";
    data_json += "\"format\":\"pcm\",";
    data_json += "\"rate\":8000,";
    data_json += "\"dev_pid\":1537,";
    data_json += "\"channel\":1,";
    data_json += "\"cuid\":\"123456\",";
    data_json += "\"token\":\"" + String(access_token) + "\",";
    data_json += "\"len\":" + String(data_len) + ",";
    data_json += "\"speech\":\"";
    data_json += base64::encode(adc_data, data_len);
    data_json += "\"}";
    
    // 使用 HTTPS 请求
    WiFiClientSecure client;
    client.setInsecure(); // 跳过证书验证

    HTTPClient https;
    https.begin(client, "https://vop.baidu.com/server_api");

    https.addHeader("Content-Type", "application/json");

    int httpCode = https.POST(data_json);

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = https.getString();
            parseBaiduResponse(payload); // 显式转换为String类型 // 你原来的处理函数
        } else {
            Serial.printf("服务器响应码不是200: %d\n", httpCode);
        }
    } else {
        Serial.printf("[HTTPS] POST 失败, 错误: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
}
