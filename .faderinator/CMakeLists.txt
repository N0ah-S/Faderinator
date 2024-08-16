cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(YAHAL_OPT_FLAGS -O0)
set(YAHAL_USE_NANO_SPECS 0)

# Set the target board
set(YAHAL_BOARD "rp2040-launchpad")

# Locate the YAHAL library folder
include(YAHAL_import.cmake)

project(faderinator)

include_directories(src/display)
add_executable(faderinator
        src/main.cpp
        src/util/GpioPin.h
        src/util/VirtualArray.h
        src/Fader.h
        src/io/DataPacket.h
        src/io/SerialConnection.cpp
        src/io/SerialConnection.h
        src/io/ConfigurationPacket.h
        src/util/ByteRegion.h
        src/display/GFX/Print.h
        src/display/GFX/Adafruit_GFX.cpp
        src/display/GFX/Adafruit_GFX.h
        src/display/Adafruit_SSD1306.cpp
        src/display/Adafruit_SSD1306.h
        src/util/i2c_rp2040_wire.h
        src/img_test.cpp
        src/util/InputPin.cpp
        src/util/InputPin.h
        src/Application.cpp
        src/Application.h
        src/Fader.cpp
        src/util/GpioPin.cpp
)

# Add YAHAL to target
yahal_add_me(faderinator)

yahal_add_custom_targets(faderinator)
yahal_add_extra_outputs(faderinator)

