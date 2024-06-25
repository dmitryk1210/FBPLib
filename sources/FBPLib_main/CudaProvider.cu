#include <cassert>
#include <stdio.h>

#include "CudaProvider.h"
#include "cuda_runtime.h"


extern "C" bool HasCudaDevice() {
    cudaDeviceProp prop;
    cudaError_t err = cudaGetDeviceProperties(&prop, 0);
    return err == cudaSuccess;
}
