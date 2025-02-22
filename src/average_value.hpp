#include <cstdint>
#include <cmath>
#include <algorithm>

class AverageValue {
	public:

		void add_measurement(uint16_t new_value) {
			measurements[next_measurement_index] = new_value;

			next_measurement_index = (next_measurement_index + 1) % NUM_MEASUREMENTS;

			above = 0;
			max = 0;
			min = 65535;

			// Voodo magic for rounding
			uint32_t sum = NUM_MEASUREMENTS / 2;

			for (uint8_t i = 0; i < NUM_MEASUREMENTS; i++) {
				sum += measurements[i];

				if (measurements[i] > value) {
					above += 1;
				}

				if (min > measurements[i]) {
					min = measurements[i];
				}

				if (max < measurements[i]) {
					max = measurements[i];
				}
			}

			average = sum / NUM_MEASUREMENTS;

			if (above >= (NUM_MEASUREMENTS * 7 / 8) || above <= (NUM_MEASUREMENTS * 1 / 8)) {
				value = average;
			}
		};

		uint32_t get_value_as_rod_position() {
			return 4000000 - ((std::clamp(value, (uint16_t)10, (uint16_t)4095) - 10) * (4000000 / ((1 << 12) - 11)));
		}

		static const uint8_t NUM_MEASUREMENTS = 32;

		uint16_t min = 0;
		uint16_t max = 0;
		uint8_t above = 0;
		uint16_t average = 0;
		uint16_t value = 0;
		uint16_t measurements[NUM_MEASUREMENTS] = {};
		uint8_t next_measurement_index = 0;
		uint16_t value_delta = 1 << 1;
};
