#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "hottub_monitor.h"

namespace esphome {
namespace hottub_monitor {

class HotTubSensorBase : public PollingComponent {
 public:
  HotTubSensorBase(HotTubMonitor *parent, uint32_t update_ms)
      : PollingComponent(update_ms), parent_(parent) {}

  void set_sensor(sensor::Sensor *s) { sensor_ = s; }

 protected:
  HotTubMonitor *parent_;
  sensor::Sensor *sensor_{nullptr};
};

// Error messages (int/bitmask)
class ErrorMessagesSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;

  void update() override {
    if (sensor_ == nullptr) return;

    int v = parent_->get_error_messages();
    if (!has_last_ || v != last_) {
      last_ = v;
      has_last_ = true;
      sensor_->publish_state(v);
    }
  }

 protected:
  bool has_last_{false};
  int last_{0};
};

// Measured temperature (int)
class MeasuredTemperatureSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;

  void update() override {
    if (sensor_ == nullptr) return;

    int v = parent_->get_measured_temp();
    if (!has_last_ || v != last_) {
      last_ = v;
      has_last_ = true;
      sensor_->publish_state(v);
    }
  }

 protected:
  bool has_last_{false};
  int last_{0};
};

// Light status (int enum)
class LightStatusSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;

  void update() override {
    if (sensor_ == nullptr) return;

    int v = parent_->get_light_state();
    if (!has_last_ || v != last_) {
      last_ = v;
      has_last_ = true;
      sensor_->publish_state(v);
    }
  }

 protected:
  bool has_last_{false};
  int last_{0};
};

}  // namespace hottub_monitor
}  // namespace esphome