cmake_minimum_required(VERSION 3.14)
project(OS-Project-02)

set(CMAKE_CXX_STANDARD 14)
include_directories(/usr/src/linux-headers-5.3.0-59-generic/include)
include_directories(/usr/src/linux-headers-5.3.0-59-generic/include/linux)
include_directories(/usr/src/linux-headers-5.3.0-59/arch/ia64/include)
add_executable(randocha_module ../src/randocha_module.c ../src/user_space.c)