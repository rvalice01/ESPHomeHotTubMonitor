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

class ErrorMessagesSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;
  void update() override {
    if (sensor_ != nullptr) sensor_->publish_state(parent_->get_error_messages());
  }
};

class MeasuredTemperatureSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;
  void update() override {
    if (sensor_ != nullptr) sensor_->publish_state(parent_->get_measured_temp());
  }
};

class LightStatusSensor : public HotTubSensorBase {
 public:
  using HotTubSensorBase::HotTubSensorBase;
  void update() override {
    if (sensor_ != nullptr) sensor_->publish_state(parent_->get_light_state());
  }
};

}  // namespace hottub_monitor
}  // namespace esphome