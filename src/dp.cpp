#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <baiduApi.h>
#include <dp.h>

const char *apiUrl = "https://api.deepseek.com/v1/chat/completions";//api接口
const char *apiKey = "";//api密钥



String userInput = "";
void parseResponse(const String &response);
void sendRequest(String userInput);
void setup();
void loop();
#line 17 "E:/machine learning/deepseek1/1/src/main.ino"
/**
 * 解析响应字符串，提取并解析JSON数据
 * @param response 包含16进制长度前缀和JSON数据的响应字符串
 */
void start_Recording1(){
    if (Serial.available())
    {   
        //userInput = parseBaiduResponse("");// 获取百度语音识别结果
        userInput = Serial.readStringUntil('\n');//读取用户输入，直到换行符,串口直接输入提示词
        userInput.trim();
        Serial.println("用户输入: " + userInput);


        if (WiFi.status() == WL_CONNECTED)
        {
            sendRequest(userInput);
            // 语音合成
        }
        else
        {
            Serial.println("Wi-Fi 未连接");
        }


        userInput = "";
    
    }



}
void parseResponse(const String &response)
{

    // 查找第一个回车换行符的位置
    int newlineIndex = response.indexOf("\r\n");

    // 如果找到回车换行符
    if (newlineIndex != -1)
    {

        // 提取16进制长度字符串
        String hexLengthStr = response.substring(0, newlineIndex);
        // 将16进制字符串转换为整型长度值
        int jsonLength = strtol(hexLengthStr.c_str(), nullptr, 16);


        // 提取JSON数据部分（跳过回车换行符）
        String jsonData = response.substring(newlineIndex + 2);

        // 检查JSON数据长度是否足够
        if (jsonData.length() >= jsonLength)
        {

            // 提取指定长度的JSON字符串
            String jsonResponse = jsonData.substring(0, jsonLength);


            // 创建JSON文档对象
            StaticJsonDocument<2048> doc;
            // 反序列化JSON数据
            DeserializationError error = deserializeJson(doc, jsonResponse);
            // 如果解析失败
            if (error)
            {
                Serial.print("JSON 解析失败: ");
                Serial.println(error.c_str());
            }
            else
            {
                Serial.println("JSON 解析成功:");

                // 提取JSON中的content字段
                const char *content = doc["choices"][0]["message"]["content"];


                // 打印content内容
                Serial.print("Content: ");
                Serial.println(content);
                speechSynthesis(content);// 语音合成
            }
        }
        else
        {
            Serial.println("JSON 数据长度不足");
        }
    }
    else
    {
        Serial.println("未找到16进制长度");
    }
}


void sendRequest(String userInput)
{
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(apiKey));
    http.setTimeout(10000);


    String jsonData = "{\"model\":\"deepseek-chat\",\"messages\":[{\"role\":\"user\",\"content\":\"" + userInput + "\"}]}";
    Serial.println("请求体: " + jsonData);

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0)
    {

        Serial.println("API 响应状态码: " + String(httpResponseCode));


        for (int i = 0; i < http.headers(); i++)
        {
            Serial.println(http.headerName(i) + ": " + http.header(i));
        }


        WiFiClient *stream = http.getStreamPtr();
        if (stream)
        {
            String response;
            unsigned long startTime = millis();
            unsigned long timeout = 100000;

            unsigned long noDataStartTime = 0;
            int noDataCount = 0;
            const int noDataThreshold = 10;
            const unsigned long noDataMaxTime = 1000;

            bool hasReceivedData = false;

            while (stream->connected())
            {
                if (millis() - startTime > timeout)
                {
                    Serial.println("整体读取超时");
                    break;
                }

                if (stream->available())
                {
                    char c = stream->read();
                    response += c;
                    Serial.print(c);

                    hasReceivedData = true;
                    startTime = millis();
                    noDataCount = 0;
                    noDataStartTime = 0;
                }
                else if (hasReceivedData)
                {
                    if (noDataStartTime == 0)
                    {
                        noDataStartTime = millis();
                    }

                    noDataCount++;


                    if (noDataCount >= noDataThreshold && (millis() - noDataStartTime) >= noDataMaxTime)
                    {
                        Serial.println("超过1秒无数据，退出循环");
                        break;
                    }

                    delay(10);
                }
            }


            if (!response.isEmpty())
            {
                parseResponse(response);
            }
            else
            {
                Serial.println("API 响应体为空");
            }
        }
        else
        {
            Serial.println("无法获取响应流");
        }
    }
    else
    {
        Serial.println("请求失败，错误代码: " + String(httpResponseCode));
        Serial.println("错误信息: " + http.errorToString(httpResponseCode));
    }

    http.end();
}