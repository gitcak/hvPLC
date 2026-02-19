// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#pragma once
#include "m5unified/Button_Class.hpp"
#include "reference_upstream/app/hal/hal.h"


namespace SYSTEM {
namespace INPUTS {
/**
 * @brief Button class with richer apis
 *
 */
class Button : public m5::Button_Class {
public:
  Button(Gamepad::GamepadButton_t button) : _button(button) {}
  void update();

  /**
   * @brief Button instances
   *
   * @return Button*
   */
  static Button *Back();
  static Button *Next();
  static Button *Ok();

  /**
   * @brief Helper method to update all button instances
   *
   */
  static void Update();

private:
  Gamepad::GamepadButton_t _button;
};

} // namespace INPUTS
} // namespace SYSTEM
