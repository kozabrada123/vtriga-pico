#ifndef SEVEN_SEGMENT_HPP
#define SEVEN_SEGMENT_HPP

#include "constants.hpp"
#include "pico/stdlib.h"

class SevenSegmentDisplay {

public:
	SevenSegmentDisplay();
	SevenSegmentDisplay(uint32_t common_pin, bool common_anode = false);

	/// Sets the common cathode to 0 / the common anode to 1, enables the display
	void select();
	/// Sets the common cathode to 1 / the common anode to 0, disables the display
	void deselect();

	/// Prints data to the display, where
	/// bit 0 - a
	/// bit 1 - b
	/// bit 2 - c
	/// bit 3 - d
	/// bit 4 - e
	/// bit 5 - f
	/// bit 6 - g
	/// bit 7 - dot
	///
	/// This method automatically takes into account whether
	/// the display is common-cathode or common-anode
	void print_bits(uint8_t data);

	/// Prints a digit (0 - 9) to the display
	///
	/// If the uint_8 cannot be displayed (ex. > 9), this method silently does nothing
	void print_digit(uint8_t digit);

	/// Prints a hex digit (0 - F) to the display
	///
	/// If the uint_8 cannot be displayed (ex. > F), this method silently does nothing
	void print_hex_digit(uint8_t digit);

	/// Prints data to the display, where
	/// bit 0 - a
	/// bit 1 - b
	/// bit 2 - c
	/// bit 3 - d
	/// bit 4 - e
	/// bit 5 - f
	/// bit 6 - g
	/// bit 7 - dot
	///
	/// This method automatically takes into account whether
	/// the display is common-cathode or common-anode
	///
	/// It also automatically clear bits and calls select()
	void safe_print_bits(uint8_t data);

	/// Prints a digit (0 - 9) to the display
	///
	/// If the uint_8 cannot be displayed (ex. > 9), this method silently does nothing
	///
	/// It also automatically clear bits and calls select()
	void safe_print_digit(uint8_t digit);

	/// Prints a hex digit (0 - F) to the display
	///
	/// If the uint_8 cannot be displayed (ex. > F), this method silently does nothing
	///
	/// It also automatically clear bits and calls select()
	void safe_print_hex_digit(uint8_t digit);

	/// Prints a minus '-' to the seven segment display
	void print_minus();

	/// Sets the dot pin to on or off
	void set_dot(bool state);

private:

  /// Whether or not the display is common anode, instead of common cathode
  bool common_anode;

  /// Common cathode / anode
  uint32_t common_pin;
  const uint32_t a_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_A;
  const uint32_t b_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_B;
  const uint32_t c_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_C;
  const uint32_t d_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_D;
  const uint32_t e_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_E;
  const uint32_t f_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_F;
  const uint32_t g_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_G;
  const uint32_t dot_pin = PERIPHERAL_PIN_SEVEN_SEGMENT_DOT;
};

#endif
