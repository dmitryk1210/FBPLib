// TracesExtraction.cpp
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

typedef uint16_t PixelType;

struct PackageInput : public fbp::PackageBase
{
    PackageInput(TGAImage<Pixel24bit>* _pImage, uint32_t _id)
        : PackageBase()
        , pImage(_pImage)
        , id(_id) { }

    TGAImage<Pixel24bit>* pImage = nullptr;
    uint32_t              id     = 0;
};

struct PackageGrayImage : public fbp::PackageBase
{
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    PGMImage<PixelType>*  pGrayImage  = nullptr;
    uint32_t              id          = 0;
};

struct PackageProcessedImage : public fbp::PackageBase
{
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    PGMImage<PixelType>*  pGrayImage  = nullptr;
    std::vector<float>    L;
    uint32_t              id          = 0;
};

struct PackageMerge : public fbp::PackageBase
{
    virtual bool IsTask() { return false; };

    uint32_t id = 0;

    PGMImage<PixelType>* pGrayImage = nullptr;
};

struct PackageMergeTask : public PackageMerge
{
    virtual bool IsTask() override { return true; }
    
    TGAImage<Pixel24bit>* pInputImage = nullptr;
    std::vector<size_t>   K;
    std::vector<float>    L;
     
    uint16_t              nProcessedChunks = 0;
    uint16_t              nChunks          = 0;
};

struct PackageImageChunk : public PackageMerge
{
    PackageMergeTask*     pMergeTask = nullptr;
    size_t                pxlFrom    = 0;
    size_t                pxlTo      = 0;
    std::vector<size_t>   K_chunk;
    std::vector<float>    L_chunk;
};


void ProcessPixel(PixelType* pxls, size_t width, size_t i, size_t j, size_t* pxlsK, float* pxlsL, PatternsLibrary& lib);

std::vector<float> ProcessImage(PGMImage<PixelType>& image)
{
    PatternsLibrary lib;
    lib.Init();
    size_t imageSize = image.pixels.size();

    std::vector<size_t> K(imageSize, 0);
    std::vector<float>  L(imageSize, 0.f);

    for (size_t i = PATTERN_MAX_SIZE / 2; i < image.height - PATTERN_MAX_SIZE / 2; ++i) {
        for (size_t j = PATTERN_MAX_SIZE / 2; j < image.width - PATTERN_MAX_SIZE / 2; ++j) {
            ProcessPixel(reinterpret_cast<PixelType*>(image.pixels.data()), image.width, i, j, &K[i * image.width + j], &L[i * image.width + j], lib);
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
    for (int id = 0; id < 1; ++id) {
        nodeInput.Push(new PackageInput(&inputImage, id));
    }
    nodeInput.Push(new fbp::PackageEndOfStream());

    Node nodeGray("Gray");
    executor.AddTask("task_MakeGray", &nodeInput, &nodeGray,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageInput* pInputImage = static_cast<PackageInput*>(packageIn);
            PackageGrayImage* pGrayImage = new PackageGrayImage();

            pGrayImage->pInputImage = pInputImage->pImage;
            pGrayImage->pGrayImage  = new PGMImage<PixelType>();
            pGrayImage->id          = pInputImage->id;
            const TGAImage<Pixel24bit>& inputImage = *(pGrayImage->pInputImage);
            PGMImage<PixelType>&        imageGray  = *(pGrayImage->pGrayImage);

            size_t imageSize = static_cast<size_t>(inputImage.header.width) * inputImage.header.height;
            
            imageGray.pixels.resize(imageSize);
            imageGray.width  = inputImage.header.width;
            imageGray.height = inputImage.header.height;

            for (size_t i = 0; i < imageSize; ++i) {
                float color = 0.299f * inputImage.pixels[i].red + 0.587f * inputImage.pixels[i].green + 0.114f * inputImage.pixels[i].blue;
                imageGray.pixels[i] = static_cast<PixelType>((color) * 0xFFu);
            }
            imageGray.SaveTo("output\\gray.pgm");

            pTask->GetOutputNode()->Push(pGrayImage);
            delete packageIn;
        }
    ).SetThreadsLimit(1);

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
    
    Node nodeChunksToProcess("ChunksToProcess");
    Node nodeMergeChunks("MergeChunks");

    executor.AddTask("task_SliceChunks", &nodeContrast, std::vector{ &nodeChunksToProcess, &nodeMergeChunks }, 
        [](fbp::PackageBase * packageIn, fbp::Task * pTask)
        {
            const int CHUNKS_PER_IMAGE = 32;
            PackageMergeTask* pMergeTask = new PackageMergeTask();

            {
                PackageGrayImage* pGrayImage = static_cast<PackageGrayImage*>(packageIn);
                pMergeTask->pInputImage = pGrayImage->pInputImage;
                pMergeTask->pGrayImage  = pGrayImage->pGrayImage;
                pMergeTask->id          = pGrayImage->id;
                pMergeTask->nChunks     = CHUNKS_PER_IMAGE;

                delete pGrayImage;
            }

            size_t pixelsToProcess = (pMergeTask->pGrayImage->height - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u)) * (pMergeTask->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            pMergeTask->K.resize(pixelsToProcess);
            pMergeTask->L.resize(pixelsToProcess);
            pTask->GetOutputNode("MergeChunks")->Push(pMergeTask);

            size_t nextChunkStartPxl = 0;
            for (int i = 0; i < CHUNKS_PER_IMAGE; ++i) {
                PackageImageChunk* pChunk = new PackageImageChunk();
                pChunk->id         = pMergeTask->id;
                pChunk->pGrayImage = pMergeTask->pGrayImage;
                pChunk->pMergeTask = pMergeTask;
                pChunk->pxlFrom    = nextChunkStartPxl;
                nextChunkStartPxl  = pixelsToProcess / CHUNKS_PER_IMAGE * (i + 1);
                pChunk->pxlTo      = (i == CHUNKS_PER_IMAGE - 1 ? pixelsToProcess : nextChunkStartPxl);
                pChunk->K_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pChunk->L_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pTask->GetOutputNode("ChunksToProcess")->Push(pChunk);
            }
        }
    ).SetThreadsLimit(1);

    executor.AddTask("task_ProcessChunks", &nodeChunksToProcess, &nodeMergeChunks,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageImageChunk* pChunk = static_cast<PackageImageChunk*>(packageIn);
            const size_t widthToProcess = (pChunk->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            PatternsLibrary lib;
            lib.Init();

            for (size_t pxl = pChunk->pxlFrom; pxl < pChunk->pxlTo; ++pxl) {
                uint16_t i = PATTERN_MAX_SIZE / 2 + pxl / widthToProcess;
                uint16_t j = PATTERN_MAX_SIZE / 2 + pxl % widthToProcess;

                ProcessPixel(reinterpret_cast<PixelType*>(pChunk->pGrayImage->pixels.data()), pChunk->pGrayImage->width, i, j, &(pChunk->K_chunk[pxl - pChunk->pxlFrom]), &(pChunk->L_chunk[pxl - pChunk->pxlFrom]), lib);
            }

            pTask->GetOutputNode("MergeChunks")->Push(pChunk);

        }
    );

    
    Node nodeProcessedImages("ProcessedImages");
    executor.AddTask("task_MergeChunks", &nodeMergeChunks, &nodeProcessedImages,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            static std::map<uint32_t, PackageMergeTask*> idToTask;
            if (static_cast<PackageMerge*>(packageIn)->IsTask()) {
                PackageMergeTask* pMergeTask = static_cast<PackageMergeTask*>(packageIn);
                idToTask.insert(std::make_pair(pMergeTask->id, pMergeTask));
                return;
            }

            PackageImageChunk* pChunk = static_cast<PackageImageChunk*>(packageIn);
            PackageMergeTask* pMergeTask = idToTask[pChunk->id];

            memcpy(pMergeTask->K.data() + pChunk->pxlFrom, pChunk->K_chunk.data(), (pChunk->pxlTo - pChunk->pxlFrom) * sizeof(size_t));
            memcpy(pMergeTask->L.data() + pChunk->pxlFrom, pChunk->L_chunk.data(), (pChunk->pxlTo - pChunk->pxlFrom) * sizeof(float ));

            delete pChunk;
            pChunk = NULL;

            pMergeTask->nProcessedChunks += 1;
            if (pMergeTask->nProcessedChunks != pMergeTask->nChunks) {
                return;
            }

            idToTask.erase(pMergeTask->id);

            PackageProcessedImage* pProcessedImage = new PackageProcessedImage();
            pProcessedImage->id = pMergeTask->id;
            pProcessedImage->pInputImage = pMergeTask->pInputImage;
            pProcessedImage->pGrayImage  = pMergeTask->pGrayImage;
            pProcessedImage->L           = std::move(pMergeTask->L);

            delete pMergeTask;

            pTask->GetOutputNode("ProcessedImages")->Push(pProcessedImage);
        }
    ).SetThreadsLimit(1);
    
    Node nodeOutput("Output");
    executor.AddTask("task_Filter", &nodeProcessedImages, &nodeOutput,
        [](fbp::PackageBase* packageIn, fbp::Task* pTask)
        {
            PackageProcessedImage* pProcessedImage = static_cast<PackageProcessedImage*>(packageIn);
    
            float L_limit = 0;
            {
                const int LIMIT = 200;
                std::vector<float> L_ordered = pProcessedImage->L;
                std::partial_sort(L_ordered.begin(), L_ordered.begin() + LIMIT, L_ordered.end(), std::greater{});
                L_limit = L_ordered[LIMIT - 1];
            }
    
            float L_max = *std::max_element(pProcessedImage->L.cbegin(), pProcessedImage->L.cend());
            
            TGAImage<Pixel24bit> result;
            result.header = pProcessedImage->pInputImage->header;
            result.pixels.resize(result.header.width * result.header.height);
            int counter = 0;

            const int widthToProcess = (result.header.width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));
    
            for (size_t i = 0; i < result.header.height; ++i) {
                for (size_t j = 0; j < result.header.width; ++j) {
                    size_t pixelIdx = i * result.header.width + j;
                    result.pixels[pixelIdx].blue = 0;
                    result.pixels[pixelIdx].green = 0;
                    if (i < PATTERN_MAX_SIZE / 2 || i >= (result.header.height - PATTERN_MAX_SIZE / 2) || j < PATTERN_MAX_SIZE / 2 || j >= (result.header.width - PATTERN_MAX_SIZE / 2)) {
                        result.pixels[pixelIdx].red = 0;
                        continue;
                    }
                    size_t pixelProcessIdx = (i - PATTERN_MAX_SIZE / 2) * widthToProcess + (j - PATTERN_MAX_SIZE / 2);
                    if (L_limit > pProcessedImage->L[pixelProcessIdx]) {
                        result.pixels[pixelIdx].red = 0;
                    }
                    else {
                        result.pixels[pixelIdx].red = static_cast<uint8_t>(std::clamp<uint32_t>(pProcessedImage->L[pixelProcessIdx] / L_max * UINT8_MAX, 0, UINT8_MAX));
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
