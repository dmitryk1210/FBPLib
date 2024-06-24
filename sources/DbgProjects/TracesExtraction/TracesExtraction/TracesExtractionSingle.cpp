// TracesExtractionSingle.cpp
//

#include "TracesExtraction.h"

#ifdef USE_SINGLE_THREAD

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

#include "Images.h"
#include "PatternsLibrary.h"


std::vector<float> ProcessImage(PGMImage<PixelType>& image)
{
    PatternsLibrary lib;
    lib.Init();

    const size_t heightToProcess = (image.height - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));
    const size_t widthToProcess  = (image.width  - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

    std::vector<uint32_t> K(heightToProcess * widthToProcess, 0);
    std::vector<float>    L(heightToProcess * widthToProcess, 0.f);

    
    for (uint32_t i = PATTERN_MAX_SIZE / 2; i < image.height - PATTERN_MAX_SIZE / 2; ++i) {
        for (uint32_t j = PATTERN_MAX_SIZE / 2; j < image.width - PATTERN_MAX_SIZE / 2; ++j) {
            uint32_t pixelProcessIdx = (i - PATTERN_MAX_SIZE / 2) * widthToProcess + (j - PATTERN_MAX_SIZE / 2);
            ProcessPixel(reinterpret_cast<PixelType*>(image.pixels.data()), image.width, i, j, &K[pixelProcessIdx], &L[pixelProcessIdx], lib);
        }
    }
    return L;
}


int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    TGAImage<Pixel24bit> inputImage;
    if (inputImage.LoadFrom("example02.tga")) {
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

    uint32_t height = imageGray.height;
    uint32_t width = imageGray.width;

    float L_limit = 0;
    {
        const int LIMIT = 200;
        std::vector<float> L_ordered = L;
        std::partial_sort(L_ordered.begin(), L_ordered.begin() + LIMIT, L_ordered.end(), std::greater{});
        L_limit = L_ordered[LIMIT - 1];
    }

    const int widthToProcess = (width - PATTERN_MAX_SIZE + (PATTERN_MAX_SIZE & 0x01u));

    std::vector<float> L_old;
    L_old.resize(height * width);
    std::swap(L_old, L);

    for (uint32_t i = 0; i < height; ++i) {
        for (uint32_t j = 0; j < width; ++j) {
            uint32_t pixelIdx = i * width + j;
            if (i < PATTERN_MAX_SIZE / 2 || i >= (height - PATTERN_MAX_SIZE / 2) || j < PATTERN_MAX_SIZE / 2 || j >= (width - PATTERN_MAX_SIZE / 2)) {
                L[pixelIdx] = 0.f;
                continue;
            }

            uint32_t pixelProcessIdx = (i - PATTERN_MAX_SIZE / 2) * widthToProcess + (j - PATTERN_MAX_SIZE / 2);
            if (L_limit > L_old[pixelProcessIdx]) {
                L[pixelIdx] = 0.f;
                continue;
            }
            L[pixelIdx] = L_old[pixelProcessIdx];
        }
    }


    const float G_max = PATTERN_MAX_SIZE * 2;
    const float F_min = 0.f;
    const float p_max = PATTERN_MAX_SIZE * 2;

    std::vector<FragmentInfo> fragments;
    for (uint32_t i = 0; i < height; ++i) {
        for (uint32_t j = 0; j < width; ++j) {
            uint32_t pixelIdx = i * width + j;
            if (L[pixelIdx] > 1e-6) {
                fragments.emplace_back(i, j, L[pixelIdx], false);
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


    TGAImage<Pixel24bit> result;
    result.header = inputImage.header;
    result.pixels.resize(result.header.width * result.header.height);
    memcpy(result.pixels.data(), inputImage.pixels.data(), result.pixels.size() * sizeof(Pixel24bit));

    for (uint32_t i = 0; i < result.header.height; ++i) {
        for (uint32_t j = 0; j < result.header.width; ++j) {
            uint32_t pixelIdx = i * result.header.width + j;
            if (L[pixelIdx] <= 1e-6) {
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

    for (const auto& track : tracks) {
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
    result.SaveTo("output\\result.tga");

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "time: " << duration.count() << " ms\n";

    return 0;
}
#endif // USE_SINGLE_THREAD
