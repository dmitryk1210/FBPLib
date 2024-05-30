#include <numeric>

#include "PatternsLibrary.h"

typedef uint16_t PixelType;

void ProcessPixel(PixelType* pxls, size_t width, size_t i, size_t j, size_t* pxlsK, float* pxlsL, PatternsLibrary& lib)
{
    size_t pixelIdx = i * width + j;

    int   k_ast_max = -1;
    float L_ast_max = 0.f;
    const uint8_t* pattern = nullptr;

    for (int k = 0; k < PATTERNS_COUNT; ++k) {
        int WL;
        int kGroup;
        if (k < UP5_OFFSET) {
            WL = UP3_SIZE;
            kGroup = k - UP3_OFFSET;
            pattern = &(lib.up3[kGroup][0]);
        }
        else if (k < UP7_OFFSET) {
            WL = UP5_SIZE;
            kGroup = k - UP5_OFFSET;
            pattern = &(lib.up5[kGroup][0]);
        }
        else if (k < UP9_OFFSET) {
            WL = UP7_SIZE;
            kGroup = k - UP7_OFFSET;
            pattern = &(lib.up7[kGroup][0]);
        }
        else if (k < UP11_OFFSET) {
            WL = UP9_SIZE;
            kGroup = k - UP9_OFFSET;
            pattern = &(lib.up9[kGroup][0]);
        }
        else {
            WL = UP11_SIZE;
            kGroup = k - UP11_OFFSET;
            pattern = &(lib.up11[kGroup][0]);
        }

        float SY = 0.f;
        float SYF = 0.f;

        for (int p = 0; p < WL; ++p) {
            for (int q = 0; q < WL; ++q) {
                PixelType pxlColor = pxls[pixelIdx];
                SY += pxlColor;
                SYF += pxlColor * pattern[p * WL + q];
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
            a_ast = lib.C_a[k] * (SYF * S - SY * lib.SF[k]);
            b_ast = lib.C_b[k] * (SY - a_ast * lib.SF[k]);
            L_ast = 2 * a_ast * SYF - a_ast * a_ast * lib.SFF[k] + b_ast * b_ast * S;
        }

        if (a_ast > 0.f && L_ast_max < L_ast || k_ast_max == -1) {
            L_ast_max = L_ast;
            k_ast_max = k;
        }
    }


    pxlsK[pixelIdx] = k_ast_max;
    pxlsL[pixelIdx] = L_ast_max;
}
