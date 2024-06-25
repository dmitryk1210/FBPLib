#include <cassert>
#include <numeric>

#include <stdio.h>

#include "CudaFunctions.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"


#include "PatternsLibrary.h"
#include "TracesExtraction.h"


#define CUDA_CHECK_ERROR(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            printf("CUDA error at %s %d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
            return; \
        } \
    } while (0)


__global__ void ProcessPixelGPU(PixelType* pxls, uint32_t width, uint32_t pxlFrom, uint32_t pxlTo, uint32_t* pxlK, float* pxlL, PatternsLibrary* pLib)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x + pxlFrom;
    if (idx >= pxlTo) return;

    uint32_t widthToProcess = (width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));
    uint16_t i = PATTERN_MAX_SIZE / 2 + idx / widthToProcess;
    uint16_t j = PATTERN_MAX_SIZE / 2 + idx % widthToProcess;

    int   k_ast_max = -1;
    float L_ast_max = 0.f;
    const uint8_t* pattern = nullptr;

    for (int k = 0; k < PATTERNS_COUNT; ++k) {
        int WL;
        int kGroup;
        if (k < UP5_OFFSET) {
            WL = UP3_SIZE;
            kGroup = k - UP3_OFFSET;
            pattern = &(pLib->up3[kGroup][0]);
        }
        else if (k < UP7_OFFSET) {
            WL = UP5_SIZE;
            kGroup = k - UP5_OFFSET;
            pattern = &(pLib->up5[kGroup][0]);
        }
        else if (k < UP9_OFFSET) {
            WL = UP7_SIZE;
            kGroup = k - UP7_OFFSET;
            pattern = &(pLib->up7[kGroup][0]);
        }
        else if (k < UP11_OFFSET) {
            WL = UP9_SIZE;
            kGroup = k - UP9_OFFSET;
            pattern = &(pLib->up9[kGroup][0]);
        }
        else {
            WL = UP11_SIZE;
            kGroup = k - UP11_OFFSET;
            pattern = &(pLib->up11[kGroup][0]);
        }

        float SY = 0.f;
        float SYF = 0.f;

        for (int p = -WL / 2; p <= WL / 2; ++p) {
            for (int q = -WL / 2; q <= WL / 2; ++q) {
                PixelType pxlColor = pxls[(i + p) * width + (j + q)];
                SY += pxlColor;
                SYF += pxlColor * pattern[(p + WL / 2) * WL + (q + WL / 2)];
            }
        }

        float a_ast;
        float b_ast;
        float L_ast;

        const int S = WL * WL;

        if (kGroup == 0) {
            a_ast = 0;
            b_ast = SY / S;
            L_ast = SY * SY / S;
        }
        else {
            a_ast = pLib->C_a[k] * (SYF * S - SY * pLib->SF[k]);
            b_ast = pLib->C_b[k] * (SY - a_ast * pLib->SF[k]);
            L_ast = 2 * a_ast * SYF - a_ast * a_ast * pLib->SFF[k] + b_ast * b_ast * S;
        }

        if (a_ast > 0.f && L_ast_max < L_ast || k_ast_max == -1) {
            L_ast_max = L_ast;
            k_ast_max = k;
        }
    }


    *(pxlK + (idx - pxlFrom)) = k_ast_max;
    *(pxlL + (idx - pxlFrom)) = L_ast_max;
}


extern "C" void ProcessImageCuda(uint16_t* pixels, uint32_t height, uint32_t width, uint32_t pxlFrom, uint32_t pxlTo, uint32_t * K, float* L, uint32_t KLsize, PatternsLibrary* pLib) {
    const int THREADS_BLOCK_SIZE = 256;
    const int GRID_SIZE = (pxlTo - pxlFrom) / THREADS_BLOCK_SIZE + 1;

    CUDA_CHECK_ERROR(cudaSetDevice(0));

    uint16_t*        devPixels = nullptr;
    uint32_t*        devK      = nullptr;
    float*           devL      = nullptr;
    PatternsLibrary* devLib    = nullptr;

    CUDA_CHECK_ERROR(cudaMalloc((void**)&devPixels, height * width * sizeof(uint16_t)));
    CUDA_CHECK_ERROR(cudaMalloc((void**)&devK, (pxlTo - pxlFrom) * sizeof(uint32_t)));
    CUDA_CHECK_ERROR(cudaMalloc((void**)&devL, (pxlTo - pxlFrom) * sizeof(float)));
    CUDA_CHECK_ERROR(cudaMalloc((void**)&devLib, sizeof(PatternsLibrary)));
    CUDA_CHECK_ERROR(cudaMemcpy(devPixels, pixels, height * width * sizeof(uint16_t), cudaMemcpyHostToDevice));
    CUDA_CHECK_ERROR(cudaMemcpy(devLib,    pLib,   sizeof(PatternsLibrary),           cudaMemcpyHostToDevice));

    ProcessPixelGPU <<<GRID_SIZE, THREADS_BLOCK_SIZE>>> (devPixels, width, pxlFrom, pxlTo, devK, devL, devLib);

    // Check for any errors launching the kernel
    CUDA_CHECK_ERROR(cudaGetLastError());
    CUDA_CHECK_ERROR(cudaDeviceSynchronize());
    
    CUDA_CHECK_ERROR(cudaMemcpy(K, devK, (pxlTo - pxlFrom) * sizeof(uint32_t), cudaMemcpyDeviceToHost));
    CUDA_CHECK_ERROR(cudaMemcpy(L, devL, (pxlTo - pxlFrom) * sizeof(float),    cudaMemcpyDeviceToHost));

    cudaFree(devPixels);
    cudaFree(devK);
    cudaFree(devL);
    cudaFree(devLib);
}
