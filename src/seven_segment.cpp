#include "seven_segment.hpp"
#include "hardware/gpio.h"

SevenSegmentDisplay::SevenSegmentDisplay() {}

SevenSegmentDisplay::SevenSegmentDisplay(uint32_t common_p, bool common_anode) {
  this->common_anode = common_anode;
  common_pin = common_p;

  // Already handled in main
  /*gpio_init(common_pin);
  gpio_set_dir(common_pin, GPIO_OUT);

  gpio_init(a_pin);
  gpio_set_dir(a_pin, GPIO_OUT);

  gpio_init(b_pin);
  gpio_set_dir(b_pin, GPIO_OUT);

  gpio_init(c_pin);
  gpio_set_dir(c_pin, GPIO_OUT);

  gpio_init(d_pin);
  gpio_set_dir(d_pin, GPIO_OUT);

  gpio_init(e_pin);
  gpio_set_dir(e_pin, GPIO_OUT);

  gpio_init(f_pin);
  gpio_set_dir(f_pin, GPIO_OUT);

  gpio_init(g_pin);
  gpio_set_dir(g_pin, GPIO_OUT);

  gpio_init(d_pin);
  gpio_set_dir(d_pin, GPIO_OUT);*/
}

void SevenSegmentDisplay::select() {
  if (common_anode) {
    gpio_put(common_pin, 1);
  } else {
    gpio_put(common_pin, 0);
  }
}

void SevenSegmentDisplay::deselect() {
  if (common_anode) {
    gpio_put(common_pin, 0);
  } else {
    gpio_put(common_pin, 1);
  }
}

void SevenSegmentDisplay::print_bits(uint8_t data) {

  if (common_anode) {
    // To set to on, set the pin to gnd
    bool a_bit = (data & 0b00000001);
    gpio_put(a_pin, !a_bit);

    bool b_bit = (data & 0b00000010) >> 1;
    gpio_put(b_pin, !b_bit);

    bool c_bit = (data & 0b00000100) >> 2;
    gpio_put(c_pin, !c_bit);

    bool d_bit = (data & 0b00001000) >> 3;
    gpio_put(d_pin, !d_bit);

    bool e_bit = (data & 0b00010000) >> 4;
    gpio_put(e_pin, !e_bit);

    bool f_bit = (data & 0b00100000) >> 5;
    gpio_put(f_pin, !f_bit);

    bool g_bit = (data & 0b01000000) >> 6;
    gpio_put(g_pin, !g_bit);

    bool dot_bit = (data & 0b10000000) >> 7;
    gpio_put(dot_pin, !dot_bit);
  } else {
    // To set to on, set the pin to 3.3v
    bool a_bit = (data & 0b00000001);
    gpio_put(a_pin, a_bit);

    bool b_bit = (data & 0b00000010) >> 1;
    gpio_put(b_pin, b_bit);

    bool c_bit = (data & 0b00000100) >> 2;
    gpio_put(c_pin, c_bit);

    bool d_bit = (data & 0b00001000) >> 3;
    gpio_put(d_pin, d_bit);

    bool e_bit = (data & 0b00010000) >> 4;
    gpio_put(e_pin, e_bit);

    bool f_bit = (data & 0b00100000) >> 5;
    gpio_put(f_pin, f_bit);

    bool g_bit = (data & 0b01000000) >> 6;
    gpio_put(g_pin, g_bit);

    bool dot_bit = (data & 0b10000000) >> 7;
    gpio_put(dot_pin, dot_bit);
  }
}

void SevenSegmentDisplay::safe_print_bits(uint8_t data) {
	print_bits(0);
	select();
	print_bits(data);
}

void SevenSegmentDisplay::print_digit(uint8_t digit) {
  switch (digit) {
  case 0:
    print_bits(0b00111111);
    break;
  case 1:
    print_bits(0b00000110);
    break;
  case 2:
    print_bits(0b01011011);
    break;
  case 3:
    print_bits(0b01001111);
    break;
  case 4:
    print_bits(0b01100110);
    break;
  case 5:
    print_bits(0b01101101);
    break;
  case 6:
    print_bits(0b01111101);
    break;
  case 7:
    print_bits(0b00000111);
    break;
  case 8:
    print_bits(0b01111111);
    break;
  case 9:
    print_bits(0b01101111);
    break;
  default:
    break;
  }
}

void SevenSegmentDisplay::safe_print_digit(uint8_t digit) {
  switch (digit) {
  case 0:
    safe_print_bits(0b00111111);
    break;
  case 1:
    safe_print_bits(0b00000110);
    break;
  case 2:
    safe_print_bits(0b01011011);
    break;
  case 3:
    safe_print_bits(0b01001111);
    break;
  case 4:
    safe_print_bits(0b01100110);
    break;
  case 5:
    safe_print_bits(0b01101101);
    break;
  case 6:
    safe_print_bits(0b01111101);
    break;
  case 7:
    safe_print_bits(0b00000111);
    break;
  case 8:
    safe_print_bits(0b01111111);
    break;
  case 9:
    safe_print_bits(0b01101111);
    break;
  default:
    break;
  }
}

void SevenSegmentDisplay::print_hex_digit(uint8_t digit) {

  if (digit < 0xA) {
    print_digit(digit);
  }

  switch (digit) {
  case 0xA:
    print_bits(0b01110111);
    break;
  case 0xB:
    print_bits(0b01111100);
    break;
  case 0xC:
    print_bits(0b00111001);
    break;
  case 0xD:
    print_bits(0b01011110);
    break;
  case 0xE:
    print_bits(0b01111001);
    break;
  case 0xF:
    print_bits(0b01110001);
    break;
  default:
    break;
  }
}

void SevenSegmentDisplay::safe_print_hex_digit(uint8_t digit) {

  if (digit < 0xA) {
    safe_print_digit(digit);
  }

  switch (digit) {
  case 0xA:
    safe_print_bits(0b01110111);
    break;
  case 0xB:
    safe_print_bits(0b01111100);
    break;
  case 0xC:
    safe_print_bits(0b00111001);
    break;
  case 0xD:
    safe_print_bits(0b01011110);
    break;
  case 0xE:
    safe_print_bits(0b01111001);
    break;
  case 0xF:
    safe_print_bits(0b01110001);
    break;
  default:
    break;
  }
}

void SevenSegmentDisplay::print_minus() { print_bits(0b01000000); }

void SevenSegmentDisplay::set_dot(bool state) {
  if (common_anode) {
    gpio_put(dot_pin, !state);
  } else {
    gpio_put(dot_pin, state);
  }
}
