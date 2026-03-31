#pragma once

// local TRUE/FALSE used by your code
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DEBUG 1

#define HA_SERVICE_UPDATE_INTERVAL 1000 /*ms*/

#define ONE_SECOND 1000 /*milliseconds*/
#define TWO_SECONDS 2000 /*milliseconds*/
#define THREE_SECONDS 3000 /*milliseconds*/
#define FIVE_SECONDS 5000 /*milliseconds*/
#define TIME_250_MS 250 /*milliseconds*/
#define TIME_100_MS 100 /*milliseconds*/
#define TIME_50_MS 50 /*milliseconds*/
#define TIME_20_MS 20 /*milliseconds*/
#define TIME_15_MS 15 /*milliseconds*/
#define SECONDS_IN_MIN 60

#define TASK_1_TIME TIME_250_MS
#define TASK_2_TIME TIME_50_MS
#define TASK_3_TIME ONE_SECOND
#define TASK_4_TIME TIME_15_MS

/* output pins */
#define MAIN_RELAY 13
#define PWM_R 16
#define PWM_G 17
#define PWM_B 5

/* input pins */
#define TEMPERATURE_SENSOR 35  /*db connector pin 5*/  // not used in your pasted code
#define HIGH_LIMIT_SWITCH 34   /*db connector pin 4*/
#define HEARTBEAT_INPUT 27
#define LIGHT_BUTTON 12

/* light button */
#define LIGHT_BUTTON_NOT_PRESSED 1
#define LIGHT_BUTTON_PRESSED 0
#define LIGHT_BUTTON_DEBOUNCE_CYCLES 10
#define LIGHT_BUTTON_TIMEOUT (5 * (ONE_SECOND / TASK_1_TIME)) /*5 seconds*/
#define LIGHT_TASK_TIME (ONE_SECOND / TASK_1_TIME)
#define LIGHT_ON_TIMEOUT 120 /*minutes*/

/* heartbeat (your context had the defines smashed together; reconstructing intent) */
#define HEARTBEAT_LIMIT_SECONDS 3
#define HEARTEBAT_CYCLE_TIME 4 /*cycles per second*/
#define HEARTBEAT_MAX_CYCLES (HEARTBEAT_LIMIT_SECONDS * HEARTEBAT_CYCLE_TIME)

/* state machine */
#define MONITOR_STATE_INIT 0
#define MONITOR_STATE_RUN 1
#define MONITOR_STATE_ERROR 2

/* error states */
#define ERR_OK 0x00
#define ERR_HEARTBEAT_STUCK 0x01
#define ERR_HEARTBEAT_TIMING 0x02
#define ERR_OVERTEMP 0x04

/* lights */
#define PWM_R_CHANNEL 0
#define PWM_G_CHANNEL 1
#define PWM_B_CHANNEL 2

#define PWM_FREQUENCY 200
#define PWM_RESOLUTION 8

#define BLUE 0
#define VIOLET 1
#define RED 2
#define AMBER 3
#define GREEN 4
#define AQUA 5
#define WHITE 6
#define PSYCHO 7
#define MAX_COLOR 8
#define LIGHTS_OFF 255
#define LIGHTS_CYCLE 254

#define ARRAY_RED 0
#define ARRAY_GREEN 1
#define ARRAY_BLUE 2

#define TRANSITION_TIME_CYCLES (THREE_SECONDS / TASK_2_TIME)
#define LIGHT_CYCLE_ARRAY_SIZE 5
#define MAX_RGB_VALUE 255

/* temperature */
#define TEMP_LOOKUP_SIZE 61
#define TEMP_LOOKUP_VALUES 2
#define TEMP_LOOKUP_ADVALUES 0
#define TEMP_LOOKUP_TEMPERATURE 1

#define ADC_SAMPLES 200
#define TEMP_DEBOUNCE_CYCLES 5
#define SAFETY_CUTOFF_TEMP 110