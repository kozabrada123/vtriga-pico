#ifndef LCD_HPP
#define LCD_HPP

#include "constants.hpp"
#include "pico/stdlib.h"

class LcdDisplay {
	public:
		LcdDisplay();

		void pulse_enable();
		void send_to_lcd(uint8_t data);
		void set_register_select(bool data);
		void setup_lcd();

		void clear_display();
		void return_home();

		/// direction = 1 -> left to right
		/// direction = 0 -> right to left
		void set_entry_mode(bool direction, bool shift_cursor);
		void set_data_shift_direction(bool reverse);
	   void set_display_flags(bool display_enabled, bool cusor_enabled, bool cursor_blink_enabled);
	   void set_display_extended_functions(bool high_font_width, bool invert_black_white, bool four_line_mode);
	   void set_display_function(bool enable_8_bit_bus);

		void write_to_lcd(uint8_t data);

	private:
		const uint32_t pin_register_select = MAIN_PIN_LCD_REGISTER_SELECT;
		const uint32_t pin_enable = MAIN_PIN_LCD_ENABLE;

		const uint32_t pin_data_0 = MAIN_PIN_LCD_DATA_0;
		const uint32_t pin_data_1 = MAIN_PIN_LCD_DATA_1;
		const uint32_t pin_data_2 = MAIN_PIN_LCD_DATA_2;
		const uint32_t pin_data_3 = MAIN_PIN_LCD_DATA_3;
		const uint32_t pin_data_4 = MAIN_PIN_LCD_DATA_4;
		const uint32_t pin_data_5 = MAIN_PIN_LCD_DATA_5;
		const uint32_t pin_data_6 = MAIN_PIN_LCD_DATA_6;
		const uint32_t pin_data_7 = MAIN_PIN_LCD_DATA_7;
};

#endif
