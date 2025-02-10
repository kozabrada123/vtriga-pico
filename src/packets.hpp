#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <stdint.h>
#include "pico/stdlib.h"

/// Communication via UART for inbetween the picos
/// First byte is an opcode (e.g. print power)
/// followed by n bytes of data, depending on the opcode

const uint8_t OPCODE_UPDATE_POWER = 1;

// For update power, the data is 4 bytes (most to least significant),
// representing a 32 bit unsigned integer that is the power in watts

const uint8_t OPCODE_UPDATE_TEMPERATURES = 2;

// For update temperatures, the data is 3 bytes, representing
// in the first two the fuel temperature and in the second one the water temperature in celcius

#endif
