#include "constants.hpp"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/structs/pll.h"
#include "intercore.hpp"
#include "lcd.hpp"
#include "packets.hpp"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "reactor.hpp"
#include "seven_segment.hpp"
#include <algorithm>
#include <cstdint>
#include <format>
#include <hardware/regs/io_bank0.h>
#include <hardware/uart.h>
#include <pico/mutex.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdio.h>
#include <string>

static IntercoreMemory intercore_memory = IntercoreMemory();

#ifdef PERIPHERAL_TARGET

// Peripheral / display pico
int main() {

  // Initialize all gpio pins we'll be using
  gpio_init_mask(0b11111001100111111111111);
  gpio_set_dir_masked(0b11111001100111111111111, 0b11111001100111111111111);

  // Enable UART
  gpio_set_function(PERIPHERAL_PIN_UART_TX, UART_FUNCSEL_NUM(uart0, 0));
  gpio_set_function(PERIPHERAL_PIN_UART_RX, UART_FUNCSEL_NUM(uart0, 1));
  uart_init(uart0, 115200);

  bool led_on = true;
  gpio_init(PIN_LED);
  gpio_set_dir(PIN_LED, 1);

  uint32_t power_watts = 0;
  uint8_t water_temperature_celcius = 20;
  uint16_t fuel_temperature_celcius = 20;

  // Create outputs
  auto power_digit_100k =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_5);
  auto power_digit_10k =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_6);
  auto power_digit_1k =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_7);
  auto power_digit_100 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_8);
  auto power_digit_10 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_9);
  auto power_digit_1 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_10);

  auto fuel_t_digit_100 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_0);
  auto fuel_t_digit_10 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_1);
  auto fuel_t_digit_1 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_2);

  auto water_t_digit_10 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_3);
  auto water_t_digit_1 =
      SevenSegmentDisplay(PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_4);

  // One core should receive from UART, another should just print to the
  // displays
  while (1) {

    // Uart code
    if (uart_is_readable(uart0)) {

      led_on = !led_on;
      gpio_put(PIN_LED, led_on);

      auto received = (uint8_t)uart_getc(uart0);

      switch (received) {

      case OPCODE_UPDATE_POWER: {
        // Listen for four more bytes
        power_watts = (uint32_t)uart_getc(uart0);
        power_watts = power_watts << 24;

        uint32_t temp = (uint32_t)uart_getc(uart0);
        power_watts = power_watts | (temp << 16);

        temp = (uint32_t)uart_getc(uart0);
        power_watts = power_watts | (temp << 8);

        temp = (uint32_t)uart_getc(uart0);
        power_watts = power_watts | temp;

        break;
      }
      case OPCODE_UPDATE_TEMPERATURES: {
        // Listen for three more bytes

        fuel_temperature_celcius = (uint16_t)uart_getc(uart0);
        fuel_temperature_celcius = fuel_temperature_celcius << 8;

        uint16_t temp_2 = (uint16_t)uart_getc(uart0);
        fuel_temperature_celcius = fuel_temperature_celcius | temp_2;

        water_temperature_celcius = (uint8_t)uart_getc(uart0);

        break;
      }

      default:
        // We caught it in the middle or this is just garbage, wait for the next
        // packet
        break;
      }
    }

	 std::string formatted_fuel_t = std::to_string(fuel_temperature_celcius);

    uint8_t length = (uint8_t)formatted_fuel_t.length();
    uint8_t digit = (uint8_t)formatted_fuel_t[length - 1] & 0x0F;

    fuel_t_digit_1.safe_print_digit(digit);
    sleep_us(100);
    fuel_t_digit_1.deselect();

    if (length >= 2) {
      digit = (uint8_t)formatted_fuel_t[length - 2] & 0x0F;

      fuel_t_digit_10.safe_print_digit(digit);
      sleep_us(100);
      fuel_t_digit_10.deselect();
    } else {
      fuel_t_digit_10.print_bits(0);
      fuel_t_digit_10.deselect();
    }

    if (length >= 3) {
      digit = (uint8_t)formatted_fuel_t[length - 3] & 0x0F;

      fuel_t_digit_100.safe_print_digit(digit);
      sleep_us(100);
      fuel_t_digit_100.deselect();
    } else {
      fuel_t_digit_100.print_bits(0);
      fuel_t_digit_100.deselect();
    }

	 std::string formatted_water_t = std::to_string(water_temperature_celcius);

    length = (uint8_t)formatted_water_t.length();
    digit = (uint8_t)formatted_water_t[length - 1] & 0x0F;

    water_t_digit_1.safe_print_digit(digit);
    sleep_us(100);
    water_t_digit_1.deselect();

    if (length >= 2) {
      digit = (uint8_t)formatted_water_t[length - 2] & 0x0F;

      water_t_digit_10.safe_print_digit(digit);
      sleep_us(100);
      water_t_digit_10.deselect();
    } else {
      water_t_digit_10.print_bits(0);
      water_t_digit_10.deselect();
    }

    std::string formatted_power = std::to_string(power_watts);

    length = (uint8_t)formatted_power.length();
    digit = (uint8_t)formatted_power[length - 1] & 0x0F;

    power_digit_1.safe_print_digit(digit);
    sleep_us(100);
    power_digit_1.deselect();

    if (length >= 2) {
      digit = (uint8_t)formatted_power[length - 2] & 0x0F;

      power_digit_10.safe_print_digit(digit);
      sleep_us(100);
      power_digit_10.deselect();
    } else {
      power_digit_10.print_bits(0);
      power_digit_10.deselect();
    }

    if (length >= 3) {
      digit = (uint8_t)formatted_power[length - 3] & 0x0F;

      power_digit_100.safe_print_digit(digit);
      sleep_us(100);
      power_digit_100.deselect();
    } else {
      power_digit_100.print_bits(0);
      power_digit_100.deselect();
    }

    if (length >= 4) {
      digit = (uint8_t)formatted_power[length - 4] & 0x0F;

      power_digit_1k.safe_print_digit(digit);
      sleep_us(100);
      power_digit_1k.deselect();
    } else {
      power_digit_1k.print_bits(0);
      power_digit_1k.deselect();
    }

    if (length >= 5) {
      digit = (uint8_t)formatted_power[length - 5] & 0x0F;

      power_digit_10k.safe_print_digit(digit);
      sleep_us(100);
      power_digit_10k.deselect();
    } else {
      power_digit_10k.print_bits(0);
      power_digit_10k.deselect();
    }

    if (length >= 6) {
      digit = (uint8_t)formatted_power[length - 6] & 0x0F;

      power_digit_100k.safe_print_digit(digit);
      sleep_us(100);
      power_digit_100k.deselect();
    } else {
      power_digit_100k.print_bits(0);
      power_digit_100k.deselect();
    }
  }
}

#else

/// Sets how bright the cherenkov leds should be.
///
/// The arguments should be between 0 and 65535, where 0 is totally off and
/// 65535 is totally on
void set_cherenkov_on_percentage(uint16_t slice_of_max) {

  // A duty cycle of 100% -> 3.3 V
  //
  // The cherenkov leds open at 2.8 V, 3 V and 3.5 V ??
  //
  // Todo: this can't be right, but what do we do?

  uint slice_num = pwm_gpio_to_slice_num(MAIN_PIN_CHERENKOV_LED);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, slice_of_max);
  pwm_set_enabled(slice_num, true);
}

/// Main for core 2 of the simulator pico
void main_core_2() {

  adc_init();
  adc_gpio_init(MAIN_PIN_SAFETY_ROD_INPUT);
  adc_gpio_init(MAIN_PIN_REGULATING_ROD_INPUT);
  adc_gpio_init(MAIN_PIN_COMPENSATING_ROD_INPUT);

  // Wait for lcd to wake
  sleep_ms(40);

  auto *lcd = new LcdDisplay();
  lcd->setup_lcd();

  double neutrons_in_core = 0;
  int16_t reactivity_pcm = 0;

  uint32_t safety_rod_current_position = 0;
  uint32_t regulating_rod_current_position = 0;
  uint32_t compensating_rod_current_position = 0;

  uint32_t safety_rod_target_position = 0;
  uint32_t regulating_rod_target_position = 0;
  uint32_t compensating_rod_target_position = 0;

  bool in_scram = false;
  bool use_adc = true;

  while (1) {

    auto current_time = get_absolute_time();
    auto next_loop_time = delayed_by_ms(current_time, 5);

    // Check if we need to use ADCs
    // 1 - automatic, since we're using pull up resistors
    use_adc = gpio_get(MAIN_PIN_MANUAL_AUTOMATIC_CONTROL_SWITCH);

    // Use ADCs to evaluate potentiometers
    if (use_adc) {
      adc_select_input(MAIN_PIN_SAFETY_ROD_INPUT - 26);
      uint16_t safety_rod_result = adc_read();

      adc_select_input(MAIN_PIN_REGULATING_ROD_INPUT - 26);
      uint16_t regulating_rod_result = adc_read();

      adc_select_input(MAIN_PIN_COMPENSATING_ROD_INPUT - 26);
      uint16_t compensating_rod_result = adc_read();

      safety_rod_target_position =
          safety_rod_result * ((4000000) / (2 ^ 12 - 1));
      regulating_rod_target_position =
          regulating_rod_result * ((4000000) / (2 ^ 12 - 1));
      compensating_rod_target_position =
          compensating_rod_result * ((4000000) / (2 ^ 12 - 1));
    }

    // Communicate with the other core
    mutex_enter_blocking(&intercore_memory.reactor_data_mutex);

    neutrons_in_core = intercore_memory.neutrons_in_core;
    reactivity_pcm = intercore_memory.reactivity_pcm;

    safety_rod_current_position = intercore_memory.safety_rod_current_position;
    regulating_rod_current_position =
        intercore_memory.regulating_rod_current_position;
    compensating_rod_current_position =
        intercore_memory.compensating_rod_current_position;

    in_scram = intercore_memory.in_scram;

    mutex_exit(&intercore_memory.reactor_data_mutex);

    mutex_enter_blocking(&intercore_memory.rod_target_positions_mutex);
    if (use_adc) {

      intercore_memory.safety_rod_target_position = safety_rod_target_position;
      intercore_memory.regulating_rod_target_position =
          regulating_rod_target_position;
      intercore_memory.compensating_rod_target_position =
          compensating_rod_target_position;
    } else {
      safety_rod_target_position = intercore_memory.safety_rod_target_position;
      regulating_rod_target_position =
          intercore_memory.regulating_rod_target_position;
      compensating_rod_target_position =
          intercore_memory.compensating_rod_target_position;
    }
    mutex_exit(&intercore_memory.rod_target_positions_mutex);

    // Display stuff on the lcd
    // First create the string buffer
    std::string line_0 = std::format("n0 : {:.2g}", neutrons_in_core);
    line_0.resize(20, ' ');

    // Get the target and current positions in the range of 0 - 999
    uint16_t safety_target_0_to_999 =
        safety_rod_target_position / (4000000 / 1000);
    std::clamp(safety_target_0_to_999, (uint16_t)0, (uint16_t)999);

    uint16_t regulating_target_0_to_999 =
        regulating_rod_target_position / (4000000 / 1000);
    std::clamp(regulating_target_0_to_999, (uint16_t)0, (uint16_t)999);

    uint16_t compensating_target_0_to_999 =
        compensating_rod_target_position / (4000000 / 1000);
    std::clamp(compensating_target_0_to_999, (uint16_t)0, (uint16_t)999);

    uint16_t safety_current_0_to_999 =
        safety_rod_current_position / (4000000 / 1000);
    std::clamp(safety_current_0_to_999, (uint16_t)0, (uint16_t)999);

    uint16_t regulating_current_0_to_999 =
        regulating_rod_current_position / (4000000 / 1000);
    std::clamp(regulating_current_0_to_999, (uint16_t)0, (uint16_t)999);

    uint16_t compensating_current_0_to_999 =
        compensating_rod_current_position / (4000000 / 1000);
    std::clamp(compensating_current_0_to_999, (uint16_t)0, (uint16_t)999);

    std::string line_1 =
        std::format("tgt: {:-03d} {:-03d} {:-03d}", safety_target_0_to_999,
                    regulating_target_0_to_999, compensating_target_0_to_999);
    line_0.resize(20, ' ');

    std::string line_2 =
        std::format("pos: {:-03d} {:-03d} {:-03d}", safety_current_0_to_999,
                    regulating_current_0_to_999, compensating_current_0_to_999);
    line_0.resize(20, ' ');

    std::string line_3 = std::format("rho: {:+d}", reactivity_pcm);
    line_0.resize(20, ' ');

    // Write to the lcd
    lcd->clear_display();

    for (int i = 0; i < 20; i++) {
      lcd->write_to_lcd(line_0[i]);
    }
    for (int i = 0; i < 20; i++) {
      lcd->write_to_lcd(line_1[i]);
    }
    for (int i = 0; i < 20; i++) {
      lcd->write_to_lcd(line_2[i]);
    }
    for (int i = 0; i < 20; i++) {
      lcd->write_to_lcd(line_3[i]);
    }

    sleep_until(next_loop_time);
  }
}

// Main / simulator pico, main core
// This handles running the simulation and communcates with the second pico and
// second core
int main() {

  // Initialize all gpio pins we'll be using
  gpio_init_mask(0b11111001100111111111000);
  gpio_set_dir_masked(0b11111001100111111111000, 0b00001001100111111111000);

  // Enable pull downs and pull ups on our input switches and button
  gpio_pull_up(MAIN_PIN_ACTIVE_COOLING_SWITCH);
  gpio_pull_up(MAIN_PIN_MANUAL_AUTOMATIC_CONTROL_SWITCH);
  gpio_pull_up(MAIN_PIN_ENABLE_SCRAMS_SWITCH);
  gpio_pull_up(MAIN_PIN_SCRAM_BUTTON);

  // Initialize intercore mutexes
  mutex_init(&intercore_memory.reactor_data_mutex);
  mutex_init(&intercore_memory.rod_target_positions_mutex);

  // Summon the second core
  multicore_launch_core1(main_core_2);

  // Enable PWM
  gpio_set_function(MAIN_PIN_CHERENKOV_LED, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(MAIN_PIN_CHERENKOV_LED);
  pwm_set_wrap(slice_num, 0xFFFF);

  // Enable UART
  gpio_set_function(MAIN_PIN_UART_TX, UART_FUNCSEL_NUM(uart0, 0));
  gpio_set_function(MAIN_PIN_UART_RX, UART_FUNCSEL_NUM(uart0, 1));
  uart_init(uart0, 115200);

  bool led_on = true;
  gpio_init(PIN_LED);
  gpio_set_dir(PIN_LED, 1);

  bool scram_led_on = false;

  Reactor *reactor = new Reactor();

  while (1) {

    auto current_time = get_absolute_time();

    reactor->tick();

    // note: 1e-4 seconds between each, * 1e6 for micros
    auto micros_between_each_loop = 1e-4 * 1e6;

    auto next_loop_time = delayed_by_us(current_time, micros_between_each_loop);

    // Manage the SCRAM led
    //
    // Pulse 1s / 1s on and off when in scram
    // set to off when not in scram
    if (reactor->get_in_scram()) {
      if (reactor->get_steps_since_scram_started() % 10000 == 0) {
        scram_led_on = !scram_led_on;
      }
    } else {
      scram_led_on = false;
    }

    gpio_put(MAIN_PIN_SCRAM_LED, scram_led_on);

    // Set the cherenkov led
    // linearly set the power with PWM from 0 to SCRAM watts
    double cherenkov_percentage =
        reactor->get_target_thermal_power_watts() / (double)POWER_SCRAM_WATTS;
    set_cherenkov_on_percentage((uint16_t)(cherenkov_percentage * 0xFFFF));

    // Check digital GPIO inputs
    // Note: these are inverted, since we pull them up
    if (gpio_get(MAIN_PIN_SCRAM_BUTTON) == false) {
      reactor->scram();
    }

    reactor->scrams_enabled = !gpio_get(MAIN_PIN_ENABLE_SCRAMS_SWITCH);
    reactor->active_cooling_system_enabled =
        !gpio_get(MAIN_PIN_ACTIVE_COOLING_SWITCH);
    reactor->automatic_control =
        !gpio_get(MAIN_PIN_MANUAL_AUTOMATIC_CONTROL_SWITCH);

    // Send to UART
    if (reactor->get_steps_elapsed() % 100 == 0) {

      if (reactor->get_steps_elapsed() % 10000 == 0) {
        led_on = !led_on;
        gpio_put(PIN_LED, led_on);
      }

      uint32_t thermal_power_watts = (uint32_t)reactor->calculate_power_watts();

      uart_putc(uart0, (char)(OPCODE_UPDATE_POWER));

      uart_putc(uart0, (char)(uint8_t)(thermal_power_watts >> 24));
      uart_putc(uart0, (char)(uint8_t)((thermal_power_watts >> 16) & 0xFF));
      uart_putc(uart0, (char)(uint8_t)((thermal_power_watts >> 8) & 0xFF));
      uart_putc(uart0, (char)(uint8_t)(thermal_power_watts & 0xFF));

      uint16_t fuel_T = (uint16_t)reactor->get_fuel_temperature_celcius();
      uint8_t water_T = (uint8_t)reactor->get_water_temperature_celcius();

      uart_putc(uart0, (char)OPCODE_UPDATE_TEMPERATURES);

      uart_putc(uart0, (char)(fuel_T >> 8));
      uart_putc(uart0, (char)((fuel_T) & 0xFF));
      uart_putc(uart0, (char)(fuel_T));
    }

    // Communicate with the other core
    mutex_enter_blocking(&intercore_memory.reactor_data_mutex);

    intercore_memory.neutrons_in_core = reactor->get_neutrons_in_core();
    intercore_memory.reactivity_pcm = (uint16_t)(reactor->get_reactivity_pcm());

    intercore_memory.safety_rod_current_position =
        reactor->get_safety_control_rod()->get_current_position();
    intercore_memory.regulating_rod_current_position =
        reactor->get_regulating_control_rod()->get_current_position();
    intercore_memory.compensating_rod_current_position =
        reactor->get_compensating_control_rod()->get_current_position();

    intercore_memory.in_scram = reactor->get_in_scram();

    mutex_exit(&intercore_memory.reactor_data_mutex);

    mutex_enter_blocking(&intercore_memory.rod_target_positions_mutex);
    if (!reactor->automatic_control) {
      reactor->get_safety_control_rod()->set_target_position(
          intercore_memory.safety_rod_target_position);
      reactor->get_regulating_control_rod()->set_target_position(
          intercore_memory.regulating_rod_target_position);
      reactor->get_compensating_control_rod()->set_target_position(
          intercore_memory.compensating_rod_target_position);
    } else {
      intercore_memory.safety_rod_target_position =
          reactor->get_safety_control_rod()->get_target_position();

      intercore_memory.regulating_rod_target_position =
          reactor->get_regulating_control_rod()->get_target_position();
      intercore_memory.compensating_rod_target_position =
          reactor->get_compensating_control_rod()->get_target_position();
    }

    mutex_exit(&intercore_memory.rod_target_positions_mutex);

    // Sleep until next loop
    sleep_until(next_loop_time);
  }
}

#endif
