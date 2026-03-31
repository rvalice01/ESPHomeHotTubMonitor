#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"

#include "consts.h"

namespace esphome {
namespace hottub_monitor {

// Globals (kept to match your original code)
extern int CommandedTemp;
extern int MeasuredTemp;
extern int Pump1State;
extern int Pump2State;
extern bool HeatingActive;
extern int ErrorMessages;
extern int LightState;

class HotTubMonitor : public Component, public api::CustomAPIDevice {
 public:
  void setup() override;
  void loop() override;

  // Used by sensor wrappers
  int get_error_messages() const { return ErrorMessages; }
  int get_measured_temp() const { return MeasuredTemp; }
  int get_light_state() const { return LightState; }

  // services / HA callback
  void On_Light_Button();
  void On_Reset_Button();
  void on_commanded_debug(esphome::api::StringRef state);

 protected:
  // main logic
  void Main_State_Machine();
  void Light_Timer();
  void Light_State_Machine();
  void Check_Hearbeat();
  void Debounce_Temperature();
  void Check_Main_Temperature();
  void Read_Temperature_ADC();
  void CalculateTemp(int AverageReading);
  void Service_Light_Button();

  // lighting helpers
  void LightsSetSolidColor(int color);
  void LightsSetRGBColor(int R, int G, int B);
  void LightsCycle();

  // data tables / state variables (moved from file-scope in your header)
  int LightArray_[HT_MAX_COLOR][3] = {
      {0, 0, 255},     /*blue*/
      {147, 0, 212},   /*violet*/
      {255, 0, 0},     /*red*/
      {237, 237, 0},   /*amber*/
      {0, 255, 0},     /*green*/
      {0, 255, 255},   /*aqua*/
      {255, 255, 255}, /*white*/
      {0, 0, 0}        /*psycho*/
  };

  int LightCycleOrder_[LIGHT_CYCLE_ARRAY_SIZE] = {HT_AMBER, HT_GREEN, HT_AQUA, HT_BLUE, HT_VIOLET};

  // temperature lookup table (from your context; 61 rows)
  int TemperatureLookup_[TEMP_LOOKUP_SIZE][TEMP_LOOKUP_VALUES] = {
      {0, 59},    {0, 60},    {5, 61},    {56, 62},   {115, 63},  {175, 64},
      {234, 64},  {293, 65},  {352, 66},  {412, 67},  {471, 68},  {530, 69},
      {589, 70},  {649, 71},  {708, 72},  {767, 73},  {826, 73},  {886, 74},
      {945, 75},  {1004, 76}, {1063, 77}, {1123, 78}, {1182, 79}, {1241, 80},
      {1301, 81}, {1360, 82}, {1419, 82}, {1478, 83}, {1538, 84}, {1597, 85},
      {1656, 86}, {1715, 87}, {1775, 88}, {1834, 89}, {1893, 90}, {1952, 91},
      {2012, 91}, {2071, 92}, {2130, 93}, {2189, 94}, {2249, 95}, {2308, 96},
      {2367, 97}, {2426, 98}, {2486, 99}, {2545, 99}, {2604, 100},{2664, 101},
      {2723, 102},{2782, 103},{2841, 104},{2901, 105},{2960, 106},{3019, 107},
      {3078, 108},{3138, 109},{3197, 109},{3256, 110},{3315, 111},{3375, 112},
      {3434, 113},
  };

  // state vars that were in the header
  bool LightCyclesFirstRun_{true};
  int MonitorState_{MONITOR_STATE_INIT};
  bool LightTurnOffFlag_{false};
  int LightButtonTimer_{0};
  int ToggleErrorCounter_{0};
  bool Hot_Tub_Debug_{0};

  int HT_State_{0};
  int HT_State_Inv_{0};
  int Pump1Timer_{0};
  int Pump2Timer_{0};
  int RawMeasuredTemp_{0};
};

}  // namespace hottub_monitor
}  // namespace esphome