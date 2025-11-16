# ============================================================================
# CMakeLists.txt "God-Key" for Project AGNI
# Hard-Locks the complete production solution (19 files)
# Forges the "Foundation" using the "Hard-Locked" Factory (compilers)
# ============================================================================

# 1. "Hard-Lock" (Find) the "Factory" (g++ 13.3.0, cmake 3.28.3)
cmake_minimum_required(VERSION 3.10)
project(AgniProduction CXX)

# Set the C++ standard to C++11, as required by the codebase
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. "Hard-Lock" (Compiles) all "Vidya" (Layer 1 + Layer 2) .cpp files.
# This list includes all production C++ source files and excludes test files,
# pseudocode, and other non-source files.
set(AGNI_SOURCES
    api_gateway.cpp
    vector_utils.cpp
    scheduler.cpp
    agni_hal.cpp
    agni_scheduler_hw.cpp
    main_vidya.cpp
)

# 3. "Hard-Lock" (Links) the "Goliath" (MLIR/LLVM) "placeholders" (.td files).
# This custom command generates C++ sources from the MLIR TableGen file.
# It assumes 'mlir-tblgen' is available in the "Hard-Locked" Factory environment.
set(MLIR_GENERATED_FILE ${CMAKE_CURRENT_BINARY_DIR}/AgniDialect.h.inc)
add_custom_command(
    OUTPUT ${MLIR_GENERATED_FILE}
    COMMAND mlir-tblgen -gen-op-defs -I=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/agni_mlir_dialect.td -o ${MLIR_GENERATED_FILE}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/agni_mlir_dialect.td
    COMMENT "Generating MLIR dialect definitions from agni_mlir_dialect.td"
)

# Create a custom target to ensure the MLIR file is generated.
add_custom_target(AgniDialectGen DEPENDS ${MLIR_GENERATED_FILE})

# Define the primary production executable.
add_executable(agni_production ${AGNI_SOURCES})

# Ensure the MLIR file is generated before the main build starts.
add_dependencies(agni_production AgniDialectGen)

# Add the directory with the generated file to the include path for the compiler.
target_include_directories(agni_production PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# 4. "Hard-Lock" (Links) the "Goliath" (Linker) "placeholder" (project_agni.ld).
# This sets the linker flags to use our custom memory map.
set_target_properties(agni_production PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/project_agni.ld"
)

# Link the pthreads library, which is required by the scheduler.
target_link_libraries(agni_production PRIVATE pthread)

# Final status message to confirm the "God-Key" is ready.
message(STATUS "Project AGNI 'God-Key' has been Hard-Locked. Ready to forge the Foundation.")
