#ifndef ESCALA_GRIS_OPENMP_H
#define ESCALA_GRIS_OPENMP_H

#include <string>
#include <cstdint>


void filtroImagenOpenMP(const uint8_t* input, uint8_t* output, int numPixeles);

#endif