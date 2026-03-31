#include "hottub_monitor.h"
#include "hottub_monitor_sensors.h"
#include "esphome/core/log.h"
#include "Arduino.h"
//FUTURE #include "esphome/components/api/api_string.h"

namespace esphome {
namespace hottub_monitor {

static const char *const TAG = "hottub_monitor";

// Globals (kept to match your original code)
int CommandedTemp = 0;
int MeasuredTemp = 0;
int Pump1State = 0;
int Pump2State = 0;
bool HeatingActive = false;
int ErrorMessages = 0;
int LightState = HT_LIGHTS_OFF;

void HotTubMonitor::setup() {
  int RawADValue = 0;

  // init vars
  LightState = HT_LIGHTS_OFF;

  // setup pins / PWM
  analogSetPinAttenuation(HIGH_LIMIT_SWITCH, ADC_12db);

  pinMode(HEARTBEAT_INPUT, INPUT);
  pinMode(MAIN_RELAY, OUTPUT);
  pinMode(LIGHT_BUTTON, INPUT_PULLUP);

  // New Arduino-ESP32 LEDC API (works on recent cores)
  ledcAttach(PWM_R, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(PWM_G, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttach(PWM_B, PWM_FREQUENCY, PWM_RESOLUTION);

  // write initial 0
  ledcWrite(PWM_R, 0);
  ledcWrite(PWM_G, 0);
  ledcWrite(PWM_B, 0);

  // register services
  register_service(&HotTubMonitor::On_Light_Button, "Hot_Tub_Light_Button");
  register_service(&HotTubMonitor::On_Reset_Button, "Reset_Button");

  // HA value
  subscribe_homeassistant_state(&HotTubMonitor::on_commanded_debug, "input_number.hot_tub_debug");

  MonitorState_ = MONITOR_STATE_INIT;

  // initialize temperature with 1 a/d read
  RawADValue = analogRead(HIGH_LIMIT_SWITCH);
  CalculateTemp(RawADValue);
  MeasuredTemp = RawMeasuredTemp_;
}

void HotTubMonitor::loop() {
  static unsigned long PreviousTime1 = 0;
  static unsigned long PreviousTime2 = 0;
  static unsigned long PreviousTime3 = 0;
  static unsigned long PreviousTime4 = 0;

  unsigned long CurrentTime = millis();
  unsigned long DeltaTime = 0;

  // task loop 1
  DeltaTime = CurrentTime - PreviousTime1;
  if (DeltaTime >= TASK_1_TIME) {
    Main_State_Machine();
    Light_Timer();
    PreviousTime1 = CurrentTime;
  }

  // task loop 2
  DeltaTime = CurrentTime - PreviousTime2;
  if (DeltaTime >= TASK_2_TIME) {
    PreviousTime2 = CurrentTime;
    if (MonitorState_ != MONITOR_STATE_ERROR) {
      Light_State_Machine();
    }
  }

  // task loop 3
  DeltaTime = CurrentTime - PreviousTime3;
  if (DeltaTime >= TASK_3_TIME) {
    Debounce_Temperature();
    Check_Main_Temperature();
    PreviousTime3 = CurrentTime;
  }

  // task loop 4
  DeltaTime = CurrentTime - PreviousTime4;
  if (DeltaTime >= TASK_4_TIME) {
    Service_Light_Button();
    Read_Temperature_ADC();
    PreviousTime4 = CurrentTime;
  }
}

void HotTubMonitor::Check_Main_Temperature() {
  if (MeasuredTemp >= SAFETY_CUTOFF_TEMP) {
    // Set error
    ErrorMessages = (ErrorMessages | ERR_OVERTEMP);
    MonitorState_ = MONITOR_STATE_ERROR;
    LightsSetSolidColor(HT_RED);
  }
}

void HotTubMonitor::Debounce_Temperature() {
  static int DebounceCounter = 0;
  static int PreviousTemp = 0;
  int CurrentTemp = RawMeasuredTemp_;

  if (CurrentTemp == PreviousTemp) {
    if (DebounceCounter != TEMP_DEBOUNCE_CYCLES) {
      DebounceCounter++;
    }
  } else {
    DebounceCounter = 0;
  }

  if (DebounceCounter == TEMP_DEBOUNCE_CYCLES) {
    MeasuredTemp = CurrentTemp;
  }

  PreviousTemp = CurrentTemp;
}

void HotTubMonitor::Read_Temperature_ADC() {
  static int counter = 0;
  static double average = 0;
  int temp;

  if (counter < ADC_SAMPLES) {
    counter++;
    average += analogRead(HIGH_LIMIT_SWITCH);
  } else {
    temp = (int) (average / ADC_SAMPLES);
    average = 0;
    counter = 0;
    CalculateTemp(temp);
  }
}

void HotTubMonitor::CalculateTemp(int AverageReading) {
  int midpoint = 0;
//ESP_LOGD(TAG, "ADC raw=%d", AverageReading);
  // validate input range
  if ((AverageReading > TemperatureLookup_[TEMP_LOOKUP_SIZE - 1][TEMP_LOOKUP_ADVALUES]) ||
      (AverageReading < TemperatureLookup_[0][TEMP_LOOKUP_ADVALUES])) {
    // out of range -> clamp to highest value to prevent heater enabling
    AverageReading = TemperatureLookup_[TEMP_LOOKUP_SIZE - 1][TEMP_LOOKUP_ADVALUES];
    ESP_LOGD(TAG, "Temp out of range (clamped)");
  }

  // lookup table: start from end for efficiency (as your comment says)
  for (int i = (TEMP_LOOKUP_SIZE - 1); i >= 0; i--) {
    if (AverageReading > TemperatureLookup_[i][TEMP_LOOKUP_ADVALUES]) {

      midpoint = TemperatureLookup_[i][TEMP_LOOKUP_ADVALUES] -
                 ((TemperatureLookup_[i][TEMP_LOOKUP_ADVALUES] -
                   TemperatureLookup_[i - 1][TEMP_LOOKUP_ADVALUES]) /
                  2);

      if (AverageReading > midpoint) {
        RawMeasuredTemp_ = TemperatureLookup_[i][TEMP_LOOKUP_TEMPERATURE];
      } else {
        RawMeasuredTemp_ = TemperatureLookup_[i - 1][TEMP_LOOKUP_TEMPERATURE];
      }
      //ESP_LOGD(TAG, "Temp=%d", RawMeasuredTemp_);
      return;
    }
  }

  // if we never matched, clamp to first
  RawMeasuredTemp_ = TemperatureLookup_[0][TEMP_LOOKUP_TEMPERATURE];
}

void HotTubMonitor::Light_State_Machine() {
  switch (LightState) {
    case HT_LIGHTS_OFF:
      LightsSetRGBColor(0, 0, 0);
      LightCyclesFirstRun_ = 1;
      break;

    case HT_LIGHTS_CYCLE:
      LightsCycle();
      break;

    case HT_BLUE:
    case HT_VIOLET:
    case HT_RED:
    case HT_AMBER:
    case HT_GREEN:
    case HT_AQUA:
    case HT_WHITE:
    case HT_PSYCHO:
      if (LightState == HT_PSYCHO) {
        LightsSetRGBColor(random(0, 255), random(0, 255), random(0, 255));
      } else {
        LightsSetSolidColor(LightState);
      }
      break;

    default:
      LightsSetSolidColor(HT_RED);
      break;
  }
}

void HotTubMonitor::Main_State_Machine() {
  switch (MonitorState_) {
    case MONITOR_STATE_INIT:
      digitalWrite(MAIN_RELAY, HIGH);
      MonitorState_ = MONITOR_STATE_RUN;
      break;

    case MONITOR_STATE_RUN:
      if (Hot_Tub_Debug_ != DEBUG) {
        Check_Hearbeat();
      }
      break;

    case MONITOR_STATE_ERROR:
      digitalWrite(MAIN_RELAY, LOW);
      break;

    default:
      MonitorState_ = MONITOR_STATE_ERROR;
      break;
  }
}

void HotTubMonitor::Check_Hearbeat() {
  int PinStatus = digitalRead(HEARTBEAT_INPUT);
  static int ToggleHighCounter = 0;
  static int ToggleLowCounter = 0;
  static int PreviousPinStatus = 0;

  // toggled?
  if (PinStatus != PreviousPinStatus) {
    if (PinStatus == HIGH) {
      if (ToggleLowCounter != HEARTEBAT_CYCLE_TIME) {
        ToggleErrorCounter_++;
      } else if (ToggleErrorCounter_ > 0) {
        ToggleErrorCounter_--;
      }
    } else {
      if (ToggleHighCounter != HEARTEBAT_CYCLE_TIME) {
        ToggleErrorCounter_++;
      } else if (ToggleErrorCounter_ > 0) {
        ToggleErrorCounter_--;
      }
    }

    ToggleHighCounter = 0;
    ToggleLowCounter = 0;
  }

  // increment counter
  if (PinStatus == HIGH) {
    ToggleHighCounter++;
  } else {
    ToggleLowCounter++;
  }

  // check counters expired
  if ((ToggleHighCounter >= HEARTBEAT_MAX_CYCLES) ||
      (ToggleLowCounter >= HEARTBEAT_MAX_CYCLES)) {
    ErrorMessages = (ErrorMessages | ERR_HEARTBEAT_STUCK);
    MonitorState_ = MONITOR_STATE_ERROR;
    LightsSetSolidColor(HT_RED);
  }

  if (ToggleErrorCounter_ >= HEARTBEAT_MAX_CYCLES) {
    ErrorMessages = (ErrorMessages | ERR_HEARTBEAT_TIMING);
    MonitorState_ = MONITOR_STATE_ERROR;
    LightsSetSolidColor(HT_RED);
  }

  PreviousPinStatus = PinStatus;
}

void HotTubMonitor::On_Light_Button() {
  if (LightState == HT_LIGHTS_OFF) {
    LightState = HT_LIGHTS_CYCLE;
  } else if (LightState == HT_LIGHTS_CYCLE) {
    LightState = HT_BLUE;
  } else if (LightState < HT_MAX_COLOR) {
    LightState++;
  }

  if (LightState == HT_MAX_COLOR) {
    LightState = HT_LIGHTS_OFF;
  }

  if (LightTurnOffFlag_ == 0) {
    LightButtonTimer_ = 0;
  } else {
    LightState = HT_LIGHTS_OFF;
  }
}

void HotTubMonitor::On_Reset_Button() {
  MonitorState_ = MONITOR_STATE_INIT;
  ErrorMessages = ERR_OK;
  LightState = HT_LIGHTS_OFF;
  ToggleErrorCounter_ = 0;
}

//FUTURE void HotTubMonitor::on_commanded_debug(esphome::api::StringRef state) {
//  // StringRef is not null-terminated; convert safely
//  std::string s(state.data(), state.size());
//  Hot_Tub_Debug_ = (bool) std::stoi(s);
//}
void HotTubMonitor::on_commanded_debug(std::string state) {
  Hot_Tub_Debug_ = (bool) std::stoi(state);
}

void HotTubMonitor::Service_Light_Button() {
  static int LightPreviousButton = LIGHT_BUTTON_NOT_PRESSED;
  static int LightDebounceCounter = 0;

  if (digitalRead(LIGHT_BUTTON) == LIGHT_BUTTON_PRESSED) {
    if ((LightDebounceCounter >= LIGHT_BUTTON_DEBOUNCE_CYCLES) &&
        (LightPreviousButton == LIGHT_BUTTON_NOT_PRESSED)) {
      On_Light_Button();
      LightDebounceCounter = LIGHT_BUTTON_DEBOUNCE_CYCLES;
      LightPreviousButton = LIGHT_BUTTON_PRESSED;
    } else {
      LightDebounceCounter++;
    }
  } else {
    LightPreviousButton = LIGHT_BUTTON_NOT_PRESSED;
    LightDebounceCounter = 0;
  }
}

void HotTubMonitor::Light_Timer() {
  static long LightSecondCounter = 0;

  // after 5 seconds of no activity, next button push should turn off
  if (LightState == HT_LIGHTS_OFF) {
    LightTurnOffFlag_ = 0;
    LightButtonTimer_ = 0;
  }

  if (LightButtonTimer_ >= LIGHT_BUTTON_TIMEOUT) {
    LightTurnOffFlag_ = 1;
  } else {
    if (LightState != HT_LIGHTS_OFF) {
      LightButtonTimer_++;
    }
  }

  // after 2 hours turn lights off
  if (LightSecondCounter < (LIGHT_ON_TIMEOUT * SECONDS_IN_MIN * LIGHT_TASK_TIME)) {
    LightSecondCounter++;
  } else {
    LightState = HT_LIGHTS_OFF;
  }

  if (LightState == HT_LIGHTS_OFF) {
    LightSecondCounter = 0;
  }
}

void HotTubMonitor::LightsCycle() {
  static float LC_R = 0;
  static float LC_G = 0;
  static float LC_B = 0;

  static float LC_Target_R = 0;
  static float LC_Target_G = 0;
  static float LC_Target_B = 0;

  static float LC_Step_Size_R = 0;
  static float LC_Step_Size_G = 0;
  static float LC_Step_Size_B = 0;

  static int CycleArrayElement = 0;
  static int CycleTimeCounter = 0;

  if (LightCyclesFirstRun_ == 1) {
    LC_R = 0;
    LC_G = 0;
    LC_B = 0;

    CycleArrayElement = 0;
    CycleTimeCounter = 0;

    LC_Target_R = LightArray_[LightCycleOrder_[CycleArrayElement]][ARRAY_RED];
    LC_Target_G = LightArray_[LightCycleOrder_[CycleArrayElement]][ARRAY_GREEN];
    LC_Target_B = LightArray_[LightCycleOrder_[CycleArrayElement]][ARRAY_BLUE];

    LightCyclesFirstRun_ = 0;
  }

  if (CycleTimeCounter == 0) {
    LC_Step_Size_R = (LC_Target_R != LC_R) ? ((LC_Target_R - LC_R) / TRANSITION_TIME_CYCLES) : 0;
    LC_Step_Size_G = (LC_Target_G != LC_G) ? ((LC_Target_G - LC_G) / TRANSITION_TIME_CYCLES) : 0;
    LC_Step_Size_B = (LC_Target_B != LC_B) ? ((LC_Target_B - LC_B) / TRANSITION_TIME_CYCLES) : 0;
  }

  // apply steps (clamp)
  auto step_clamp = [](float &v, float step) {
    if (step > 0) {
      v = (v + step > MAX_RGB_VALUE) ? MAX_RGB_VALUE : (v + step);
    } else if (step < 0) {
      v = (v + step < 0) ? 0 : (v + step);
    }
  };

  step_clamp(LC_R, LC_Step_Size_R);
  step_clamp(LC_G, LC_Step_Size_G);
  step_clamp(LC_B, LC_Step_Size_B);

  CycleTimeCounter++;

  if (CycleTimeCounter >= TRANSITION_TIME_CYCLES) {
    LC_R = LC_Target_R;
    LC_G = LC_Target_G;
    LC_B = LC_Target_B;

    CycleTimeCounter = 0;

    CycleArrayElement++;
    if (CycleArrayElement >= LIGHT_CYCLE_ARRAY_SIZE) {
      CycleArrayElement = 0;
    }

    // your context shows you started randomizing targets
    LC_Target_R = random(0, 255);
    LC_Target_G = random(0, 255);
    LC_Target_B = random(0, 255);
  }

  LightsSetRGBColor((int) LC_R, (int) LC_G, (int) LC_B);
}

void HotTubMonitor::LightsSetSolidColor(int color) {
  ledcWrite(PWM_R, LightArray_[color][ARRAY_RED]);
  ledcWrite(PWM_G, LightArray_[color][ARRAY_GREEN]);
  ledcWrite(PWM_B, LightArray_[color][ARRAY_BLUE]);
}

void HotTubMonitor::LightsSetRGBColor(int R, int G, int B) {
  ledcWrite(PWM_R, R);
  ledcWrite(PWM_G, G);
  ledcWrite(PWM_B, B);
}

}  // namespace hottub_monitor
}  // namespace esphome