# ============================================================================
# CMakeLists.txt "God-Key" for Project AGNI (v2 - Corrected)
# Hard-Locks the complete production solution
# ============================================================================

cmake_minimum_required(VERSION 3.10)
project(AgniProduction CXX)

# Set the C++ standard unambiguously to C++11 for all targets
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Prevents use of GNU extensions like gnu++11

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

# Identify all production C++ source files
set(AGNI_SOURCES
    api_gateway.cpp
    vector_utils.cpp
    scheduler.cpp
    agni_hal.cpp
    agni_scheduler_hw.cpp
    main_vidya.cpp
)

# Define the primary production executable
add_executable(agni_production ${AGNI_SOURCES})

# Link the pthreads library, required by the scheduler
target_link_libraries(agni_production PRIVATE pthread)

message(STATUS "Project AGNI 'God-Key' v2 has been Hard-Locked. Ready for the final forge.")
