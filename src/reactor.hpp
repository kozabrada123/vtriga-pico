#ifndef REACTOR_HPP
#define REACTOR_HPP

// PC-based JSI research reactor simulator -
// https://www.sciencedirect.com/science/article/pii/S0306454920303285#s0010
#include "control_rod.hpp"
#include <stdint.h>

class Reactor {
public:
  Reactor();

  ControlRod *get_safety_control_rod();
  ControlRod *get_regulating_control_rod();
  ControlRod *get_compensating_control_rod();
  /// Same as compensating
  ControlRod *get_shim_control_rod();

  float get_time_delta_seconds();
  void set_time_delta_seconds(float time_delta_s);

  double get_time_elapsed_seconds();
  uint64_t get_steps_elapsed();

  double get_fuel_temperature_celcius();
  double get_water_temperature_celcius();

  double get_reactivity_pcm();
  double get_reactivity_no_units();

  double get_neutrons_in_core();

  double get_neutron_population_for_group(uint8_t group);
  double get_delayed_neutron_fraction_for_group(uint8_t group);
  double get_neutron_decay_time_for_group(uint8_t group);

  /// Sets the power the RCS should try to keep the reactor at
  void set_target_thermal_power_watts(uint32_t target);
  /// Gets the power the RCS is trying to keep the reactor at
  uint32_t get_target_thermal_power_watts();

  bool get_in_scram();

  bool get_active_cooling_system_enabled();
  void set_active_cooling_system_enabled(bool enabled);

  // Gets how many iterations it's been since the scram started
  uint64_t get_steps_since_scram_started();

  // Physical simulation steps
  /// Calculates the first kinetic point equation, dN(t)/dt
  double calculate_dN_dt();

  /// Calculates the second kinetic point equation, dCi(t)/dt
  double calculate_dCi_dt(uint8_t i);

  /// Calculates the temperature dependent fuel capacity, marked as Cp(t)
  double calculate_temperature_dependent_fuel_capacity_J_per_kgK(
      double temperature_celcius);

  /// Calculates the temperature dependent fuel capacity, marked as Cp(t),
  /// normalized to the mass of the fuel
  double calculate_temperature_dependent_fuel_capacity_J_per_K(
      double temperature_celcius);

  /// Calculates the power exchanged between the fuel and the environment
  /// based on the temperature.
  ///
  /// (In other words, P_fe_stat). See figs 6 and 7 in
  /// https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0070
  ///
  /// This was essentially stolen from RRS/src/simulator.cpp,
  /// Simulator::getCoolingFromTemperature (L521)
  double
  calculate_power_exchanged_joule_per_second(double fuel_temperature_celcius);

  /// Calcuates the temperature of the fuel element in stationary conditions
  ///
  /// See fig 6
  /// https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0070
  double calculate_stationary_fuel_temperature();

  /// Calculates the power produced by each element, P_el
  double calculate_normalized_power_joule_per_second();

  /// Calculates the change to fuel temperature in one time step, in degress
  /// celcius
  double calculate_fuel_temperature_change_celcius();

  /// Calculates the heat that escapes the water tank by convection to air, Q
  /// air
  ///
  /// See figure 10 in
  /// https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
  double calculate_water_tank_to_air_convection_J_per_second();

  /// Calculates the heat that is exchanged between the water in the reactor
  /// tank and the concrete reactor wall, Q concrete
  ///
  /// See figure 11 in
  /// https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
  double calculate_water_tank_to_conrete_heat_exchange_J_per_second();

  /// Calculates the change to water tank temperature in one time step, in
  /// degress celcius
  ///
  /// Called after applying fuel_temperature_change_celcius
  double calculate_water_temperature_change_celcius();

  /// Recalculates the reactivity inside the reactor core
  double calculate_reactivity_pcm();

  /// Calculates the reactor power from the number of neutrons and constants
  double calculate_power_MeV_per_second();
  double calculate_power_watts();
  double calculate_power_joules_per_second();

  /// Calculates the reactor flux
  double calculate_flux();

  /// Calculates the pcm feedback from the fuel temperature (based on the fuel
  /// temperature feedback coefficients)
  double calculate_fuel_temperature_feedback_pcm();

  /// Runs the reactor simulation forward one time_delta_s fraction of time
  void tick();

  // Reactor control system
  /// Moves the control rods to try to reach the target power
  void balance_control_rods();
  /// Initiates an emergency shutdown that lasts 6 seconds
  void scram();

  // Switches on the model
  /// Whether or not to take 200kW out of the cooling loop
  bool active_cooling_system_enabled = true;
  /// Whether or not to automatically balance the rods
  bool automatic_control = true;
  /// Whether or not to check SCRAM conditions
  bool scrams_enabled = true;

protected:
  /// Time for each simulation step, in seconds
  float time_delta_seconds = 1e-4;
  /// How many loops we've calculated
  uint64_t steps_elapsed = 0;

  // Scram data
  /// Is a scram active?
  bool in_scram = false;
  /// The step we started the scram
  uint64_t step_scram_started = 0;

  // Temperatures
  double water_temperature_celcius = 20.0;
  double fuel_temperature_celcius = 20.0;

  // Reactivity and neutrons
  double reactivity_pcm = calculate_reactivity_pcm();
  double neutrons_in_core = 0.0;

  double neutron_population_group_1 = 0.0;
  double neutron_population_group_2 = 0.0;
  double neutron_population_group_3 = 0.0;
  double neutron_population_group_4 = 0.0;
  double neutron_population_group_5 = 0.0;
  double neutron_population_group_6 = 0.0;

  // Control rods
  //
  // See section 2.7 of
  // https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
  ControlRod safety_control_rod;
  ControlRod regulating_control_rod;
  /// Also called shim sometimes
  ControlRod compensating_control_rod;

  // RCS information
  uint32_t target_thermal_power_watts = 20001;
};
#endif
