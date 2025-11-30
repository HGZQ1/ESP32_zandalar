#include "max98357.h"

// 音量控制变量
static float current_volume = 0.3f;  // 默认音量1.0（最大）

// 设置音量函数
void max98357_set_volume(float volume) {
    // 限制音量范围在0.0到1.0之间
    if(volume > 0.5f) volume = 0.5f;
    if(volume < 0.0f) volume = 0.0f;
    current_volume = volume;
}

// 获取当前音量
float max98357_get_volume(void) {
    return current_volume;
}

// max98357初始化配置
void max98357_Init(void) {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
}

// 带音量控制的音频写入函数
void max98357_write_audio(int16_t* samples, size_t sample_count) {
    // 创建临时缓冲区
    int16_t* buffer = (int16_t*)malloc(sample_count * sizeof(int16_t));
    if(buffer == NULL) return;
    
    // 应用音量控制
    for(size_t i = 0; i < sample_count; i++) {
        buffer[i] = (int16_t)(samples[i] * current_volume);
    }
    
    // 写入I2S
    size_t bytes_written = 0;
    i2s_write(I2S_NUM, buffer, sample_count * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    
    // 释放临时缓冲区
    free(buffer);
}
