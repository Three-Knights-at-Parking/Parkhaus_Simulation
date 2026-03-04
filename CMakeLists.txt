cmake_minimum_required(VERSION 3.16)

project(Teil1_Parkhaus_Simulation_Plannung C)

set(CMAKE_C_STANDARD 17)

add_executable(Teil1_Parkhaus_Simulation_Plannung
        include/types.h
        include/Car.h
        include/Queue.h
        include/Parkhaus.h
        include/ui/ui_simulation.h
        include/SimulationObject.h
        include/Settings.h
        include/utils/VehicleList.h
        include/io/ConfigFileHandler.h
        include/io/SaveHandler.h
        include/io/GraphHandler.h
        include/utils/RNG.h
        include/Simulation.h
        include/GenericVehicle.h
        include/utils/StatList.h
        include/ui/ui.h
        include/ui/ui_home.h
        include/ui/ui_config.h
        include/ui/ui_storage.h
        src/main.c
        src/Car.c
        src/Queue.c
        src/Parkhaus.c
        src/generic/SimulationObject.c
        src/Settings.c
        src/Simulation.c
        src/generic/GenericVehicle.c
        src/io/ConfigFileHandler.c
        src/io/SaveHandler.c
        src/io/GraphHandler.c
        src/ui/ui_simulation.c
        src/generic/RNG.c
        src/generic/StatList.c
        src/generic/VehicleList.c
        src/ui/ui.c
        src/ui/ui_home.c
        src/ui/ui_config.c
        src/ui/ui_storage.c
)

target_include_directories(Teil1_Parkhaus_Simulation_Plannung
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
