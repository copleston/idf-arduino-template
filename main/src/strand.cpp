
// #define SPI_DEVICE_NO_DUMMY

#include "Strand.hpp"
#include <HardwareSerial.h>
#include <SPI.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include "net.h"

static const char *TAG = "STEPPER";

QueueHandle_t xQueue_stepper_command; // Must redefine here

stepper_command_t stepper_commands;

//uninitalised pointers to SPI objects
static const int spiClk = 1000000; // 1 MHz

const int uart_buffer_size = (1024 * 2);
#define RXD2 16
#define TXD2 17
#define EN_PIN           22 // Enable (5)
#define DIR_PIN          16 // Direction (14)
#define STEP_PIN         17 // Step (12)

#define cs_pin              5   // Chip select
#define mosi_pin            23 // Software Master Out Slave In (MOSI) 23
#define miso_pin            19 // Software Master In Slave Out (MISO) 19
#define sck_pin            18 // Software Slave Clock (SCK)
#define DRIVER_ADDRESS 0b00

//#define R_SENSE 0.11f //TMC2208
#define R_SENSE 0.075f//TMC5160

//homiing buttion stuff
#define HOME_PIN         34 // HOME

TMC5160Stepper driver(cs_pin, R_SENSE);

AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);
constexpr uint32_t steps_per_mm = 80;

struct {
    uint8_t blank_time = 16;        // [16, 24, 36, 54]
    uint8_t off_time = 1;           // [1..15]
    uint8_t hysteresis_start = 8;   // [1..8]
    int8_t hysteresis_end = 12;     // [-3..12]
} config;

long currentPosition;
float factor = 11.8; // wheel ratio steps per mm

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};
Button button1 = {HOME_PIN, 0, false};

void IRAM_ATTR isr() {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
}

void command_move(int type, int move, int speed, int accel, int min, int max){
    //xQueueSendToBack(xQueue_stepper_command, (void *) &move, 0);
    stepper_command_t test_action;
    test_action.move = move;
    test_action.type = type;
    test_action.speed = speed;
    test_action.accel = accel;
    test_action.min = min;
    test_action.max = max;

    xQueueSendToBack(xQueue_stepper_command, (void *) &test_action, 0);            
}

void init_strand() {
   pinMode(EN_PIN, OUTPUT);
   pinMode(DIR_PIN, OUTPUT);
   pinMode(STEP_PIN, OUTPUT);
   digitalWrite(EN_PIN, LOW); //deactivate driver (LOW active)

   SPI.begin(); 
   driver.begin();//Begin TMC             
   int err = driver.GSTAT();
   ESP_LOGW(TAG, "Driver stat: %i", err);

   driver.reset();
   driver.toff(4);                  // Enables driver in software
   driver.blank_time(24);
   driver.rms_current(800);        // Set motor RMS current
   driver.microsteps(8);          // Set microsteps to 1/16th
   driver.en_pwm_mode(1);      // Enable extremely quiet stepping
   if (driver.drv_err()) {
       ESP_LOGW(TAG, "Driver ERROR");
   }
   //Driver Tests 
   ESP_LOGI(TAG,"\nTesting connection...");
   uint8_t result = driver.test_connection();

   if (result) {
    ESP_LOGI(TAG,"failed!");
    ESP_LOGI(TAG,"Likely cause: ");

    switch(result) {
        case 0: ESP_LOGW(TAG,"SUCCESS"); break;
        case 1: ESP_LOGW(TAG,"loose connection"); break;
        case 2: ESP_LOGW(TAG,"no power"); break;
        default: ESP_LOGW(TAG,"Default. result: %i", result); break;
    }
    ESP_LOGI(TAG,"Fix the problem and reset board.");
    // We need this delay or messages above don't get fully printed out
    delay(100);
    server_ping("ERROR");//Sends the boot up message to the server

   }
    /* ----THRESHOLD----
     * Changing the 'thr' variable raises or lowers the velocity at which the stepper motor switches between StealthChop and SpreadCycle
     * - Low values results in SpreadCycle being activated at lower velocities
     * - High values results in SpreadCycle being activated at higher velocities
     * - If SpreadCycle is active while too slow, there will be noise
     * - If StealthChop is active while too fast, there will also be noise
     * For the 15:1 stepper, values between 70-120 is optimal 
    */
   uint32_t thr = 100; // 70-120 is optimal
   driver.TPWMTHRS(thr);

   // Stepper Library Setup
   stepper.setMaxSpeed(1600); // 100mm/s @ 80 steps/mm
   stepper.setAcceleration(2000); // 2000mm/s^2
   stepper.setEnablePin(EN_PIN);
   stepper.setPinsInverted(false, false, true);
   stepper.enableOutputs();

    //SENSOR
    pinMode(button1.PIN, INPUT);
    attachInterrupt(button1.PIN, isr, FALLING);
    
}

void stepper_task(void *args) {
    ESP_LOGI(TAG, "Init Stepper Queue");
    // Setup the data structure to store and retrieve stepper commands
    xQueue_stepper_command = xQueueCreate(10, sizeof(stepper_command_t));
    if (xQueue_stepper_command == NULL) ESP_LOGE(TAG, "Unable to create stepper command queue");

    long stepper_move = 0; // storage for incoming stepper command
    int stepper_target = 0;


    ESP_LOGI(TAG, "Start Stepper Task");
    while(1) {

        if (xQueueReceive(xQueue_stepper_command, &stepper_commands, portMAX_DELAY)) {
            //if type 0 DONT record the position (relative)
            //ESP_LOGI(TAG, "Stepper Type %d", stepper_commands.type);

            //stepper.setMaxSpeed(stepper_commands.speed); // 100mm/s @ 80 steps/mm


            if (stepper_commands.type == 1){

                if (stepper_commands.move <= stepper_commands.min) {
                    ESP_LOGI(TAG, "MIN");
                    stepper_target = stepper_commands.min;
                }
                else if (stepper_commands.move  >= stepper_commands.max){
                    ESP_LOGI(TAG, "MAX"); 
                    stepper_target = stepper_commands.max;
                }
                else {
                    stepper_target = stepper_commands.move;
                }
                stepper_move = (stepper_target - currentPosition) * factor;//works out based on mm

                ESP_LOGI(TAG, "Stepper Move To : %ld Dif %ld : Current : %ld",stepper_commands.move, stepper_move, currentPosition);

                currentPosition = stepper_target;
                 //save out and back to main = currentPosition;
            }
            if (stepper_commands.type == 0){
                stepper_move = stepper_commands.move;
                ESP_LOGI(TAG, "Stepper Move %ld : %ld", stepper_move, currentPosition);
            }
            //if type 1 record the position 
            //Print
            ESP_LOGI(TAG, "Stepper Move %ld", stepper_move);
            // Set distance to move from comand variable
            stepper.move(stepper_move);
            // Run the stepper loop until we get to our destination
            while(stepper.distanceToGo() != 0) {
                // if (!button1.pressed){
                if (button1.pressed) {
                    ESP_LOGI(TAG,"Button 1 has been pressed %u times\n", button1.numberKeyPresses);
                    button1.pressed = false;
                    server_ping("home");//Sends the boot up message to the server
                    stepper.stop();
                    
                    //stepper.move(200);
                }
                // }
                stepper.run();
                // vTaskDelay(1);
            }
        }
        
    }

}