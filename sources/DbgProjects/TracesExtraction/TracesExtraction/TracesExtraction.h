// TracesExhraction.h
//

#pragma once


#include <numeric>

#include "PatternsLibrary.h"

//#define USE_SINGLE_THREAD

typedef uint16_t PixelType;

void ProcessPixel(PixelType* pxls, uint32_t width, uint32_t i, uint32_t j, uint32_t* pxlsK, float* pxlsL, PatternsLibrary& lib);
