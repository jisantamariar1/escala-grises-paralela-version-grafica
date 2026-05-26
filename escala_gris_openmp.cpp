#include "escala_gris_openmp.h"
#include <omp.h>
#include <cmath>
#include <cstdint>
#include <algorithm>


void filtroImagenOpenMP(const uint8_t* input, uint8_t* output, int numPixeles) {
    #pragma omp parallel
    {
        int numThreads = omp_get_num_threads();
        int threadID = omp_get_thread_num();
        
        int delta = std::ceil((numPixeles * 1.0) / numThreads);
        int start = delta * threadID;
        int end = std::min(delta * (threadID + 1), numPixeles);

        for (int x = start; x < end; x++) {
            uint8_t r = input[(x * 4) + 0];
            uint8_t g = input[(x * 4) + 1];
            uint8_t b = input[(x * 4) + 2];
            uint8_t a = input[(x * 4) + 3];

            uint8_t gris = static_cast<uint8_t>(0.21f * r + 0.72f * g + 0.07f * b);

            output[(x * 4) + 0] = gris;
            output[(x * 4) + 1] = gris;
            output[(x * 4) + 2] = gris;
            output[(x * 4) + 3] = a;
        }
    }
}