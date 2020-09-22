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

static const char *TAG = "STARTUP";

extern "C" void app_main() {    
      
   // Serial.begin(115200);
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_INFO);
   
    initArduino();

    
   
}