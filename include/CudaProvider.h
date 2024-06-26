// CudaProvider.h
//

#pragma once

#include "fbpGlobalDefines.h"

#ifdef USE_CUDA

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool HasCudaDevice();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USE_CUDA
