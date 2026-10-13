cmake_minimum_required(VERSION 3.16)
project(TrafficSimulation LANGUAGES CXX)

# Configure modern C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If compiling with GCC / MinGW, prioritize local MSYS2 UCRT64 SFML package
if(MINGW)
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
endif()

find_package(SFML 3 COMPONENTS Graphics QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML 3 not found in system search paths. Fetching SFML 3.0.1 via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
    )
    FetchContent_MakeAvailable(SFML)
else()
    message(STATUS "Found local SFML 3 installation at: ${SFML_DIR}")
endif()

# Define executable target and source files
add_executable(TrafficSimulation
    main.cpp
    src/Lane.cpp
    src/TrafficLight.cpp
    src/Vehicle.cpp
    src/Environment.cpp
)

# Add header include directory
target_include_directories(TrafficSimulation PRIVATE include)

# Link SFML Graphics module (automatically pulls Window and System)
target_link_libraries(TrafficSimulation PRIVATE SFML::Graphics)

# Automatically copy SFML runtime DLLs to output directory on Windows for convenience
if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    add_custom_command(TARGET TrafficSimulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:TrafficSimulation>
            $<TARGET_FILE_DIR:TrafficSimulation>
        COMMAND_EXPAND_LISTS
    )
endif()

