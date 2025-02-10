#include "control_rod.hpp"
#include "constants.hpp"
#include <algorithm>
#include <cmath>
//#include "pico/stdlib.h"

ControlRod::ControlRod() { current_position = 0; }

/// Creates a control rod with a starting position
ControlRod::ControlRod(uint32_t position) {
  this->current_position = std::clamp(position, (uint32_t)0, (uint32_t)4e6);
}

/// Gets the current position of the control rod, between 0 and 4_000_000
uint32_t ControlRod::get_current_position() { return current_position; }

/// Gets the current position of the control rod, as a double between 0 and 1
///
/// 1 is fully inside and 0 is fully outside
double ControlRod::get_current_position_as_fraction() { return (double)current_position / (double)4e6; }

/// Sets the current position of the control rod, between 0 and 4_000_000
void ControlRod::set_current_position(uint32_t new_position) {
  current_position = std::clamp(new_position, (uint32_t)0, (uint32_t)4e6);
}

/// Gets the target position of the control rod, between 0 and 4_000_000
uint32_t ControlRod::get_target_position() { return target_position; }

/// Sets the target position of the control rod, between 0 and 4_000_000
void ControlRod::set_target_position(uint32_t new_position) {
  target_position = std::clamp(new_position, (uint32_t)0, (uint32_t)4e6);
}

/// Gets the current position of the control rod, as a double between 0 and 1
///
/// 1 is fully inside and 0 is fully outside
double ControlRod::get_target_position_as_fraction() { return (double)target_position / (double)4e6; }

/// Gets the speed of the control rod, in a fraction per second
uint32_t ControlRod::get_speed_steps_per_second() { return speed_per_second; }

/// Sets the speed of the control rod, in a fraction per second
void ControlRod::set_speed_steps_per_second(
    uint32_t new_speed_frac_per_second) {
  speed_per_second = new_speed_frac_per_second;
}

/// Slowly moves the rod towards the target, for a given delta_t time
void ControlRod::move_towards_target(double delta_t_seconds) {
  int64_t target_delta_position = (int64_t)target_position - (int64_t)current_position;

  int64_t max_delta_position =
      (uint64_t)((double)delta_t_seconds * (double)speed_per_second);

  int64_t delta_position = std::clamp(target_delta_position,
                                       -max_delta_position, max_delta_position);

  current_position = (uint32_t)((int64_t)current_position + delta_position);
}

/// Calculates the normalized worth of the control rod at a given position
/// between 0 and 1.
///
/// 1 means the rod is contributing its full worth and 0 it is not contributing
/// any of its worth
double ControlRod::calculate_normalized_worth_at_position(double position) {
  // TODO: Maybe use bezier at some point
  // For now, just do it fully linearly
  //
  // For a fully linear rod:
  // position 0.0 => worth 0.0
  // position 0.5 => worth 0.5
  // position 1.0 => worth 1.0
  return (double)position / (double) 4e6;
}

/// Calculates the normalized worth of the control rod at its current position.
///
/// 1 means the rod is contributing its full worth and 0 it is not contributing
/// any of its worth
double ControlRod::calculate_normlized_worth() {
  return calculate_normalized_worth_at_position(current_position);
}

/// Calculates the worth of the control rod at a given position between 0 and 1,
/// in pcm
///
/// 1 is fully inside and 0 is fully outside
double ControlRod::calculate_worth_at_position_pcm(double position) {
  return calculate_normalized_worth_at_position(position) *
         CONTROL_ROD_WORTH_PCM;
}

/// Calculates the worth of the control rod at its current position, in pcm
double ControlRod::calculate_worth_pcm() {
  return calculate_worth_at_position_pcm(current_position);
}
