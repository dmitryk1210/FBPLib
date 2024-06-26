// CudaFunctions.h
//

#pragma once
#include "cuda_runtime.h"

#include "PatternsLibrary.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void ProcessImageCuda(uint16_t* pixels, uint32_t height, uint32_t width, uint32_t pxlFrom, uint32_t pxlTo, uint32_t* K, float* L, uint32_t KLsize, PatternsLibrary* lib);

#ifdef __cplusplus
}
#endif // __cplusplus
