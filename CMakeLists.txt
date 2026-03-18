cmake_minimum_required(VERSION 3.10)
project(Dynamol C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags for suppressing warnings during development
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# Add include directories (so header files can be found)
include_directories(${CMAKE_SOURCE_DIR}/system)
include_directories(${CMAKE_SOURCE_DIR}/sconfigs)
include_directories(${CMAKE_SOURCE_DIR}/intrinsics)
include_directories(${CMAKE_SOURCE_DIR}/calc_algo/header)
include_directories(${CMAKE_SOURCE_DIR}/internal_utils)
include_directories(${CMAKE_SOURCE_DIR}/big_numbers)

# =====================================================
# BigInt Executable (compilation test)
# =====================================================
add_executable(main
    big_numbers/bigInt.c
    big_numbers/bigInt_io.c
    internal_utils/bigNum_utils.c
    internal_utils/misc_utils.c
    internal_utils/str_parse.c
    intrinsics/limb_math.c
    calc_algo/src/add_sub.c
    calc_algo/src/mul.c
    calc_algo/src/div.c
    calc_algo/src/mod.c
    calc_algo/src/mod_op.c
    calc_algo/src/num_theory.c
    calc_algo/src/pow_root.c
)

target_compile_options(main PRIVATE -Wno-unused-function)
target_sources(main PRIVATE ztest/main.c)
