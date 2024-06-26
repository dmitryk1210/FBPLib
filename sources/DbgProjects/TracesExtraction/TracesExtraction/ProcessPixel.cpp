// ProcessPixel.cpp
//

#include <numeric>

#include "PatternsLibrary.h"
#include "TracesExtraction.h"


void ProcessPixel(PixelType* pxls, uint32_t width, uint32_t i, uint32_t j, uint32_t* pxlK, float* pxlL, PatternsLibrary* pLib)
{
    uint32_t pixelIdx = i * width + j;

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

        for (int p = -WL/2; p <= WL/2; ++p) {
            for (int q = -WL/2; q <= WL/2; ++q) {
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


    *pxlK = k_ast_max;
    *pxlL = L_ast_max;
}
