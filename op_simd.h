#ifndef OP_SIMD_H
#define OP_SIMD_H

#include <cstdint>


void filtroImagenSIMD(const uint8_t* input, uint8_t* output, int numPixeles);

#endif