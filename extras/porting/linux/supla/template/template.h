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

#ifndef EXTRAS_PORTING_LINUX_SUPLA_TEMPLATE_TEMPLATE_H_
#define EXTRAS_PORTING_LINUX_SUPLA_TEMPLATE_TEMPLATE_H_

#include <supla/output/output.h>

#include <map>
#include <string>
#include <variant>

namespace Supla {
namespace Template {

class Template {
 public:
  explicit Template(Supla::Output::Output *);
  virtual ~Template() {
  }

  virtual void addKey(const std::string &key, int index);
  virtual bool isBasedOnIndex() = 0;

  void turnOn(std::variant<int, bool, std::string> onValue);

  void turnOff(std::variant<int, bool, std::string> offValue);

 protected:
  std::map<std::string, int> keys;
  Supla::Output::Output *output = nullptr;
};
};  // namespace Template
};  // namespace Supla

#endif  // EXTRAS_PORTING_LINUX_SUPLA_TEMPLATE_TEMPLATE_H_