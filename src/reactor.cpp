#include "reactor.hpp"
#include "constants.hpp"
#include "control_rod.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

Reactor::Reactor() {
	// Note: change back to 4e6 at some point
  safety_control_rod = ControlRod(0);
  safety_control_rod.set_speed_steps_per_second(SAFETY_ROD_SPEED_PER_SECOND);
  safety_control_rod.set_target_position(0);

  regulating_control_rod = ControlRod(24e5);
  regulating_control_rod.set_speed_steps_per_second(
      REGULATING_ROD_SPEED_PER_SECOND);
  regulating_control_rod.set_target_position(24e5);

  compensating_control_rod = ControlRod(0);
  compensating_control_rod.set_speed_steps_per_second(
      COMPENSATING_ROD_SPEED_PER_SECOND);
  regulating_control_rod.set_target_position(0);
}



ControlRod *Reactor::get_safety_control_rod() { return &safety_control_rod; }

ControlRod *Reactor::get_regulating_control_rod() {
  return &regulating_control_rod;
}

ControlRod *Reactor::get_compensating_control_rod() {
  return &compensating_control_rod;
}

/// Same as compensating control rod
ControlRod *Reactor::get_shim_control_rod() {
  return &compensating_control_rod;
}

/// Sets the power the RCS should try to keep the reactor at
void Reactor::set_target_thermal_power_watts(uint32_t target) {
  target_thermal_power_watts = target;
}

/// Gets the power the RCS is trying to keep the reactor at
uint32_t Reactor::get_target_thermal_power_watts() {
  return target_thermal_power_watts;
}

float Reactor::get_time_delta_seconds() { return time_delta_seconds; }

void Reactor::set_time_delta_seconds(float new_time_delta_s) {
  time_delta_seconds = new_time_delta_s;
}

double Reactor::get_time_elapsed_seconds() {
  return (double)steps_elapsed * (double)time_delta_seconds;
}

uint64_t Reactor::get_steps_elapsed() { return steps_elapsed; }

double Reactor::get_fuel_temperature_celcius() {
  return fuel_temperature_celcius;
}

double Reactor::get_water_temperature_celcius() {
  return water_temperature_celcius;
}

double Reactor::get_reactivity_pcm() { return reactivity_pcm; }

double Reactor::get_reactivity_no_units() { return reactivity_pcm * 1e-5; }

double Reactor::get_neutrons_in_core() { return neutrons_in_core; }

bool Reactor::get_in_scram() { return in_scram; }

bool Reactor::get_active_cooling_system_enabled() {
  return active_cooling_system_enabled;
}

void Reactor::set_active_cooling_system_enabled(bool enabled) {
  active_cooling_system_enabled = enabled;
}

uint64_t Reactor::get_steps_since_scram_started() {
  return steps_elapsed - step_scram_started;
}

double Reactor::get_neutron_population_for_group(uint8_t group) {
  switch (group) {
  case 1:
    return neutron_population_group_1;
    break;
  case 2:
    return neutron_population_group_2;
    break;
  case 3:
    return neutron_population_group_3;
    break;
  case 4:
    return neutron_population_group_4;
    break;
  case 5:
    return neutron_population_group_5;
    break;
  case 6:
    return neutron_population_group_6;
    break;
  default:
    return 0.0;
    break;
  }
}

double Reactor::get_delayed_neutron_fraction_for_group(uint8_t group) {
  switch (group) {
  case 1:
    return DELAYED_NEUTRON_FRACTION_GROUP_1;
    break;
  case 2:
    return DELAYED_NEUTRON_FRACTION_GROUP_2;
    break;
  case 3:
    return DELAYED_NEUTRON_FRACTION_GROUP_3;
    break;
  case 4:
    return DELAYED_NEUTRON_FRACTION_GROUP_4;
    break;
  case 5:
    return DELAYED_NEUTRON_FRACTION_GROUP_5;
    break;
  case 6:
    return DELAYED_NEUTRON_FRACTION_GROUP_6;
    break;
  default:
    return 0.0;
    break;
  }
}

double Reactor::get_neutron_decay_time_for_group(uint8_t group) {
  switch (group) {
  case 1:
    return DECAY_TIME_GROUP_1;
    break;
  case 2:
    return DECAY_TIME_GROUP_2;
    break;
  case 3:
    return DECAY_TIME_GROUP_3;
    break;
  case 4:
    return DECAY_TIME_GROUP_4;
    break;
  case 5:
    return DECAY_TIME_GROUP_5;
    break;
  case 6:
    return DECAY_TIME_GROUP_6;
    break;
  default:
    return 0.0;
    break;
  }
}

// Physical steps
/// Calculates the first kinetic point equation, dN(t)/dt
double Reactor::calculate_dN_dt() {
  double neutrons_from_activity = NEUTRON_SOURCE_INTENSITY_NEUTRONS_PER_SECOND;

  double neutrons_from_population =
      DECAY_TIME_GROUP_1 * neutron_population_group_1;
  neutrons_from_population += DECAY_TIME_GROUP_2 * neutron_population_group_2;
  neutrons_from_population += DECAY_TIME_GROUP_3 * neutron_population_group_3;
  neutrons_from_population += DECAY_TIME_GROUP_4 * neutron_population_group_4;
  neutrons_from_population += DECAY_TIME_GROUP_5 * neutron_population_group_5;
  neutrons_from_population += DECAY_TIME_GROUP_6 * neutron_population_group_6;

  // "Delayed neutron fractions are directly used as a sum to calculate the
  // effective delayed neutron fraction."
  double effective_delayed_neutron_fraction =
      DELAYED_NEUTRON_FRACTION_GROUP_1 + DELAYED_NEUTRON_FRACTION_GROUP_2 +
      DELAYED_NEUTRON_FRACTION_GROUP_3 + DELAYED_NEUTRON_FRACTION_GROUP_4 +
      DELAYED_NEUTRON_FRACTION_GROUP_5 + DELAYED_NEUTRON_FRACTION_GROUP_6;

  double reactivity_no_unit = get_reactivity_no_units();

  double balanced_reactivity =
      reactivity_no_unit - effective_delayed_neutron_fraction;

  double neutron_multiplier =
      balanced_reactivity / PROMPT_NEUTRON_LIFETIME_SECONDS;

  double current_neutrons_times_stuff = neutrons_in_core * neutron_multiplier;

  /*printf("dN/dt => Nc = %f; balanced Rho = %f, mult = %f, Nc' = %f \n",
         neutrons_in_core, balanced_reactivity, neutron_multiplier,
         current_neutrons_times_stuff);
  printf("dN/dt => N' = Nc' (%f) + CN (%f) + An (%f) \n",
         current_neutrons_times_stuff, neutrons_from_population,
         neutrons_from_activity);
  printf("dN/dt => N' = %f /dt \n", current_neutrons_times_stuff +
                                        neutrons_from_population +
                                        neutrons_from_activity);
  printf("dN => %f / %f s = %f \n",
         current_neutrons_times_stuff + neutrons_from_population +
             neutrons_from_activity,
         time_delta_seconds,
         (current_neutrons_times_stuff + neutrons_from_population +
          neutrons_from_activity) *
             time_delta_seconds);*/

  return current_neutrons_times_stuff + neutrons_from_population +
         neutrons_from_activity;
}

/// Calculates the second kinetic point equation, dCi(t)/dt, which represents
/// the neutron populations of individual groups
double Reactor::calculate_dCi_dt(uint8_t i) {

  double delayed_neutron_fraction = get_delayed_neutron_fraction_for_group(i);

  double a = delayed_neutron_fraction / PROMPT_NEUTRON_LIFETIME_SECONDS;

  double decay_time = get_neutron_decay_time_for_group(i);
  double neutron_population = get_neutron_population_for_group(i);

  double neutrons = a * neutrons_in_core - decay_time * neutron_population;

  /*printf("dCi/dt = a: %f, nic: %f, dec: %f, np: %f\n", a, neutrons_in_core,
         decay_time, neutron_population);
  printf("dCi/dt = a: %f, b: %f\n", a * neutrons_in_core,
         decay_time * neutron_population);*/

  return neutrons;
}

/// Calculates the temperature dependent fuel capacity, marked as Cp(t)
double Reactor::calculate_temperature_dependent_fuel_capacity_J_per_kgK(
    double fuel_temperature_celcius) {
  // Their paper does fuel temperature as kelvin - 273 K
  // so literally
  // C + 273.15 K to get kelvin and then - 273 K
  double fuel_temperature_sorta_kelvin = fuel_temperature_celcius + 0.15;

  double result = 333     // J/kgK
                  + 0.678 // J/kgK^2
                        * fuel_temperature_sorta_kelvin;
  return result;
}

/// Calculates the temperature dependent fuel capacity, marked as Cp(t),
/// normalized to the mass of the fuel
double Reactor::calculate_temperature_dependent_fuel_capacity_J_per_K(
    double fuel_temperature_celcius) {

  double J_per_kg_K = calculate_temperature_dependent_fuel_capacity_J_per_kgK(
      fuel_temperature_celcius);

  // In RSS/src/Simulator.cpp, L513, they multiply with 0.858??

  double result_J_per_K = J_per_kg_K * FUEL_MASS_KG;
  return result_J_per_K;
}

/// Calculates the change to fuel temperature in one time step, in degrees
/// celcius
double Reactor::calculate_fuel_temperature_change_celcius() {

  double Cp_J_per_K = calculate_temperature_dependent_fuel_capacity_J_per_K(
      get_fuel_temperature_celcius());

  double thermal_power_generated_in_timestep_J =
      (calculate_power_joules_per_second() * time_delta_seconds);

  double power_exchanged_J = (calculate_power_exchanged_joule_per_second(
                                  get_fuel_temperature_celcius()) *
                              time_delta_seconds);
  double difference_kelvin =
      (thermal_power_generated_in_timestep_J - power_exchanged_J) / Cp_J_per_K;

  // printf("Fuel Generated: %.3e J\n", thermal_power_generated_in_timestep_J);
  // printf("Fuel Lost: %.3e J\n", power_exchanged_J);
  // printf("Fuel Delta T: %.3e C\n", difference_kelvin);

  // A change of K is equal to a change of C
  return difference_kelvin;
}

/// Calculates the heat that escapes the water tank by convection to air, Q air
///
/// See figure 10 in
/// https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
///
/// Always assumes the air is at 20 C, we ain't simulating air thermodynamics
double Reactor::calculate_water_tank_to_air_convection_J_per_second() {
  auto air_temperature_celcius = 20;

  // If the air is hotter than the water, no convection will occur
  if (air_temperature_celcius > water_temperature_celcius) {
    return 0.0;
  }

  double temperature_delta_K =
      water_temperature_celcius -
      air_temperature_celcius; // They do it the other way around, it doesn't
                               // matter since we raise it to a power of 4

  double temperature_delta_to_the_3_4_K =
      std::cbrt(std::pow(temperature_delta_K, 4)); // to the power of 4/3

  return 13.6 * temperature_delta_to_the_3_4_K;
}

/// Calculates the heat that is exchanged between the water in the reactor tank
/// and the concrete reactor wall, Q concrete
///
/// See figure 11 in
/// https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
///
/// Always assumes the concrete is at 20 C
double Reactor::calculate_water_tank_to_conrete_heat_exchange_J_per_second() {

  double concrete_temperature_celcius = 20.0;

  // They do it the other way around, which I'm pretty sure is wrong if we later
  // use a minus
  //
  // We're calculating how much went from the water to the concrete, so if water
  // > concrete it should be positive If we later did a plus and calculated the
  // change in the water, then we'd do it the other way around here
  double temperature_delta_K =
      water_temperature_celcius - concrete_temperature_celcius;

  return 250.0 * temperature_delta_K;
}

/// Calculates the change to water tank temperature in one time step, in degress
/// celcius
///
/// Called after applying fuel_temperature_change_celcius
double Reactor::calculate_water_temperature_change_celcius() {

  double thermal_power_generated_in_timestep_J =
      (calculate_power_joules_per_second() * time_delta_seconds);

  double convection_to_air_J =
      calculate_water_tank_to_air_convection_J_per_second() *
      time_delta_seconds;

  double transfer_to_concrete_J =
      calculate_water_tank_to_conrete_heat_exchange_J_per_second() *
      time_delta_seconds;

  double cooling_from_active_cooling_system_J = 0;

  if (active_cooling_system_enabled) {
    cooling_from_active_cooling_system_J =
        WATER_ACTIVE_COOLING_POWER_WATTS * time_delta_seconds;
  }

  double resultant_J = thermal_power_generated_in_timestep_J -
                       convection_to_air_J - transfer_to_concrete_J -
                       cooling_from_active_cooling_system_J;

  double temperature_change_K = resultant_J / WATER_HEAT_CAPACITY_J_PER_K;

  // printf("Water Generated: %.3e J\n", thermal_power_generated_in_timestep_J);
  // printf("Water Convection to air: %.3e J\n", convection_to_air_J);
  // printf("Water Transfer to concrete: %.3e J\n", transfer_to_concrete_J);
  // printf("Water Active cooling system: %.3e J\n",
  // cooling_from_active_cooling_system_J); printf("Water Resultant: %.3e J\n",
  // resultant_J); printf("Water Delta T: %.3f C\n", temperature_change_K);

  return temperature_change_K;
}

/// Calculates the power exchanged between the fuel and the environment based on
/// the temperature.
///
/// (In other words, P_fe_stat). See figs 6 and 7 in
/// https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0070
///
/// This was essentially stolen from RRS/src/simulator.cpp,
/// Simulator::getCoolingFromTemperature (L521)
double Reactor::calculate_power_exchanged_joule_per_second(
    double fuel_temperature_celcius) {
  double temperature_difference_kelvin =
      water_temperature_celcius - fuel_temperature_celcius;

  // printf("T diff = %f C\n", temperature_difference_kelvin);

  double first = TEMPERATURE_FE_STAT_A1 * TEMPERATURE_FE_STAT_A1 -
                 3.0 * TEMPERATURE_FE_STAT_A0 * TEMPERATURE_FE_STAT_A2;
  double second = 2.0 * TEMPERATURE_FE_STAT_A1 * TEMPERATURE_FE_STAT_A1 *
                      TEMPERATURE_FE_STAT_A1 -
                  9.0 * TEMPERATURE_FE_STAT_A0 * TEMPERATURE_FE_STAT_A1 *
                      TEMPERATURE_FE_STAT_A2 +
                  27.0 * TEMPERATURE_FE_STAT_A2 * TEMPERATURE_FE_STAT_A2 *
                      temperature_difference_kelvin;
  double root = std::cbrt(
      (second + std::sqrt(second * second - 4.0 * first * first * first)) /
      2.0);
  double result = -FUEL_ELEMENTS_IN_CORE *
                  (1.0 / (3.0 * TEMPERATURE_FE_STAT_A2)) *
                  (TEMPERATURE_FE_STAT_A1 + root + (first / root));

  return result;
}

/// Calcuates the temperature of the fuel element in stationary conditions,
/// T_fe_stat.
///
/// See fig 6
/// https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0070
double Reactor::calculate_stationary_fuel_temperature() {
  double power_normalized_joule_per_second =
      calculate_normalized_power_joule_per_second();

  return TEMPERATURE_FE_STAT_A0 * power_normalized_joule_per_second +
         TEMPERATURE_FE_STAT_A1 * power_normalized_joule_per_second *
             power_normalized_joule_per_second +
         TEMPERATURE_FE_STAT_A2 * power_normalized_joule_per_second *
             power_normalized_joule_per_second *
             power_normalized_joule_per_second +
         water_temperature_celcius;
}

/// Calculates the power produced by each element, P_el
double Reactor::calculate_normalized_power_joule_per_second() {
  return calculate_power_joules_per_second() / FUEL_ELEMENTS_IN_CORE;
}

/// Calculates the reactivity of the reactor
double Reactor::calculate_reactivity_pcm() {

  double control_rod_worths_pcm =
      safety_control_rod.calculate_worth_pcm() +
      regulating_control_rod.calculate_worth_pcm() +
      compensating_control_rod.calculate_worth_pcm();

  // See RRS/src/Simulator.cpp:867 and RRS/src/Simulator.cpp:774
  // However they are doing some goofy things
  double cold_core_reactivity_pcm =
      EXCESS_REACTIVITY_PCM - control_rod_worths_pcm;

  double fuel_t_feedback_pcm = calculate_fuel_temperature_feedback_pcm();

  // Note: we don't model xenon poisoning
  double reactivity_pcm = cold_core_reactivity_pcm - fuel_t_feedback_pcm;

  // printf("Rods: -%f pcm\n", control_rod_worths_pcm);
  // printf("Cold core: %f pcm\n", cold_core_reactivity_pcm);
  // printf("Fuel feedback: %f pcm\n", fuel_t_feedback_pcm);

  // return std::clamp(reactivity_pcm, 0.0, (double) EXCESS_REACTIVITY_PCM); We
  // can have negative reactivity, right=
  return reactivity_pcm;
}

/// Calculates the reactor power from the number of neutrons and constants
double Reactor::calculate_power_MeV_per_second() {
  // Stolen from
  // <https://github.com/ijs-f8/Research-Reactor-Simulator/blob/dee250af1809909bb759b4381595a5a489fe5690/include/Simulator.h#L67C19-L67C31>
  double macroscopic_cross_section_for_fission_1_per_meter = 0.56;

  return neutrons_in_core * macroscopic_cross_section_for_fission_1_per_meter *
         (double)NEUTRON_VELOCITY_METERS_PER_SECOND *
         NEUTRON_FISSION_ENERGY_RELEASED_MEV;
}

double Reactor::calculate_power_watts() {

  double in_MeV_per_second = calculate_power_MeV_per_second();

  double MeV_per_second_to_watt = 1.6022e-13;

  return in_MeV_per_second * MeV_per_second_to_watt;
}

double Reactor::calculate_power_joules_per_second() {
  // A joule per second is a watt
  return calculate_power_watts();
}

/// Calculates the reactor flux
double Reactor::calculate_flux() {

  double core_volume_cubic_centimeters =
      CORE_VOLUME_LITERS * 10.0 * 10.0 * 10.0;

  double neutron_velocity_cm_per_second =
      NEUTRON_VELOCITY_METERS_PER_SECOND * 100.0;

  return neutrons_in_core *
         (neutron_velocity_cm_per_second / core_volume_cubic_centimeters);
}

/// Calculates the pcm feedback from the fuel temperature (based on the fuel
/// temperature feedback coefficients)
double Reactor::calculate_fuel_temperature_feedback_pcm() {

  // If cold, there is no feedback
  if (fuel_temperature_celcius <= 0.0) {
    return 0.0;
  }

  // Between 0 and 240, linerally interpolate
  if (fuel_temperature_celcius <= 240.0) {

    double fraction_to_240_celcius = fuel_temperature_celcius / 240.0;

    double coefficient_difference_pcm_per_c =
        FUEL_T_FEEDBACK_COEFFICIENT_240_C_PCM_PER_C -
        FUEL_T_FEEDBACK_COEFFICIENT_0_C_PCM_PER_C;

    return fuel_temperature_celcius *
           (FUEL_T_FEEDBACK_COEFFICIENT_0_C_PCM_PER_C +
            coefficient_difference_pcm_per_c * fraction_to_240_celcius);
  }

  // Above 240, calculate with peak
  double how_far_above_240_celcius = fuel_temperature_celcius - 240.0;

  double coefficient_at_temperature =
      FUEL_T_FEEDBACK_COEFFICIENT_240_C_PCM_PER_C +
      FUEL_T_FEEDBACK_COEFFICIENT_SLOPE_AFTER_PEAK_PCM_PER_C_SQUARED *
          how_far_above_240_celcius;

  return fuel_temperature_celcius * coefficient_at_temperature;
}

void Reactor::tick() {

  // https://www.sciencedirect.com/science/article/pii/S0306454920303285
  //
  // IJS Triga Reserach reactor simulator developer's manual:
  //
  // We won't calculate pulse mode
  // 1. Re-calculate the fuel temperature based on power from the previous
  fuel_temperature_celcius += calculate_fuel_temperature_change_celcius();

  // 2. Move control rods

  // 2.1 to their target positions
  safety_control_rod.move_towards_target(time_delta_seconds);
  regulating_control_rod.move_towards_target(time_delta_seconds);
  compensating_control_rod.move_towards_target(time_delta_seconds);

  // 2.2 to balance target power
  if (!in_scram && automatic_control) {
    balance_control_rods();
  }

  // 3. Recalculate the reactivity
  reactivity_pcm = calculate_reactivity_pcm();

  // 4. Numerically evaluate the point kinetic equations

  // Uhmmm yes it's called numerical evaluation, didn't you know?
  neutrons_in_core += calculate_dN_dt() * time_delta_seconds;

  neutron_population_group_1 += calculate_dCi_dt(1) * time_delta_seconds;
  neutron_population_group_2 += calculate_dCi_dt(2) * time_delta_seconds;
  neutron_population_group_3 += calculate_dCi_dt(3) * time_delta_seconds;
  neutron_population_group_4 += calculate_dCi_dt(4) * time_delta_seconds;
  neutron_population_group_5 += calculate_dCi_dt(5) * time_delta_seconds;
  neutron_population_group_6 += calculate_dCi_dt(6) * time_delta_seconds;

  // 5. Propagate the temperature of the water in the fuel tank
  water_temperature_celcius += calculate_water_temperature_change_celcius();

  if (water_temperature_celcius < 20.0) {
    water_temperature_celcius = 20.0;
  }

  // 6. Check operational limits and start SCRAM

  // If we're in a scram, stop after a while
  if (in_scram) {

    // Allow a quick restart by toggling the enable SCRAMs switch

    uint32_t power_watts = calculate_power_watts();

    if (power_watts <= 1 || !scrams_enabled) {
      in_scram = false;
      step_scram_started = 0;

      if (automatic_control) {
        get_safety_control_rod()->set_target_position(0.0);
        get_compensating_control_rod()->set_target_position(0.0);
      }
    }
  }

  if (scrams_enabled) {

    if (calculate_power_watts() >= (double)POWER_SCRAM_WATTS) {
      scram();
    }

    if (water_temperature_celcius >= (double)WATER_TEMPERATURE_SCRAM_CELCIUS) {
      scram();
    }

    if (fuel_temperature_celcius >= (double)FUEL_TEMPERATURE_SCRAM_CELCIUS) {
      scram();
    }
  }

  steps_elapsed += 1;
}

// Reactor control system
/// Moves the control rods to try to reach the target power
void Reactor::balance_control_rods() {

  // Limits of the RCS when balancing rods
  uint32_t max_position = 4e6;
  uint32_t min_position = 24e5; // 60 %

  int16_t step = 4000; // 10 mm at a time 1e-3 out of 1.0

  // Remove the two other rods, if they aren't already
  if (safety_control_rod.get_target_position() != 0) {
    safety_control_rod.set_target_position(0);
  }

  if (compensating_control_rod.get_target_position() != 0) {
    compensating_control_rod.set_target_position(0);
  }

  bool safety_in_position = safety_control_rod.get_current_position() == 0;
  bool compensating_in_position =
      compensating_control_rod.get_current_position() == 0;

  if (!safety_in_position || !compensating_in_position) {
    return;
  }

  // Compensate the regulating rod
  uint32_t thermal_power_watts = (uint32_t)calculate_power_watts();
  uint32_t thermal_power_delta =
      target_thermal_power_watts - thermal_power_watts;

  auto rod = get_regulating_control_rod();

  int16_t delta_position = 0;

  if (thermal_power_watts > target_thermal_power_watts) {
    delta_position = step;
  }
  if (thermal_power_watts < target_thermal_power_watts) {
    delta_position = -step;
  }

  rod->set_target_position(
      std::clamp((uint32_t)(rod->get_current_position() + delta_position),
                 min_position, max_position));
}

/// Initiates an emergency shutdown that lasts 6 seconds
void Reactor::scram() {
  in_scram = true;
  step_scram_started = steps_elapsed;

  // Slam all da control rods in
  safety_control_rod.set_current_position(4e6);
  safety_control_rod.set_target_position(4e6);

  regulating_control_rod.set_current_position(4e6);
  regulating_control_rod.set_target_position(4e6);

  compensating_control_rod.set_current_position(4e6);
  compensating_control_rod.set_target_position(4e6);
}
