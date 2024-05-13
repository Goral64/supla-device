/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef EXTRAS_PORTING_LINUX_SUPLA_CONTROL_CUSTOM_RELAY_H_
#define EXTRAS_PORTING_LINUX_SUPLA_CONTROL_CUSTOM_RELAY_H_

#include <supla/control/control_template.h>
#include <supla/control/virtual_relay.h>
#include <supla/sensor/sensor_parsed.h>
#include <supla/template/template.h>

#include <string>

#include "custom_virtual_relay.h"

namespace Supla {
namespace Template {
const char State[] = "set_state";
const char SetOn[] = "set_on";
const char SetOff[] = "set_off";
};  // namespace Template

namespace Control {
class CustomRelay : public Sensor::SensorParsed<CustomVirtualRelay>,
                    public Template::ControlTemplate<CustomVirtualRelay> {
 public:
  CustomRelay(Supla::Parser::Parser *parser,
              Supla::Template::Template *templateValue,
              _supla_int_t functions =
                  (0xFF ^ SUPLA_BIT_FUNC_CONTROLLINGTHEROLLERSHUTTER));

  void onInit() override;
  void turnOn(_supla_int_t duration = 0) override;
  void turnOff(_supla_int_t duration = 0) override;
  bool isOn() override;
  void iterateAlways() override;
  bool isOffline();  // add override

  void setCmdOn(const std::string &);
  void setCmdOff(const std::string &);
  void setTurnOn(const std::string &);
  void setTurnOff(const std::string &);
  void setUseOfflineOnInvalidState(bool useOfflineOnInvalidState);

 protected:
  std::string cmdOn;
  std::string cmdOff;
  std::string setOn;
  std::string setOff;
  uint32_t lastReadTime = 0;
  bool useOfflineOnInvalidState = false;
};

};  // namespace Control
};  // namespace Supla

#endif  // EXTRAS_PORTING_LINUX_SUPLA_CONTROL_CUSTOM_RELAY_H_