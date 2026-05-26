cmake_minimum_required(VERSION 3.16)

project(MiProyecto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -fopenmp")

# Buscar librerías
find_package(fmt CONFIG REQUIRED)

find_package(SFML COMPONENTS System Graphics Window CONFIG REQUIRED)

# Ejecutable
add_executable(mi-proyecto main.cpp op_simd.cpp escala_gris_openmp.cpp) 

# Enlazar todo
target_link_libraries(mi-proyecto PRIVATE fmt::fmt SFML::Graphics SFML::Window SFML::System)