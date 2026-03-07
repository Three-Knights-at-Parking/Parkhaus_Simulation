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

add_executable(Parkhaus_Simulation ${PARKHAUS_SIMULATION_SOURCES} ${PARKHAUS_SIMULATION_HEADERS})

target_include_directories(Parkhaus_Simulation
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        ${JSONC_INCLUDE_DIRS}
)

target_link_libraries(Parkhaus_Simulation
        PRIVATE
        PkgConfig::JSONC
        m
)
