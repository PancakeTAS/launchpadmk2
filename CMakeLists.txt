cmake_minimum_required(VERSION 3.29)
project(launchpadmk2)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB SOURCES "src/*.c")

add_compile_options(-Wall -Wextra -Werror)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
else()
  add_compile_options(-O3 -march=native -mtune=native)
endif()

add_executable(launchpadmk2 ${SOURCES})

target_link_libraries(launchpadmk2 asound)