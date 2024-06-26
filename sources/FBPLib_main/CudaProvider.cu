// CudaProvider.cu
//

#include "fbpGlobalDefines.h"

#ifdef USE_CUDA
#include <cassert>
#include <stdio.h>

#include "CudaProvider.h"
#include "cuda_runtime.h"


extern "C" bool HasCudaDevice() {
    cudaDeviceProp prop;
    cudaError_t err = cudaGetDeviceProperties(&prop, 0);
    return err == cudaSuccess;
}
#endif // USE_CUDA
