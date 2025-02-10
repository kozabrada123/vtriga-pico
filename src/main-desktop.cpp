#include "reactor.hpp"
#include <chrono>
#include <cmath>
#include <ctime>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <fstream>

int main() {
  Reactor *reactor = new Reactor();
  reactor->get_safety_control_rod()->set_current_position(0);
  reactor->get_safety_control_rod()->set_target_position(0);
  reactor->get_regulating_control_rod()->set_current_position(24e5);
  reactor->get_regulating_control_rod()->set_target_position(24e5);
  reactor->get_compensating_control_rod()->set_current_position(0);

  while (true) {

    auto current_clock = std::chrono::system_clock::now();

    reactor->tick();

	 // Stop condition for data
    /*if (reactor->get_steps_elapsed() > 10000 * 600) {
		 return 0;
	 }*/

    if (reactor->get_steps_elapsed() % 500 == 0) {

	   /*std::ofstream output_file;

		output_file.open("power.txt", std::ios_base::app);

		output_file << std::format("{:.2f} {:.2f}\n", reactor->get_time_elapsed_seconds(), reactor->calculate_power_watts());

		output_file.close();*/

      printf("\033c");
      printf("\n");

      if (reactor->get_in_scram()) {
        printf("\033[1;34;31m  !! IN SCRAM !!\033[0m\n");
      }

      printf("\033[1;34;34m  %.0f seconds (%.0e steps) since reactor "
             "start\033[0m\n",
             reactor->get_time_elapsed_seconds(),
             (double)reactor->get_steps_elapsed());

      printf("\n");

      printf("\033[1;34;33m  Reactor\033[0m\n");
      printf("\033[1;34;33m  Neutrons: %.2e\033[0m\n",
             reactor->get_neutrons_in_core());

      printf("\033[1;34;33m  Thermal power: %.0f W, target %u W\033[0m\n",
             reactor->calculate_power_watts(),
             reactor->get_target_thermal_power_watts());

      auto reactivity_pcm = reactor->get_reactivity_pcm();
      auto reactivity_no_units = reactivity_pcm * 1.0e-5;
      auto reactivity_k = 1.0 / (1.0 - reactivity_no_units);

      printf("\033[1;34;33m  Reactivity: %.0f pcm (k = %f)\033[0m\n",
             reactivity_pcm, reactivity_k);

      printf("\n");

      printf("\033[1;34;33m  Rods\033[0m\n");

      printf(
          "\033[1;34;33m  Positions: s %.4f, r %.4f, c %.4f\033[0m\n",
          reactor->get_safety_control_rod()->get_current_position_as_fraction(),
          reactor->get_regulating_control_rod()
              ->get_current_position_as_fraction(),
          reactor->get_compensating_control_rod()
              ->get_current_position_as_fraction());
      printf(
          "\033[1;34;33m  Targets:   s %.4f, r %.4f, c %.4f\033[0m\n",
          reactor->get_safety_control_rod()->get_target_position_as_fraction(),
          reactor->get_regulating_control_rod()
              ->get_target_position_as_fraction(),
          reactor->get_compensating_control_rod()
              ->get_target_position_as_fraction());

      printf("\n");

      printf("\033[1;34;36m  Thermal data\033[0m\n");

      printf("\033[1;34;36m  Fuel  T: %.1f °C\033[0m\n",
             reactor->get_fuel_temperature_celcius());
      printf("\033[1;34;36m  Water T: %.1f °C\033[0m\n",
             reactor->get_water_temperature_celcius());

      if (reactor->get_active_cooling_system_enabled()) {
        printf("\033[1;34;36m  Active cooling: Y\033[0m\n");
      } else {
        printf("\033[1;34;36m  Active cooling: N\033[0m\n");
      }
    }

    auto next_tick = current_clock +
                     std::chrono::nanoseconds(
                         (uint64_t)(reactor->get_time_delta_seconds() * 1e9f));

    std::this_thread::sleep_until(next_tick);
  }

  return 0;
}
