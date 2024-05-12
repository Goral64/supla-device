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

#include "custom_relay.h"

#include <supla/log_wrapper.h>
#include <supla/sensor/binary_parsed.h>
#include <supla/template/template.h>
#include <supla/time.h>

#include <cstdio>

Supla::Control::CustomRelay::CustomRelay(
    Supla::Parser::Parser *parser,
    Supla::Template::Template *templateValue,
    _supla_int_t functions)
    : Supla::Sensor::SensorParsed<Supla::Control::CustomVirtualRelay>(parser),
      Supla::Template::ControlTemplate<Supla::Control::CustomVirtualRelay>(
          templateValue) {
  channel.setFuncList(functions);
}

void Supla::Control::CustomRelay::onInit() {
  VirtualRelay::onInit();
  registerActions();
}

void Supla::Control::CustomRelay::turnOn(_supla_int_t duration) {
  Supla::Control::VirtualRelay::turnOn(duration);
  channel.setNewValue(isOn());

  templateValue->turnOn(setOnValue);

  if (cmdOn.length() > 0) {
    auto p = popen(cmdOn.c_str(), "r");
    pclose(p);
  }
}

void Supla::Control::CustomRelay::turnOff(_supla_int_t duration) {
  Supla::Control::VirtualRelay::turnOff(duration);
  channel.setNewValue(isOn());

  templateValue->turnOff(setOffValue);

  if (cmdOff.length() > 0) {
    auto p = popen(cmdOff.c_str(), "r");
    pclose(p);
  }
}

void Supla::Control::CustomRelay::setCmdOn(const std::string &newCmdOn) {
  cmdOn = newCmdOn;
}

void Supla::Control::CustomRelay::setCmdOff(const std::string &newCmdOff) {
  cmdOff = newCmdOff;
}

void Supla::Control::CustomRelay::setTurnOn(const std::string &newSetOn) {
  setOn = newSetOn;
}

void Supla::Control::CustomRelay::setTurnOff(const std::string &newSetOff) {
  setOff = newSetOff;
}

bool Supla::Control::CustomRelay::isOn() {
  bool newState = false;

  int result = 0;
  if (parser) {
    result = getStateValue();
    if (result == 1) {
      newState = true;
    } else if (result != -1) {
      result = 0;
    }
  } else {
    newState = Supla::Control::VirtualRelay::isOn();
  }

  setLastState(result);

  return newState;
}

void Supla::Control::CustomRelay::iterateAlways() {
  Supla::Control::Relay::iterateAlways();

  if (parser && (millis() - lastReadTime > 100)) {
    lastReadTime = millis();
    channel.setNewValue(isOn());
    if (isOffline()) {
      channel.setOffline();
    } else {
      channel.setOnline();
    }
  }
}

bool Supla::Control::CustomRelay::isOffline() {
  if (useOfflineOnInvalidState && parser) {
    if (getStateValue() == -1) {
      return true;
    }
  }
  return false;
  //    return Supla::Control::VirtualRelay::isOffline();
}

void Supla::Control::CustomRelay::setUseOfflineOnInvalidState(
    bool useOfflineOnInvalidState) {
  this->useOfflineOnInvalidState = useOfflineOnInvalidState;
  SUPLA_LOG_INFO("useOfflineOnInvalidState = %d", useOfflineOnInvalidState);
}
