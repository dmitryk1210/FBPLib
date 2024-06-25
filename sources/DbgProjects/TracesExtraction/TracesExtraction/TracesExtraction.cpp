// TracesExtraction.cpp
//

#include "TracesExtraction.h"

#ifndef USE_SINGLE_THREAD

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

#include "CudaFunctions.h"


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
    uint32_t           id = 0;
};

struct PackageProcessedImageWithGroups : public PackageProcessedImage
{
    std::vector<std::vector<FragmentInfo>> tracks;
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
    uint16_t              nChunks = 0;
};

struct PackageImageChunk : public PackageMerge
{
    PackageMergeTask* pMergeTask = nullptr;
    uint32_t              pxlFrom = 0;
    uint32_t              pxlTo = 0;
    std::vector<uint32_t> K_chunk;
    std::vector<float>    L_chunk;
};

struct PackageSaveToFile : public fbp::PackageBase
{
    std::string filename;
    std::shared_ptr<TGAImage<Pixel24bit>> pTGAImage;
    std::shared_ptr<PGMImage<PixelType>>  pPGMImage;
};


class FunctorGroupFragments {
public:
    void operator()(fbp::uptr_PackageBase packageIn, fbp::Task* pTask) {
        std::unique_ptr<PackageProcessedImage> pProcessedImage = fbp::uniquePtrCast<PackageProcessedImage>(std::move(packageIn));

        const float G_max = PATTERN_MAX_SIZE * 2;
        const float F_min = 0.f;
        const float p_max = PATTERN_MAX_SIZE * 2;

        uint32_t height = pProcessedImage->pInputImage->header.height;
        uint32_t width = pProcessedImage->pInputImage->header.width;

        std::vector<FragmentInfo> fragments;
        for (uint32_t i = 0; i < height; ++i) {
            for (uint32_t j = 0; j < width; ++j) {
                uint32_t pixelIdx = i * width + j;
                if (pProcessedImage->L[pixelIdx] > 1e-6) {
                    fragments.emplace_back(i, j, pProcessedImage->L[pixelIdx], false);
                }
            }
        }

        std::vector<std::vector<FragmentInfo>> tracks;

        for (uint32_t i = 0; i < fragments.size(); ++i) {
            if (fragments[i].used) {
                continue;
            }

            FragmentInfo& startFragment = fragments[i];
            fragments[i].used = true;
            std::vector<FragmentInfo> currentTrack;
            currentTrack.push_back(startFragment);

            // Find the closest unused fragment to startFragment
            FragmentInfo* closestFragmentPtr = nullptr;
            float minDistance = G_max;

            for (auto& frag : fragments) {
                if (!frag.used) {
                    float distance = std::sqrt(std::pow(frag.i - startFragment.i, 2) + std::pow(frag.j - startFragment.j, 2));
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestFragmentPtr = &frag;
                    }
                }
            }

            if (closestFragmentPtr == nullptr) {
                continue;
            }

            FragmentInfo& nextFragment = *closestFragmentPtr;
            nextFragment.used = true;
            currentTrack.push_back(nextFragment);

            // Calculate line equation parameters: y = kx + b
            float k = static_cast<float>(nextFragment.j - startFragment.j) / (nextFragment.i - startFragment.i);
            float b = startFragment.j - k * startFragment.i;

            // Find all candidates within p_max distance to the line
            std::vector<FragmentInfo*> candidates;
            for (auto& frag : fragments) {
                if (!frag.used) {
                    float distanceToLine = std::abs(k * frag.i - frag.j + b) / std::sqrt(k * k + 1);
                    if (distanceToLine < p_max) {
                        candidates.push_back(&frag);
                    }
                }
            }

            bool added = true;
            while (added) {
                added = false;
                for (auto it = candidates.begin(); it != candidates.end();) {
                    FragmentInfo* candidate = *it;
                    bool closeToTrack = false;
                    for (const auto& trackFragment : currentTrack) {
                        float distanceToTrack = std::sqrt(std::pow(candidate->i - trackFragment.i, 2) + std::pow(candidate->j - trackFragment.j, 2));
                        if (distanceToTrack < G_max) {
                            closeToTrack = true;
                            break;
                        }
                    }
                    if (closeToTrack) {
                        candidate->used = true;
                        currentTrack.push_back(*candidate);
                        it = candidates.erase(it);
                        added = true;
                    }
                    else {
                        ++it;
                    }
                }
            }

            // Check if the density and distance criteria are met for the current track
            // float density = static_cast<float>(currentTrack.size()) / (height * width);
            bool validTrack = true;// density >= F_min;

            if (validTrack) {
                tracks.push_back(currentTrack);
            }
            else {
                for (auto& frag : currentTrack) {
                    fragments[frag.i * width + frag.j].used = false;
                }
            }
        }

        std::unique_ptr<PackageProcessedImageWithGroups> pOutput = std::make_unique<PackageProcessedImageWithGroups>();
        pOutput->pInputImage = std::move(pProcessedImage->pInputImage);
        pOutput->pGrayImage = std::move(pProcessedImage->pGrayImage);
        pOutput->id = std::move(pProcessedImage->id);
        pOutput->L = std::move(pProcessedImage->L);
        pOutput->tracks = std::move(tracks);

        pTask->GetOutputNode()->Push(std::move(pOutput));
    }
};


void DrawImage(fbp::uptr_PackageBase packageIn, fbp::Task* pTask)
{
    std::unique_ptr<PackageProcessedImageWithGroups> pProcessedImage = fbp::uniquePtrCast<PackageProcessedImageWithGroups>(std::move(packageIn));

    std::shared_ptr<TGAImage<Pixel24bit>> pResult = std::make_shared<TGAImage<Pixel24bit>>();
    TGAImage<Pixel24bit>& result = *pResult;
    result.header = pProcessedImage->pInputImage->header;
    result.pixels.resize(result.header.width * result.header.height);
    memcpy(result.pixels.data(), pProcessedImage->pInputImage->pixels.data(), result.pixels.size() * sizeof(Pixel24bit));

    for (uint32_t i = 0; i < result.header.height; ++i) {
        for (uint32_t j = 0; j < result.header.width; ++j) {
            uint32_t pixelIdx = i * result.header.width + j;
            if (pProcessedImage->L[pixelIdx] <= 1e-6) {
                continue;
            }

            const int SQUARE_RADIUS = PATTERN_MAX_SIZE / 2;
            for (int k = -SQUARE_RADIUS; k < SQUARE_RADIUS; ++k) {
                result(i - SQUARE_RADIUS, j + k).Set(UINT8_MAX, 0, 0);
                result(i + SQUARE_RADIUS, j - k).Set(UINT8_MAX, 0, 0);
                result(i + k, j + SQUARE_RADIUS).Set(UINT8_MAX, 0, 0);
                result(i - k, j - SQUARE_RADIUS).Set(UINT8_MAX, 0, 0);
            }
        }
    }

    for (const auto& track : pProcessedImage->tracks) {
        if (track.empty()) continue;

        // Find the bounding box of the group
        uint32_t min_i = track.front().i;
        uint32_t max_i = track.front().i;
        uint32_t min_j = track.front().j;
        uint32_t max_j = track.front().j;

        for (const auto& frag : track) {
            if (frag.i < min_i) min_i = frag.i;
            if (frag.i > max_i) max_i = frag.i;
            if (frag.j < min_j) min_j = frag.j;
            if (frag.j > max_j) max_j = frag.j;
        }

        // Extend the bounding box by PATTERN_MAX_SIZE
        min_i = (min_i > PATTERN_MAX_SIZE) ? min_i - PATTERN_MAX_SIZE : 0;
        max_i = (max_i + PATTERN_MAX_SIZE < result.header.height) ? max_i + PATTERN_MAX_SIZE : result.header.height - 1;
        min_j = (min_j > PATTERN_MAX_SIZE) ? min_j - PATTERN_MAX_SIZE : 0;
        max_j = (max_j + PATTERN_MAX_SIZE < result.header.width) ? max_j + PATTERN_MAX_SIZE : result.header.width - 1;

        // Draw blue rectangle around the group
        for (uint32_t k = min_j; k <= max_j; ++k) {
            result(min_i, k).Set(0, UINT8_MAX, 0);
            result(max_i, k).Set(0, UINT8_MAX, 0);
        }
        for (uint32_t k = min_i; k <= max_i; ++k) {
            result(k, min_j).Set(0, UINT8_MAX, 0);
            result(k, max_j).Set(0, UINT8_MAX, 0);
        }
    }

    std::unique_ptr<PackageSaveToFile> pSaveToFile = std::make_unique<PackageSaveToFile>();
    pSaveToFile->filename = "output\\result";
    pSaveToFile->filename += std::to_string(pProcessedImage->id) + ".tga";
    pSaveToFile->pTGAImage = pResult;
    pTask->GetOutputNode("Save")->Push(std::move(pSaveToFile));
}


int main()
{
    using namespace fbp;

    Executor executor;

    auto start = std::chrono::high_resolution_clock::now();

    executor.AddNode("Input", true);

    for (int id = 0; id < N_ITERATIONS; ++id) {
        std::shared_ptr<TGAImage<Pixel24bit>> pInputImage = std::make_shared<TGAImage<Pixel24bit>>();
        if (pInputImage->LoadFrom("example02.tga")) {
            assert(false);
            return -1;
        }
        executor.GetNode("Input").Push(std::make_unique<PackageInput>(pInputImage, id));
    }
    executor.GetNode("Input").Push(fbp::MakePackageEndOfStream());

    executor.AddNode("Gray");
    executor.AddNode("Save");
    executor.AddTask("task_MakeGray", "Input", std::vector<std::string>{"Gray", "Save"},
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageInput> pInputImage = uniquePtrCast<PackageInput>(std::move(packageIn));
            std::unique_ptr<PackageGrayImage> pGrayImage = std::make_unique<PackageGrayImage>();

            pGrayImage->pInputImage = pInputImage->pImage;
            pGrayImage->pGrayImage = std::make_shared<PGMImage<PixelType>>();
            pGrayImage->id = pInputImage->id;
            const TGAImage<Pixel24bit>& inputImage = *(pGrayImage->pInputImage);
            PGMImage<PixelType>& imageGray = *(pGrayImage->pGrayImage);

            uint32_t imageSize = static_cast<uint32_t>(inputImage.header.width) * inputImage.header.height;

            imageGray.pixels.resize(imageSize);
            imageGray.width = inputImage.header.width;
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

    executor.AddNode("Output");
    executor.AddTask("task_Save", "Save", "Output",
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
    ).SetThreadsLimit(6);

    executor.AddNode("Contrast");
    executor.AddTask("task_MakeContrast", "Gray", std::vector<std::string>{"Contrast", "Save"},
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageGrayImage> pGrayImage = uniquePtrCast<PackageGrayImage>(std::move(packageIn));
            auto pGrayImageOld = pGrayImage->pGrayImage;
            pGrayImage->pGrayImage = std::make_shared<PGMImage<PixelType>>();
            *(pGrayImage->pGrayImage) = *pGrayImageOld;
            pGrayImage->pGrayImage->AddContrastFilter(50);

            std::unique_ptr<PackageSaveToFile> pSaveToFile = std::make_unique<PackageSaveToFile>();
            pSaveToFile->filename = "output\\grayContrast.pgm";
            pSaveToFile->pPGMImage = pGrayImage->pGrayImage;
            pTask->GetOutputNode("Save")->Push(std::move(pSaveToFile));

            pTask->GetOutputNode("Contrast")->Push(std::move(pGrayImage));
        }
    );

    executor.AddNode("ChunksToProcess");
    executor.AddNode("MergeChunks");
    executor.AddTask("task_SliceChunks", "Contrast", std::vector<std::string>{"ChunksToProcess", "MergeChunks"},
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
#ifdef USE_CUDA
            const int CHUNKS_PER_IMAGE = 1;
#else
            const int CHUNKS_PER_IMAGE = 32;
#endif // USE_CUDA
            std::unique_ptr<PackageMergeTask> pMergeTask = std::make_unique<PackageMergeTask>();

            {
                std::unique_ptr<PackageGrayImage> pGrayImage = uniquePtrCast<PackageGrayImage>(std::move(packageIn));
                pMergeTask->pInputImage = pGrayImage->pInputImage;
                pMergeTask->pGrayImage = pGrayImage->pGrayImage;
                pMergeTask->id = pGrayImage->id;
                pMergeTask->nChunks = CHUNKS_PER_IMAGE;
            }

            uint32_t pixelsToProcess = (pMergeTask->pGrayImage->height - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u)) * (pMergeTask->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            pMergeTask->K.resize(pixelsToProcess);
            pMergeTask->L.resize(pixelsToProcess);

            uint32_t nextChunkStartPxl = 0;
            for (int i = 0; i < CHUNKS_PER_IMAGE; ++i) {
                std::unique_ptr<PackageImageChunk> pChunk = std::make_unique<PackageImageChunk>();
                pChunk->id = pMergeTask->id;
                pChunk->pGrayImage = pMergeTask->pGrayImage;
                pChunk->pMergeTask = &(*pMergeTask);
                pChunk->pxlFrom = nextChunkStartPxl;
                nextChunkStartPxl = pixelsToProcess / CHUNKS_PER_IMAGE * (i + 1);
                pChunk->pxlTo = (i == CHUNKS_PER_IMAGE - 1 ? pixelsToProcess : nextChunkStartPxl);
                pChunk->K_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pChunk->L_chunk.resize(pChunk->pxlTo - pChunk->pxlFrom);
                pTask->GetOutputNode("ChunksToProcess")->Push(std::move(pChunk));
            }
            pTask->GetOutputNode("MergeChunks")->Push(std::move(pMergeTask));
        }
    ).SetThreadsLimit(1);

    executor.AddTaskCuda("task_ProcessChunks", "ChunksToProcess", "MergeChunks",
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageImageChunk> pChunk = uniquePtrCast<PackageImageChunk>(std::move(packageIn));
            const size_t widthToProcess = (pChunk->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            PatternsLibrary lib;

            ProcessImageCuda(pChunk->pGrayImage->pixels.data(), pChunk->pGrayImage->height, pChunk->pGrayImage->width, pChunk->pxlFrom, pChunk->pxlTo, pChunk->K_chunk.data(), pChunk->L_chunk.data(), pChunk->L_chunk.size(), &lib);

            pTask->GetOutputNode("MergeChunks")->Push(std::move(pChunk));
        },
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageImageChunk> pChunk = uniquePtrCast<PackageImageChunk>(std::move(packageIn));
            const size_t widthToProcess = (pChunk->pGrayImage->width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            PatternsLibrary lib;

            for (uint32_t pxl = pChunk->pxlFrom; pxl < pChunk->pxlTo; ++pxl) {
                uint16_t i = PATTERN_MAX_SIZE / 2 + pxl / widthToProcess;
                uint16_t j = PATTERN_MAX_SIZE / 2 + pxl % widthToProcess;

                ProcessPixel(reinterpret_cast<PixelType*>(pChunk->pGrayImage->pixels.data()), pChunk->pGrayImage->width, i, j, &(pChunk->K_chunk[pxl - pChunk->pxlFrom]), &(pChunk->L_chunk[pxl - pChunk->pxlFrom]), &lib);
            }

            pTask->GetOutputNode("MergeChunks")->Push(std::move(pChunk));
        }
    );

    executor.AddNode("ProcessedImages");
    executor.AddTask("task_MergeChunks", "MergeChunks", "ProcessedImages",
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
            memcpy(pMergeTask->L.data() + pChunk->pxlFrom, pChunk->L_chunk.data(), (pChunk->pxlTo - pChunk->pxlFrom) * sizeof(float));

            pChunk.reset();

            pMergeTask->nProcessedChunks += 1;
            if (pMergeTask->nProcessedChunks != pMergeTask->nChunks) {
                return;
            }

            std::unique_ptr<PackageProcessedImage> pProcessedImage = std::make_unique<PackageProcessedImage>();
            pProcessedImage->id = pMergeTask->id;
            pProcessedImage->pInputImage = pMergeTask->pInputImage;
            pProcessedImage->pGrayImage = pMergeTask->pGrayImage;
            pProcessedImage->L = std::move(pMergeTask->L);
            idToTask.erase(pMergeTask->id);
            assert(pMergeTask.use_count() == 1);

            pTask->GetOutputNode("ProcessedImages")->Push(std::move(pProcessedImage));
        }
    );

    executor.AddNode("Group");
    executor.AddTask("task_Filter", "ProcessedImages", "Group",
        [](uptr_PackageBase packageIn, fbp::Task* pTask)
        {
            std::unique_ptr<PackageProcessedImage> pProcessedImage = uniquePtrCast<PackageProcessedImage>(std::move(packageIn));

            uint32_t height = pProcessedImage->pInputImage->header.height;
            uint32_t width = pProcessedImage->pInputImage->header.width;

            float L_limit = 0;
            {
                const int LIMIT = 200;
                std::vector<float> L_ordered = pProcessedImage->L;
                std::partial_sort(L_ordered.begin(), L_ordered.begin() + LIMIT + 1, L_ordered.end(), std::greater{});
                L_limit = L_ordered[LIMIT];
            }

            const int widthToProcess = (width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

            std::vector<float> L_old;
            L_old.resize(height * width);
            std::swap(L_old, pProcessedImage->L);

            for (uint32_t i = 0; i < height; ++i) {
                for (uint32_t j = 0; j < width; ++j) {
                    uint32_t pixelIdx = i * width + j;
                    if (i < PATTERN_MAX_SIZE / 2 || i >= (height - PATTERN_MAX_SIZE / 2) || j < PATTERN_MAX_SIZE / 2 || j >= (width - PATTERN_MAX_SIZE / 2)) {
                        pProcessedImage->L[pixelIdx] = 0.f;
                        continue;
                    }

                    uint32_t pixelProcessIdx = (i - PATTERN_MAX_SIZE / 2) * widthToProcess + (j - PATTERN_MAX_SIZE / 2);
                    if (L_limit >= L_old[pixelProcessIdx]) {
                        pProcessedImage->L[pixelIdx] = 0.f;
                        continue;
                    }
                    pProcessedImage->L[pixelIdx] = L_old[pixelProcessIdx];
                }
            }

            pTask->GetOutputNode()->Push(std::move(pProcessedImage));
        }
    );

    executor.AddNode("Draw");
    executor.AddTask("task_Group", "Group", "Draw", FunctorGroupFragments());

    executor.AddTask("task_Draw", "Draw", "Save", DrawImage);

    executor.ExecuteAndAwait();
    executor.PrintDebugData("output\\debugData.out");

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}
#endif // !USE_SINGLE_THREAD
