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

#include "fbplib.h"

#include "Images.h"
#include "PatternsLibrary.h"
#include "TracesExtraction.h"


#ifdef USE_SINGLE_THREAD

std::vector<float> ProcessImage(PGMImage<PixelType>& image)
{
    PatternsLibrary lib;
    lib.Init();
    uint32_t imageSize = image.pixels.size();

    std::vector<uint32_t> K(imageSize, 0);
    std::vector<float>    L(imageSize, 0.f);

    for (uint32_t i = PATTERN_MAX_SIZE / 2; i < image.height - PATTERN_MAX_SIZE / 2; ++i) {
        for (uint32_t j = PATTERN_MAX_SIZE / 2; j < image.width - PATTERN_MAX_SIZE / 2; ++j) {
            ProcessPixel(reinterpret_cast<PixelType*>(image.pixels.data()), image.width, i, j, &K[i * image.width + j], &L[i * image.width + j], lib);
        }
    }
    return L;
}

#else

struct PackageInput : public fbp::PackageBase
{
    PackageInput(const std::shared_ptr<TGAImage<Pixel24bit>>& _pImage, uint32_t _id)
        : PackageBase()
        , pImage(_pImage)
        , id(_id) { }

    PackageInput(std::shared_ptr<TGAImage<Pixel24bit>>&& _pImage, uint32_t _id)
        : PackageBase()
        , pImage(_pImage)
        , id(_id) { }

    std::shared_ptr<TGAImage<Pixel24bit>> pImage;
    uint32_t id = 0;
};

struct PackageGrayImage : public fbp::PackageBase
{
    std::shared_ptr<TGAImage<Pixel24bit>> pInputImage;
    std::shared_ptr<PGMImage<PixelType>>  pGrayImage;
    uint32_t id = 0;
};

struct PackageProcessedImage : public fbp::PackageBase
{
    std::shared_ptr<TGAImage<Pixel24bit>> pInputImage;
    std::shared_ptr<PGMImage<PixelType>>  pGrayImage;
    std::vector<float> L;
    uint32_t           id          = 0;
};

struct PackageMerge : public fbp::PackageBase
{
    virtual bool IsTask() { return false; };

    uint32_t id = 0;

    std::shared_ptr<PGMImage<PixelType>> pGrayImage;
};

struct PackageMergeTask : public PackageMerge
{
    virtual bool IsTask() override { return true; }
    
    std::shared_ptr<TGAImage<Pixel24bit>> pInputImage;
    std::vector<uint32_t> K;
    std::vector<float>    L;
    uint16_t              nProcessedChunks = 0;
    uint16_t              nChunks          = 0;
};

struct PackageImageChunk : public PackageMerge
{
    PackageMergeTask*     pMergeTask = nullptr;
    uint32_t              pxlFrom    = 0;
    uint32_t              pxlTo      = 0;
    std::vector<uint32_t> K_chunk;
    std::vector<float>    L_chunk;
};

struct PackageSaveToFile : public fbp::PackageBase
{
    std::string filename;
    std::shared_ptr<TGAImage<Pixel24bit>> pTGAImage;
    std::shared_ptr<PGMImage<PixelType>>  pPGMImage;
};

typedef std::unique_ptr<fbp::PackageBase> uptr_PackageBase;

#endif // USE_SINGLE_THREAD


template <typename Derived, typename Base>
std::unique_ptr<Derived> uniquePtrCast(std::unique_ptr<Base> ptr) {
    return std::unique_ptr<Derived>(static_cast<Derived*>(ptr.release()));
}

int main()
{
    using namespace fbp;

    Executor executor;

    auto start = std::chrono::high_resolution_clock::now();

#ifdef USE_SINGLE_THREAD
    TGAImage<Pixel24bit> inputImage;
    if (inputImage.LoadFrom("example01.tga")) {
        return 1;
    }

    uint32_t imageSize = static_cast<uint32_t>(inputImage.header.width) * inputImage.header.height;

    PGMImage<uint16_t> imageGray;
    imageGray.pixels.resize(imageSize);
    imageGray.width = inputImage.header.width;
    imageGray.height = inputImage.header.height;

    for (uint32_t i = 0; i < imageSize; ++i) {
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
    
    TGAImage<Pixel24bit> result;
    result.header = inputImage.header;
    result.pixels.resize(imageSize);
    int counter = 0;

    for (uint32_t i = 0; i < result.header.height; ++i) {
        for (uint32_t j = 0; j < result.header.width; ++j) {
            uint32_t pixelIdx = i * result.header.width + j;
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

#else
    Node nodeInput("Input");
    for (int id = 0; id < 1; ++id) {
        std::shared_ptr<TGAImage<Pixel24bit>> pInputImage = std::make_shared<TGAImage<Pixel24bit>>();
        if (pInputImage->LoadFrom("example01.tga")) {
            assert(false);
            return -1;
        }
        nodeInput.Push(std::make_unique<PackageInput>(pInputImage, id));
    }
    nodeInput.Push(std::make_unique<fbp::PackageEndOfStream>());

    Node nodeGray("Gray");
    Node nodeSave("Save");
    executor.AddTask("task_MakeGray", &nodeInput, std::vector{ &nodeGray, &nodeSave },
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageInput> pInputImage    = uniquePtrCast<PackageInput>(std::move(packageIn));
            std::unique_ptr<PackageGrayImage> pGrayImage = std::make_unique<PackageGrayImage>();

            pGrayImage->pInputImage = pInputImage->pImage;
            pGrayImage->pGrayImage  = std::make_shared<PGMImage<PixelType>>();
            pGrayImage->id          = pInputImage->id;
            const TGAImage<Pixel24bit>& inputImage = *(pGrayImage->pInputImage);
            PGMImage<PixelType>&        imageGray  = *(pGrayImage->pGrayImage);

            uint32_t imageSize = static_cast<uint32_t>(inputImage.header.width) * inputImage.header.height;
            
            imageGray.pixels.resize(imageSize);
            imageGray.width  = inputImage.header.width;
            imageGray.height = inputImage.header.height;

            for (uint32_t i = 0; i < imageSize; ++i) {
                float color = 0.299f * inputImage.pixels[i].red + 0.587f * inputImage.pixels[i].green + 0.114f * inputImage.pixels[i].blue;
                imageGray.pixels[i] = static_cast<PixelType>((color) * 0xFFu);
            }

            std::unique_ptr<PackageSaveToFile> pSaveToFile = std::make_unique<PackageSaveToFile>();
            pSaveToFile->filename = "output\\gray.pgm";
            pSaveToFile->pPGMImage = pGrayImage->pGrayImage;
            pTask->GetOutputNode("Save")->Push(std::move(pSaveToFile));

            pTask->GetOutputNode("Gray")->Push(std::move(pGrayImage));
        }
    ).SetThreadsLimit(1);

    Node nodeSaveout("Saveout");
    executor.AddTask("task_Save", &nodeSave, &nodeSaveout,
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageSaveToFile> pSaveToFile = uniquePtrCast<PackageSaveToFile>(std::move(packageIn));
            if (pSaveToFile->pTGAImage) {
                pSaveToFile->pTGAImage->SaveTo(pSaveToFile->filename);
            }
            if (pSaveToFile->pPGMImage) {
                pSaveToFile->pPGMImage->SaveTo(pSaveToFile->filename);
            }
        }
    );

    Node nodeContrast("Contrast");
    executor.AddTask("task_MakeContrast", &nodeGray, &nodeContrast,
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageGrayImage> pGrayImage = uniquePtrCast<PackageGrayImage>(std::move(packageIn));
            pGrayImage->pGrayImage->AddContrastFilter(50);
            pGrayImage->pGrayImage->SaveTo("output\\grayContrast.pgm");
    
            pTask->GetOutputNode()->Push(std::move(pGrayImage));
        }
    );
    
    Node nodeChunksToProcess("ChunksToProcess");
    Node nodeMergeChunks("MergeChunks");

    executor.AddTask("task_SliceChunks", &nodeContrast, std::vector{ &nodeChunksToProcess, &nodeMergeChunks }, 
        [](uptr_PackageBase packageIn, fbp::Task * pTask)
        {
            const int CHUNKS_PER_IMAGE = 32;
            std::unique_ptr<PackageMergeTask> pMergeTask = std::make_unique<PackageMergeTask>();

            {
                std::unique_ptr<PackageGrayImage> pGrayImage = uniquePtrCast<PackageGrayImage>(std::move(packageIn));
                pMergeTask->pInputImage = pGrayImage->pInputImage;
                pMergeTask->pGrayImage  = pGrayImage->pGrayImage;
                pMergeTask->id          = pGrayImage->id;
                pMergeTask->nChunks     = CHUNKS_PER_IMAGE;
            }

            uint32_t pixelsToProcess = (pMergeTask->pGrayImage->height - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u)) * (pMergeTask->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            pMergeTask->K.resize(pixelsToProcess);
            pMergeTask->L.resize(pixelsToProcess);

            uint32_t nextChunkStartPxl = 0;
            for (int i = 0; i < CHUNKS_PER_IMAGE; ++i) {
                std::unique_ptr<PackageImageChunk> pChunk = std::make_unique<PackageImageChunk>();
                pChunk->id         = pMergeTask->id;
                pChunk->pGrayImage = pMergeTask->pGrayImage;
                pChunk->pMergeTask = &(*pMergeTask);
                pChunk->pxlFrom    = nextChunkStartPxl;
                nextChunkStartPxl  = pixelsToProcess / CHUNKS_PER_IMAGE * (i + 1);
                pChunk->pxlTo      = (i == CHUNKS_PER_IMAGE - 1 ? pixelsToProcess : nextChunkStartPxl);
                pChunk->K_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pChunk->L_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pTask->GetOutputNode("ChunksToProcess")->Push(std::move(pChunk));
            }
            pTask->GetOutputNode("MergeChunks")->Push(std::move(pMergeTask));
        }
    ).SetThreadsLimit(1);

    executor.AddTask("task_ProcessChunks", &nodeChunksToProcess, &nodeMergeChunks,
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageImageChunk> pChunk = uniquePtrCast<PackageImageChunk>(std::move(packageIn));
            const size_t widthToProcess = (pChunk->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            PatternsLibrary lib;
            lib.Init();

            for (uint32_t pxl = pChunk->pxlFrom; pxl < pChunk->pxlTo; ++pxl) {
                uint16_t i = PATTERN_MAX_SIZE / 2 + pxl / widthToProcess;
                uint16_t j = PATTERN_MAX_SIZE / 2 + pxl % widthToProcess;

                ProcessPixel(reinterpret_cast<PixelType*>(pChunk->pGrayImage->pixels.data()), pChunk->pGrayImage->width, i, j, &(pChunk->K_chunk[pxl - pChunk->pxlFrom]), &(pChunk->L_chunk[pxl - pChunk->pxlFrom]), lib);
            }

            pTask->GetOutputNode("MergeChunks")->Push(std::move(pChunk));
        }
    );

    
    Node nodeProcessedImages("ProcessedImages");
    executor.AddTask("task_MergeChunks", &nodeMergeChunks, &nodeProcessedImages,
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            static std::map<uint32_t, std::shared_ptr<PackageMergeTask>> idToTask;
            std::unique_ptr<PackageMerge> pMerge = uniquePtrCast<PackageMerge>(std::move(packageIn));
            if (pMerge->IsTask()) {
                std::unique_ptr<PackageMergeTask> pMergeTask = uniquePtrCast<PackageMergeTask>(std::move(pMerge));
                uint32_t id = pMergeTask->id;
                idToTask.insert(std::make_pair(id, std::shared_ptr<PackageMergeTask>(std::move(pMergeTask))));
                return;
            }

            std::unique_ptr<PackageImageChunk> pChunk = uniquePtrCast<PackageImageChunk>(std::move(pMerge));
            std::shared_ptr<PackageMergeTask> pMergeTask = idToTask[pChunk->id];

            memcpy(pMergeTask->K.data() + pChunk->pxlFrom, pChunk->K_chunk.data(), (pChunk->pxlTo - pChunk->pxlFrom) * sizeof(uint32_t));
            memcpy(pMergeTask->L.data() + pChunk->pxlFrom, pChunk->L_chunk.data(), (pChunk->pxlTo - pChunk->pxlFrom) * sizeof(float ));

            pChunk.reset();

            pMergeTask->nProcessedChunks += 1;
            if (pMergeTask->nProcessedChunks != pMergeTask->nChunks) {
                return;
            }

            std::unique_ptr<PackageProcessedImage> pProcessedImage = std::make_unique<PackageProcessedImage>();
            pProcessedImage->id = pMergeTask->id;
            pProcessedImage->pInputImage = pMergeTask->pInputImage;
            pProcessedImage->pGrayImage  = pMergeTask->pGrayImage;
            pProcessedImage->L           = std::move(pMergeTask->L);
            idToTask.erase(pMergeTask->id);
            assert(pMergeTask.use_count() == 1);

            pTask->GetOutputNode("ProcessedImages")->Push(std::move(pProcessedImage));
        }
    ).SetThreadsLimit(1);
    
    Node nodeOutput("Output");
    executor.AddTask("task_Filter", &nodeProcessedImages, &nodeOutput,
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageProcessedImage> pProcessedImage = uniquePtrCast<PackageProcessedImage>(std::move(packageIn));
    
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
            memcpy(result.pixels.data(), pProcessedImage->pInputImage->pixels.data(), result.pixels.size() * sizeof(Pixel24bit));
            int counter = 0;

            const int widthToProcess = (result.header.width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));
    
            for (uint32_t i = 0; i < result.header.height; ++i) {
                for (uint32_t j = 0; j < result.header.width; ++j) {
                    uint32_t pixelIdx = i * result.header.width + j;
                    if (i < PATTERN_MAX_SIZE / 2 || i >= (result.header.height - PATTERN_MAX_SIZE / 2) || j < PATTERN_MAX_SIZE / 2 || j >= (result.header.width - PATTERN_MAX_SIZE / 2)) {
                        continue;
                    }
                    
                    uint32_t pixelProcessIdx = (i - PATTERN_MAX_SIZE / 2) * widthToProcess + (j - PATTERN_MAX_SIZE / 2);
                    if (L_limit > pProcessedImage->L[pixelProcessIdx]) {
                        continue;
                    }
                    
                    // draw square around point
                    const int SQUARE_RADIUS = PATTERN_MAX_SIZE / 2;
                    for (int k = -SQUARE_RADIUS; k < SQUARE_RADIUS; ++k) {
                        result(i - SQUARE_RADIUS, j + k).Set(UINT8_MAX, 0, 0);
                        result(i + SQUARE_RADIUS, j - k).Set(UINT8_MAX, 0, 0);
                        result(i + k, j + SQUARE_RADIUS).Set(UINT8_MAX, 0, 0);
                        result(i - k, j - SQUARE_RADIUS).Set(UINT8_MAX, 0, 0);
                    }
                }
            }
    
            result.SaveTo("output\\result.tga");
        }
    );
    
    executor.ExecuteAndAwait();
    executor.PrintDebugData("output\\debugData.out");
    executor.Terminate();

#endif // USE_SINGLE_THREAD

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}
