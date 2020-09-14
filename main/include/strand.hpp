#ifndef _UTILS_H
#define _UTILS_H

#include "main.h"

#ifdef __cplusplus
    extern "C" {
#endif

// ----VARIABLES----
extern QueueHandle_t xQueue_stepper_command; // This external reference has to be defined again in Strand.c


// ----FUNCTIONS----
void init_strand();
void stepper_task(void *args);

void command_move(int move, int type, int speed, int accel, int min, int max); 

#ifdef __cplusplus
  }
#endif

#endif // _UTILS_H