/* ESP-IDF with Arduino as Component
   A template by Oliver Copleston (SquidSoup)
   
   ----README----
   - Arduino and otherlibraries in the Main/Libraries folder
   - Additional source files in the src folder
   - Additional header files in the include folder
*/
#include "main.h"
#include "sdkconfig.h"
#include <Arduino.h>

#include <esp_log.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* strtol */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "i2s_stream.h"
#include "wav_decoder.h"
#include "fatfs_stream.h"
#include "downmix.h"
#include "raw_stream.h"
#include "board.h"
#include "periph_sdcard.h"
#include "periph_button.h"


static const char *TAG = "DOWNMIX_PIPELINE";

#define SAMPLERATE 48000
#define DEFAULT_CHANNEL 1
#define TRANSMITTIME 10
#define MUSIC_GAIN_DB 0
#define PLAY_STATUS ESP_DOWNMIX_OUTPUT_TYPE_ONE_CHANNEL
#define NUMBER_SOURCE_FILE 2

extern "C" void app_main() {    
      
   // Serial.begin(115200);
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_INFO);
   
    initArduino();

    
   
}