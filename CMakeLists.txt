cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

file(GLOB vtriga_SRC
	"src/*.hpp"
	"src/main.cpp"
	"src/control_rod.cpp"
	"src/seven_segment.cpp"
	"src/reactor.cpp"
	"src/packet.hpp"
	"src/lcd.cpp"
	"src/lcd.hpp"
	"src/intercore.cpp"
)

add_executable(vtriga
	${vtriga_SRC}
)

#pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(vtriga 1)
pico_add_extra_outputs(vtriga)
target_link_libraries(vtriga pico_stdlib pico_multicore hardware_adc hardware_pwm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_compile_options(vtriga PRIVATE "-O3")
