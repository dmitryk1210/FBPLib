// TracesExtraction.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

#include "Executor.h"
#include "Node.h"
#include "Task.h"

#include "Images.h"
#include "PatternsLibrary.h"



class PackageInput : public fbp::PackageBase
{
public:
    PackageInput(TGAImage<Pixel24bit>* _pImage)
        : PackageBase()
        , pImage(_pImage) { }

    TGAImage<Pixel24bit>* pImage = nullptr;
};

class PackageGrayImage : public fbp::PackageBase
{
public:
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    PGMImage<uint16_t>*   pGrayImage  = nullptr;
};

class PackageProcessedImage : public fbp::PackageBase
{
public:
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    PGMImage<uint16_t>*   pGrayImage  = nullptr;
    std::vector<float>    L;
};

class PackageImageChunk : public fbp::PackageBase
{
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    PGMImage<uint16_t>*   pGrayImage  = nullptr;
    size_t                pxlFrom     = 0;
    size_t                pxlTo       = 0;
    std::vector<float>    L_chunk;
};

typedef uint16_t PixelType;

void ProcessPixel(PixelType* pxls, size_t width, size_t i, size_t j, size_t* pxlsK, float* pxlsL, PatternsLibrary& lib);

std::vector<float> ProcessImage(PGMImage<PixelType>& image)
{
    PatternsLibrary lib;
    lib.Init();
    size_t imageSize = image.pixels.size();

    std::vector<size_t> K(imageSize, 0);
    std::vector<float>  L(imageSize, 0.f);

    for (size_t i = UP11_SIZE / 2; i < image.height - UP11_SIZE / 2; ++i) {
        for (size_t j = UP11_SIZE / 2; j < image.width - UP11_SIZE / 2; ++j) {
            ProcessPixel(reinterpret_cast<PixelType*>(image.pixels.data()), image.width, i, j, K.data(), L.data(), lib);
        }
    }
    return L;
}

//#define USE_SINGLE_THREAD

int main()
{
    using namespace fbp;

    Executor executor;

    auto start = std::chrono::high_resolution_clock::now();

    TGAImage<Pixel24bit> inputImage;
    if (inputImage.LoadFrom("example01.tga")) {
        return 1;
    }

    size_t imageSize = static_cast<size_t>(inputImage.header.width) * inputImage.header.height;

#ifdef USE_SINGLE_THREAD
    PGMImage<uint16_t> imageGray;
    imageGray.pixels.resize(imageSize);
    imageGray.width = inputImage.header.width;
    imageGray.height = inputImage.header.height;

    for (size_t i = 0; i < imageSize; ++i) {
        float color = 0.299f * inputImage.pixels[i].red + 0.587f * inputImage.pixels[i].green + 0.114f * inputImage.pixels[i].blue;
        imageGray.pixels[i] = static_cast<uint16_t>((color) * 0xFFu);
    }
    imageGray.SaveTo("output\\gray.pgm");

    imageGray.AddContrastFilter(50);

    imageGray.SaveTo("output\\grayContrast.pgm");

    std::vector<float> L = ProcessImage(imageGray);

    float L_limit = 0;
    {
        const int LIMIT = 20;
        std::vector<float> L_ordered = L;
        std::partial_sort(L_ordered.begin(), L_ordered.begin() + 1500, L_ordered.end(), std::greater{});
        L_limit = L_ordered[LIMIT - 1];
    }

    float L_max = *std::max_element(L.cbegin(), L.cend());
    // convert 16bit gray to 24bit rgb
    TGAImage<Pixel24bit> result;
    result.header = inputImage.header;
    result.pixels.resize(imageSize);
    int counter = 0;

    for (size_t i = 0; i < result.header.height; ++i) {
        for (size_t j = 0; j < result.header.width; ++j) {
            size_t pixelIdx = i * result.header.width + j;
            result.pixels[pixelIdx].blue = 0;
            result.pixels[pixelIdx].green = 0;
            if (L_limit > L[pixelIdx]) {
                result.pixels[pixelIdx].red = 0;
            }
            else {
                result.pixels[pixelIdx].red = static_cast<uint8_t>(std::clamp<uint32_t>(L[pixelIdx] / L_max * UINT8_MAX, 0, UINT8_MAX));
                counter++;
            }
        }
    }

    result.SaveTo("output\\result.tga");


    std::cout << "The image has been converted to grayscale and saved as output.tga.\n" << std::flush;
#else
    Node nodeInput("Input");
    for (int i = 0; i < 1; ++i) {
        nodeInput.Push(new PackageInput(&inputImage));
    }
    nodeInput.Push(new fbp::PackageEndOfStream());

    Node nodeGray("Gray");
    executor.AddTask("task_MakeGray", &nodeInput, &nodeGray,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageGrayImage* pGrayImage = new PackageGrayImage();

            pGrayImage->pInputImage = static_cast<PackageInput*>(packageIn)->pImage;
            pGrayImage->pGrayImage  = new PGMImage<uint16_t>();
            const TGAImage<Pixel24bit>& inputImage = *(pGrayImage->pInputImage);
            PGMImage<uint16_t>&         imageGray  = *(pGrayImage->pGrayImage);

            size_t imageSize = static_cast<size_t>(inputImage.header.width) * inputImage.header.height;
            
            imageGray.pixels.resize(imageSize);
            imageGray.width  = inputImage.header.width;
            imageGray.height = inputImage.header.height;

            for (size_t i = 0; i < imageSize; ++i) {
                float color = 0.299f * inputImage.pixels[i].red + 0.587f * inputImage.pixels[i].green + 0.114f * inputImage.pixels[i].blue;
                imageGray.pixels[i] = static_cast<uint16_t>((color) * 0xFFu);
            }
            imageGray.SaveTo("output\\gray.pgm");

            pTask->GetOutputNode()->Push(pGrayImage);
            delete packageIn;
        }
    );

    Node nodeContrast("Contrast");
    executor.AddTask("task_MakeContrast", &nodeGray, &nodeContrast,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageGrayImage* pGrayImage = static_cast<PackageGrayImage*>(packageIn);
            pGrayImage->pGrayImage->AddContrastFilter(50);
            pGrayImage->pGrayImage->SaveTo("output\\grayContrast.pgm");
    
            pTask->GetOutputNode()->Push(packageIn);
        }
    );
    
    //Node nodeChunksToProcess("ChunksToProcess");
    //
    //Node nodeMergeChunks("MergeChunks");
    
    Node nodeProcessedImages("ProcessedImages");
    executor.AddTask("task_ProcessImage", &nodeContrast, &nodeProcessedImages,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageGrayImage* pGrayImage = static_cast<PackageGrayImage*>(packageIn);
            PackageProcessedImage* pProcessedImage = new PackageProcessedImage();
    
            pProcessedImage->pInputImage = pGrayImage->pInputImage;
            pProcessedImage->pGrayImage  = pGrayImage->pGrayImage;
            pProcessedImage->L           = ProcessImage(*pProcessedImage->pGrayImage);
    
            pTask->GetOutputNode()->Push(pProcessedImage);
            
            delete packageIn;
        }
    );
    
    Node nodeOutput("Output");
    executor.AddTask("task_Filter", &nodeProcessedImages, &nodeOutput,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageProcessedImage* pProcessedImage = static_cast<PackageProcessedImage*>(packageIn);
    
            float L_limit = 0;
            {
                const int LIMIT = 20;
                std::vector<float> L_ordered = pProcessedImage->L;
                std::partial_sort(L_ordered.begin(), L_ordered.begin() + 1500, L_ordered.end(), std::greater{});
                L_limit = L_ordered[LIMIT - 1];
            }
    
            float L_max = *std::max_element(pProcessedImage->L.cbegin(), pProcessedImage->L.cend());
            // convert 16bit gray to 24bit rgb
            TGAImage<Pixel24bit> result;
            result.header = pProcessedImage->pInputImage->header;
            result.pixels.resize(result.header.width * result.header.height);
            int counter = 0;
    
            for (size_t i = 0; i < result.header.height; ++i) {
                for (size_t j = 0; j < result.header.width; ++j) {
                    size_t pixelIdx = i * result.header.width + j;
                    result.pixels[pixelIdx].blue = 0;
                    result.pixels[pixelIdx].green = 0;
                    if (L_limit > pProcessedImage->L[pixelIdx]) {
                        result.pixels[pixelIdx].red = 0;
                    }
                    else {
                        result.pixels[pixelIdx].red = static_cast<uint8_t>(std::clamp<uint32_t>(pProcessedImage->L[pixelIdx] / L_max * UINT8_MAX, 0, UINT8_MAX));
                        counter++;
                    }
                }
            }
    
            result.SaveTo("output\\result.tga");
    
            delete pProcessedImage->pGrayImage;
            delete packageIn;
        }
    );
    
    executor.Execute(true);

    while (!executor.IsDone())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    executor.PrintDebugData("output\\debugData.out");
    executor.Terminate();

#endif // USE_SINGLE_THREAD

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}
