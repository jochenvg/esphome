#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace fujitsu_general_ar_je5 {

const uint8_t FUJITSU_GENERAL_TEMP_MIN = 16;  // Celsius // TODO 16 for heating, 18 for cooling, unsupported in ESPH
const uint8_t FUJITSU_GENERAL_TEMP_MAX = 30;  // Celsius

// clang-format off
/**
 * ```
 *                                                                                               turn
 *                                                                                               on  temp mode     fan
 *                     0x14     0x63     0x00     0x10     0x10      0xFC     0x08     0x30      *   |  | | |      | |
 *
 * temperatures                                                                                  1   1248 124      124
 * auto auto 18        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000100 00000000 00000000 00000000  00000000 00000000 chksum
 * auto auto 19        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10001100 00000000 00000000 00000000  00000000 00000000 chksum
 * auto auto 30        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 00000000 00000000 00000000  00000000 00000000 chksum
 *
 * on flag:
 * on at 16            00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000000 00100000 00000000 00000000  00000000 00000000 chksum
 * down to 16          00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  00000000 00100000 00000000 00000000  00000000 00000000 chksum
 *
 * mode options:
 * auto auto 30        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 00000000 00000000 00000000  00000000 00000000 chksum
 * cool auto 30        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 10000000 00000000 00000000  00000000 00000000 chksum
 * dry auto 30         00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 01000000 00000000 00000000  00000000 00000000 chksum
 * fan (auto) (30)     00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 11000000 00000000 00000000  00000000 00000000 chksum
 * heat auto 30        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 00100000 00000000 00000000  00000000 00000000 chksum
 *
 * fan options:
 * heat 30 high        00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  10000111 00100000 10000000 00000000  00000000 00000000 chksum
 * heat 30 med         00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  00000111 00100000 01000000 00000000  00000000 00000000 chksum
 * heat 30 low         00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  00000111 00100000 11000000 00000000  00000000 00000000 chksum
 * heat 30 quiet       00101000 11000110 00000000 00001000  00001000 00111111 00010000 00001100  00000111 00100000 00100000 00000000  00000000 00000000 chksum
 *
 * ```
 */
// clang-format on

class FujitsuGeneralClimate : public climate_ir::ClimateIR {
 public:
  FujitsuGeneralClimate()
      : ClimateIR(FUJITSU_GENERAL_TEMP_MIN, FUJITSU_GENERAL_TEMP_MAX, 1.0f, true, true,
                  {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                   climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                  {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

 protected:
  /// Override control to cater for separate transmission of swing mode change.
  void control(const climate::ClimateCall &call) override;
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;
  /// Transmit via IR power off command.
  void transmit_off_();
  /// Transmit via IR swing command.
  void transmit_swing_();

  /// Parse incoming message
  bool on_receive(remote_base::RemoteReceiveData data) override;

  /// Transmit message as IR pulses
  void transmit_(uint8_t const *message, uint8_t length);

  /// Calculate checksum for a state message
  uint8_t checksum_state_(uint8_t const *message);

  /// Calculate cecksum for a util message
  uint8_t checksum_util_(uint8_t const *message);

  // true if currently on - fujitsus transmit an on flag on when the remote moves from off to on
  bool power_{false};
};

}  // namespace fujitsu_general_ar_je5
}  // namespace esphome
