#include "op_simd.h"
#include <immintrin.h>


void filtroImagenSIMD(const uint8_t* input, uint8_t* output, int numPixeles) {
    __m256 luminanciaR = _mm256_set1_ps(0.21f);
    __m256 luminanciaG = _mm256_set1_ps(0.72f);
    __m256 luminanciaB = _mm256_set1_ps(0.07f);

    int i = 0;
    // Procesamos de 8 en 8 píxeles
    for (; i + 7 < numPixeles; i += 8) {
        // Extraemos los canales R, G, B
        __m256 v_R = _mm256_set_ps(
            input[(i+7)*4+0], input[(i+6)*4+0], input[(i+5)*4+0], input[(i+4)*4+0],
            input[(i+3)*4+0], input[(i+2)*4+0], input[(i+1)*4+0], input[(i+0)*4+0]
        );
        __m256 v_G = _mm256_set_ps(
            input[(i+7)*4+1], input[(i+6)*4+1], input[(i+5)*4+1], input[(i+4)*4+1],
            input[(i+3)*4+1], input[(i+2)*4+1], input[(i+1)*4+1], input[(i+0)*4+1]
        );
        __m256 v_B = _mm256_set_ps(
            input[(i+7)*4+2], input[(i+6)*4+2], input[(i+5)*4+2], input[(i+4)*4+2],
            input[(i+3)*4+2], input[(i+2)*4+2], input[(i+1)*4+2], input[(i+0)*4+2]
        );

        // Operación SIMD: 0.21*R + 0.72*G + 0.07*B
        __m256 multi1 = _mm256_mul_ps(luminanciaR, v_R);
        __m256 multi2 = _mm256_mul_ps(luminanciaG, v_G);
        __m256 multi3 = _mm256_mul_ps(luminanciaB, v_B);
        __m256 resV = _mm256_add_ps(multi1, _mm256_add_ps(multi2, multi3));

        float resTemp[8];
        _mm256_storeu_ps(resTemp, resV);
       
        // Volcamos el resultado al buffer de salida (manteniendo formato RGBA)
        for(int j = 0; j < 8; ++j) {
            uint8_t gris = static_cast<uint8_t>(resTemp[j]);
            output[(i + j) * 4 + 0] = gris; // R
            output[(i + j) * 4 + 1] = gris; // G
            output[(i + j) * 4 + 2] = gris; // B
            output[(i + j) * 4 + 3] = input[(i + j) * 4 + 3]; // Alpha original
        }
    }

    // Residuos
    for (; i < numPixeles; i++) {
        uint8_t r = input[(i * 4) + 0];
        uint8_t g = input[(i * 4) + 1];
        uint8_t b = input[(i * 4) + 2];
        uint8_t gris = static_cast<uint8_t>(0.21f * r + 0.72f * g + 0.07f * b);
        
        output[(i * 4) + 0] = gris;
        output[(i * 4) + 1] = gris;
        output[(i * 4) + 2] = gris;
        output[(i * 4) + 3] = input[(i * 4) + 3];
    }
}