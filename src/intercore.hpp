#ifndef INTERCORE_HPP
#define INTERCORE_HPP
#include "pico/mutex.h"
#include <stdint.h>

class IntercoreMemory {
public:
  // Main core writes, secondary core reads
  double neutrons_in_core = 0;
  int16_t reactivity_pcm = 0;

  uint32_t safety_rod_current_position = 0;
  uint32_t regulating_rod_current_position = 0;
  uint32_t compensating_rod_current_position = 0;

  bool in_scram = false;

  mutex reactor_data_mutex;

  // Secondary core writes, main core reads if manual control is enabled
  // Main core writes and secondary reads if automatic control is enabled
  uint32_t safety_rod_target_position = 0;
  uint32_t regulating_rod_target_position = 0;
  uint32_t compensating_rod_target_position = 0;

  mutex rod_target_positions_mutex;
};
#endif
