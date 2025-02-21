#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <stdint.h>

// Constants

// == Hardware constants ==
// Also see on tinkercad

const uint32_t PIN_LED = 25;

const uint32_t PIN_ADC_2 = 28;
const uint32_t PIN_ADC_1 = 27;
const uint32_t PIN_ADC_0 = 26;

// -- Peripheral / display pico
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_A = 21;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_B = 22;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_C = 14;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_D = 15;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_E = 18;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_F = 20;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_G = 19;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_DOT = 11;

const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_0 = 10;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_1 = 9;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_2 = 8;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_3 = 7;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_4 = 6;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_5 = 5;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_6 = 4;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_7 = 3;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_8 = 2;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_9 = 1;
const uint32_t PERIPHERAL_PIN_SEVEN_SEGMENT_CATHODE_10 = 0;

const uint32_t PERIPHERAL_PIN_UART_RX = 13;
const uint32_t PERIPHERAL_PIN_UART_TX = 12;

// -- Main / simulator pico
const uint32_t MAIN_PIN_COMPENSATING_ROD_INPUT = PIN_ADC_2;
const uint32_t MAIN_PIN_REGULATING_ROD_INPUT = PIN_ADC_1;
const uint32_t MAIN_PIN_SAFETY_ROD_INPUT = PIN_ADC_0;

const uint32_t MAIN_PIN_MANUAL_AUTOMATIC_CONTROL_SWITCH = 22;
const uint32_t MAIN_PIN_ACTIVE_COOLING_SWITCH = 21;
const uint32_t MAIN_PIN_ENABLE_SCRAMS_SWITCH = 20;

const uint32_t MAIN_PIN_SCRAM_BUTTON = 19;

const uint32_t MAIN_PIN_CHERENKOV_LED = 18;
const uint32_t MAIN_PIN_SCRAM_LED = 15;

const uint32_t MAIN_PIN_LCD_REGISTER_SELECT = 14;
const uint32_t MAIN_PIN_LCD_ENABLE = 11;

const uint32_t MAIN_PIN_LCD_DATA_0 = 10;
const uint32_t MAIN_PIN_LCD_DATA_1 = 9;
const uint32_t MAIN_PIN_LCD_DATA_2 = 8;
const uint32_t MAIN_PIN_LCD_DATA_3 = 7;
const uint32_t MAIN_PIN_LCD_DATA_4 = 6;
const uint32_t MAIN_PIN_LCD_DATA_5 = 5;
const uint32_t MAIN_PIN_LCD_DATA_6 = 4;
const uint32_t MAIN_PIN_LCD_DATA_7 = 3;

const uint32_t MAIN_PIN_UART_RX = 17;
const uint32_t MAIN_PIN_UART_TX = 16;

// == Simulation constants ==

// Stolen from RRS/include/Settings.h
// Which is in turn stolen from Pulstri.pdf
const auto PROMPT_NEUTRON_LIFETIME_SECONDS = 39e-6;

/// How big our core is, trust the guys at IJS
const auto CORE_VOLUME_LITERS = 25.43;
const auto WATER_VOLUME_CUBIC_METERS = 20;

/// See figure 9 in https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
const auto WATER_DENSITY_KG_PER_M3 = 998.2;

/// Specific heat capacity of water at 20 C, also from fig 9 in https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
///
/// (cw)
const auto WATER_SPECIFIC_HEAT_CAPACITY_J_PER_KG_K = 4185.0;

/// Heat capacity of the cooling water, Cw
///
/// Note: the specific heat capacity is always used at 20 C
///
/// See figure 9 in https://www.sciencedirect.com/science/article/pii/S0306454920303285#t0005
const auto WATER_HEAT_CAPACITY_J_PER_K = WATER_VOLUME_CUBIC_METERS * WATER_DENSITY_KG_PER_M3 * (double) WATER_SPECIFIC_HEAT_CAPACITY_J_PER_KG_K;

/// Reactivity if we removed all the control poisons
const auto EXCESS_REACTIVITY_PCM = 3000;
const auto WATER_ACTIVE_COOLING_POWER_WATTS = 240000;

/// How does this relate to excess reactivity? Does putting just one rod into
/// fully into the core completely kill reactivity?
const auto CONTROL_ROD_WORTH_PCM = 4000;

// Scram conditions
const auto SCRAM_PERIOD_SECONDS = 6.0;
const auto POWER_SCRAM_WATTS = 250000;
const auto FUEL_TEMPERATURE_SCRAM_CELCIUS = 300;
const auto WATER_TEMPERATURE_SCRAM_CELCIUS = 80;

const auto NEUTRON_SOURCE_INTENSITY_NEUTRONS_PER_SECOND = 1e5;

// See table 1 again
const auto DELAYED_NEUTRON_FRACTION_GROUP_1 = 0.00023097;
const auto DELAYED_NEUTRON_FRACTION_GROUP_2 = 0.00153278;
const auto DELAYED_NEUTRON_FRACTION_GROUP_3 = 0.00137180;
const auto DELAYED_NEUTRON_FRACTION_GROUP_4 = 0.00276451;
const auto DELAYED_NEUTRON_FRACTION_GROUP_5 = 0.00080489;
const auto DELAYED_NEUTRON_FRACTION_GROUP_6 = 0.00029396;

const auto DECAY_TIME_GROUP_1 = 0.01240;
const auto DECAY_TIME_GROUP_2 = 0.03041;
const auto DECAY_TIME_GROUP_3 = 0.11150;
const auto DECAY_TIME_GROUP_4 = 0.30100;
const auto DECAY_TIME_GROUP_5 = 0.13800;
const auto DECAY_TIME_GROUP_6 = 3.01000;

const auto FUEL_T_FEEDBACK_COEFFICIENT_0_C_PCM_PER_C = 6.0;
const auto FUEL_T_FEEDBACK_COEFFICIENT_240_C_PCM_PER_C = 9.0;
const auto FUEL_T_FEEDBACK_COEFFICIENT_SLOPE_AFTER_PEAK_PCM_PER_C_SQUARED =
    -0.004;

// See section 2.1
const auto NEUTRON_VELOCITY_METERS_PER_SECOND = 2200;
const auto NEUTRON_FISSION_ENERGY_RELEASED_MEV = 200;

// Taken from RRS/include/Simulator.h
const auto FUEL_ELEMENTS_IN_CORE = 59;

// Taken from RSS/src/Simulator.cpp, line 504, getFuelCp
const auto FUEL_ELEMENT_OUTER_RADIUS_CM = 3.556;
const auto FUEL_ELEMENT_INNER_RADIUS_CM = 0.635;
const auto FUEL_ELEMENT_LENGTH_CM = 38.1;
const auto ONE_FUEL_ELEMENT_VOLUME_CM3 =

    ((0.5 * FUEL_ELEMENT_OUTER_RADIUS_CM) *
     (0.5 * FUEL_ELEMENT_OUTER_RADIUS_CM) -
     (0.5 * FUEL_ELEMENT_INNER_RADIUS_CM) *
         (0.5 * FUEL_ELEMENT_INNER_RADIUS_CM)
	 ) *
    FUEL_ELEMENT_LENGTH_CM * 3.1415926535;

/// Taken from the paper https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0110
/// and RRS/include/Simulator.h
///
/// Constants in the paper are 333 J/kgK and 0.678 J/kgK
/// Constants in the code are 2.04 J/kgcm3 and 4.17e-3 J/kgcm3
///
/// Diving those, you get 0.06126 kg/cm3 and 0.06150 kg/cm3 respectively
/// for the fuel density
///
/// Also, https://ric.ijs.si/wp-content/uploads/Description_TRIGA_Reactor.pdf
/// says it's 6.0? Who knows at this point
const auto FUEL_DENSITY_KG_PER_CM3 = 0.0614;

/// Mass of all the fuel elements in the core
const auto FUEL_MASS_KG = FUEL_DENSITY_KG_PER_CM3 * ONE_FUEL_ELEMENT_VOLUME_CM3 * FUEL_ELEMENTS_IN_CORE;

/// taken from tempModelCoeff in RRS/include/Simulator.h, L142,
/// used to calculate Temperature_fe_stat
///
/// this was in turn "// From TRIGLAV documentation"
///
/// Also see figure 6 in https://www.sciencedirect.com/science/article/pii/S0306454920303285#b0070
const auto TEMPERATURE_FE_STAT_A0 = 67.18e-03;
const auto TEMPERATURE_FE_STAT_A1 = -8.381e-06;
const auto TEMPERATURE_FE_STAT_A2 = 0.3843e-09;

/// Taken from RRS
// Their values are 15/1000, 7/1000 and 20/1000
const auto SAFETY_ROD_SPEED_PER_SECOND = 15 * 4000;
const auto REGULATING_ROD_SPEED_PER_SECOND = 7 * 4000;
const auto COMPENSATING_ROD_SPEED_PER_SECOND = 20 * 4000;
#endif
