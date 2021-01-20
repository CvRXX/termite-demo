cmake_minimum_required (VERSION 2.8.11)
project (demo CXX)

set(sources main.cpp)

add_library(hwlib INTERFACE)
include_directories(deps/am2302)
target_include_directories(hwlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/hwlib/library)
target_sources(hwlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/hwlib/library/hwlib.cpp)
add_definitions( -DHWLIB_BAUDRATE=19200)
add_definitions( -DHWLIB_TARGET_termite)
set(CMAKE_CXX_STANDARD 20)


# Where the toolchain is located on your pc.
set(toolchain deps/toolchain) # Your path to kvasir_toolchain

# The compiler that is used:
include(${toolchain}/compilers/arm-none-eabi.cmake)

# The target chip that is compiled for:
include(${toolchain}/targets/arm32/cm3/stm/f1xx/f103/f103.cmake)
target_link_libraries(demo_f103.elf hwlib)



