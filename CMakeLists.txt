cmake_minimum_required(VERSION 3.5)
project(chip8emu)

set(CMAKE_C_STANDARD 11)
find_package(Curses)
set(LIBS ${LIBS} ${CURSES_LIBRARIES} pthread)

set(SOURCE_FILES main.c chip8.h chip8.c)
add_executable(chip8emu ${SOURCE_FILES})
target_link_libraries(chip8emu ${LIBS})