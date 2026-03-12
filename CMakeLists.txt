cmake_minimum_required(VERSION 3.16)
project(Parkhaus_Simulation C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c)

file(GLOB_RECURSE PARKHAUS_SIMULATION_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE PARKHAUS_SIMULATION_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

file(GLOB_RECURSE PARKHAUS_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.c"
)

set(PARKHAUS_LIBRARY_SOURCES ${PARKHAUS_SIMULATION_SOURCES})
list(REMOVE_ITEM PARKHAUS_LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_library(Parkhaus_Simulation_Core
        ${PARKHAUS_LIBRARY_SOURCES}
        ${PARKHAUS_SIMULATION_HEADERS}
)

target_include_directories(Parkhaus_Simulation_Core
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        ${JSONC_INCLUDE_DIRS}
)

target_link_libraries(Parkhaus_Simulation_Core
        PUBLIC
        PkgConfig::JSONC
        m
)

add_executable(Parkhaus_Simulation
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
)

target_link_libraries(Parkhaus_Simulation
        PRIVATE
        Parkhaus_Simulation_Core
)

enable_testing()

add_executable(Parkhaus_Simulation_Tests
        ${PARKHAUS_TEST_SOURCES}
)

target_link_libraries(Parkhaus_Simulation_Tests
        PRIVATE
        Parkhaus_Simulation_Core
)

add_test(NAME Parkhaus_Simulation_Tests COMMAND Parkhaus_Simulation_Tests)