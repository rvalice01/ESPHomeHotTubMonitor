#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "hottub_monitor.h"

namespace esphome {
namespace hottub_monitor {

class HotTubBaseSensor : public PollingComponent, public sensor::Sensor {
 public:
  HotTubBaseSensor(HotTubMonitor *parent, uint32_t update_ms)
      : PollingComponent(update_ms), parent_(parent) {}

 protected:
  HotTubMonitor *parent_;
};

class ErrorMessagesSensor : public HotTubBaseSensor {
 public:
  using HotTubBaseSensor::HotTubBaseSensor;
  void update() override { this->publish_state(parent_->get_error_messages()); }
};

class MeasuredTemperatureSensor : public HotTubBaseSensor {
 public:
  using HotTubBaseSensor::HotTubBaseSensor;
  void update() override { this->publish_state(parent_->get_measured_temp()); }
};

class LightStatusSensor : public HotTubBaseSensor {
 public:
  using HotTubBaseSensor::HotTubBaseSensor;
  void update() override { this->publish_state(parent_->get_light_state()); }
};

}  // namespace hottub_monitor
}  // namespace esphome