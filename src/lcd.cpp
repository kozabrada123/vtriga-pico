#include "lcd.hpp"
#include "constants.hpp"
#include "hardware/gpio.h"
#include <pico/time.h>

LcdDisplay::LcdDisplay() {}

void LcdDisplay::pulse_enable() {
  gpio_put(pin_enable, true);
  sleep_us(40);
  gpio_put(pin_enable, false);
  sleep_us(40);
}

void LcdDisplay::send_to_lcd(uint8_t data) {
  gpio_put(pin_data_0, data & 0b1);
  gpio_put(pin_data_1, (data & 0b10) >> 1);
  gpio_put(pin_data_2, (data & 0b100) >> 2);
  gpio_put(pin_data_3, (data & 0b1000) >> 3);
  gpio_put(pin_data_4, (data & 0b10000) >> 4);
  gpio_put(pin_data_5, (data & 0b100000) >> 5);
  gpio_put(pin_data_6, (data & 0b1000000) >> 6);
  gpio_put(pin_data_7, (data & 0b10000000) >> 7);
  pulse_enable();
}

void LcdDisplay::set_register_select(bool data) {
  gpio_put(pin_register_select, data);
}

void LcdDisplay::clear_display() {
  set_register_select(false);
  send_to_lcd(0b00000001);
  sleep_us(1530);
}

void LcdDisplay::return_home() {
  set_register_select(false);
  send_to_lcd(0b00000010);
  sleep_us(1530);
}

/// Direction = 1 -> left to right
/// Direction = 0 -> right to left
void LcdDisplay::set_entry_mode(bool direction, bool shift_cursor) {
  set_register_select(false);
  send_to_lcd(0b0000100 | ((uint8_t)direction << 1) | (uint8_t)!shift_cursor);
  sleep_us(39);
}

void LcdDisplay::set_data_shift_direction(bool reverse) {
  set_register_select(false);
  send_to_lcd(0b00000110 | ((uint8_t)reverse));
  sleep_us(39);
}

void LcdDisplay::set_display_flags(bool display_enabled, bool cursor_enabled, bool cursor_blink_enabled) {
  set_register_select(false);
  send_to_lcd(0b00001000 | (((uint8_t) display_enabled) << 2) | (((uint8_t) cursor_enabled) << 1) | (uint8_t) cursor_blink_enabled);
  sleep_us(39);
}

void LcdDisplay::set_display_extended_functions(bool high_font_width, bool invert_black_white, bool four_line_mode) {
  set_register_select(false);
  send_to_lcd(0b00001000 | (((uint8_t) high_font_width) << 2) | (((uint8_t) invert_black_white) << 1) | (uint8_t) four_line_mode);
  sleep_us(39);
}

void LcdDisplay::set_display_function(bool enable_8_bit_bus) {
  set_register_select(false);
  send_to_lcd(0b00100000 | (((uint8_t) enable_8_bit_bus) << 4));
  sleep_us(39);
}

void LcdDisplay::write_to_lcd(uint8_t data) {
  set_register_select(true);
  send_to_lcd(data);
  sleep_us(5);
}

void LcdDisplay::setup_lcd() {
  set_register_select(false);
  send_to_lcd(0b00110100);
  sleep_ms(1);
  send_to_lcd(0b00001001);
  sleep_ms(1);
  send_to_lcd(0b00110000);
  sleep_ms(1);
  send_to_lcd(0b00001110);
  sleep_ms(1);
  clear_display();
  send_to_lcd(0b00000110);
}
