#ifndef CONTROL_ROD_HPP
#define CONTROL_ROD_HPP

//#include "pico/stdlib.h"
#include <stdint.h>

class ControlRod {
	public:

		ControlRod();

		/// Creates a control rod with a starting position
		ControlRod(uint32_t position);

		/// Gets the current position of the control rod, between 0 and 4_000_000
		///
		/// 4_000_000 is fully inside and 0 is fully outside
		uint32_t get_current_position();

		/// Gets the current position of the control rod, as a double between 0 and 1
		///
		/// 1 is fully inside and 0 is fully outside
		double get_current_position_as_fraction();

		/// Sets the current position of the control rod, between 0 and 4_000_000
		///
		/// 4_000_000 is fully inside and 0 is fully outside
		void set_current_position(uint32_t new_position);

		/// Gets the target position of the control rod, between 0 and 4_000_000
		///
		/// 4_000_000 is fully inside and 0 is fully outside
		uint32_t get_target_position();

		/// Gets the target position of the control rod, as a double between 0 and 1
		///
		/// 1 is fully inside and 0 is fully outside
		double get_target_position_as_fraction();

		/// Sets the target position of the control rod, between 0 and 4_000_000
		///
		/// 4_000_000 is fully inside and 0 is fully outside
		void set_target_position(uint32_t new_position);

		/// Gets the speed of the control rod, in steps per second
		uint32_t get_speed_steps_per_second();

		/// Sets the speed of the control rod, in steps per second
		void set_speed_steps_per_second(uint32_t new_speed);

		/// Slowly moves the rod towards the target
		void move_towards_target(double delta_t_seconds);

		/// Calculates the normalized worth of the control rod at a given position between 0 and 1.
		///
		/// 1 means the rod is contributing its full worth and 0 it is not contributing any of its worth
		double calculate_normalized_worth_at_position(double position);

		/// Calculates the normalized worth of the control rod at its current position.
		///
		/// 1 means the rod is contributing its full worth and 0 it is not contributing any of its worth
		double calculate_normlized_worth();

		/// Calculates the worth of the control rod at a given position between 0 and 1,
		/// in pcm
		///
		/// 1 is fully inside and 0 is fully outside
		double calculate_worth_at_position_pcm(double position);

		/// Calculates the worth of the control rod at its current position, in pcm
		double calculate_worth_pcm();

	protected:

		/// Between 0 and 4000000, 4000000 is fully inside and 0 is fully outside
		uint32_t current_position = 0;

	   /// Between 0 and 4000000, 4000000 is fully inside and 0 is fully outside
		uint32_t target_position = 0;

		// Steps of the total position per second
		// We want 15/1000, so 15 * 4000000/1000 = 15 * 4000
		uint32_t speed_per_second = 15 * 4000;

		// Ignore this for now, TODO: use bezier instead of fully linear rods
		/// See figure 22 in https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
		///
		/// 0 = 0.57 and 1 = 0.68 would follow the calibration of the pulsing rod
		///
		/// 0 = 0.0 and 1 = 1.0 is a linear calibration curve
		//double bezier_parameter_0 = 0.0;
		//double bezier_parameter_1 = 1.0;
};
#endif
