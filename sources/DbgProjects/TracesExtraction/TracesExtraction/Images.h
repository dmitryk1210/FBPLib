// Images.h
//

#pragma once

#include <fstream>
#include <numeric>
#include <vector>

#pragma pack(push, 1)
struct TGAHeader {
    uint8_t  idlength;
    uint8_t  colourmaptype;
    uint8_t  datatypecode;
    uint16_t colourmaporigin;
    uint16_t colourmaplength;
    uint8_t  colourmapdepth;
    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsperpixel;
    uint8_t  imagedescriptor;
};
#pragma pack(pop)

struct Pixel24bit {
    uint8_t blue;
    uint8_t green;
    uint8_t red;

    inline void Set(uint8_t r, uint8_t g, uint8_t b) { red = r; green = g; blue = b; }
};

template <typename T>
struct TGAImage {
    TGAHeader header;
    std::vector<T> pixels;

    T& operator()(size_t i, size_t j) {
        return pixels[i * header.width + j];
    }

    const T& operator()(size_t i, size_t j) const {
        return pixels[i * header.width + j];
    }

    int SaveTo  (const std::string& filename);
    int LoadFrom(const std::string& filename);
};

template <typename T>
struct PGMImage {
    size_t width;
    size_t height;
    std::vector<T> pixels;

    void AddContrastFilter(int correction);

    int SaveTo(const std::string& filename);
};


template <typename T>
int TGAImage<T>::SaveTo(const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "!\n" << std::flush;
        return 1;
    }

    outFile.write(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
    outFile.write(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(T));
    outFile.close();
    return 0;
}


template <typename T>
int TGAImage<T>::LoadFrom(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "!\n" << std::flush;
        return 1;
    }

    inFile.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));

    if (header.bitsperpixel != sizeof(T) * 8) {
        std::cerr << "Error: Only " << sizeof(T) * 8 << "-bit images are supported.\n" << std::flush;
        return 1;
    }

    pixels.resize(static_cast<size_t>(header.width) * header.height);

    inFile.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(T));
    inFile.close();

    return 0;
}


template <typename T>
void PGMImage<T>::AddContrastFilter(int correction)
{
    std::vector<size_t> pixelsCount(UINT16_MAX + 1, 0);
    for (const T& pixel : this->pixels) {
        pixelsCount[pixel] += 1;
    }
    size_t totalPixels = this->pixels.size();
    double averageColor = 0;
    for (size_t i = 0; i < pixelsCount.size(); ++i) {
        averageColor += static_cast<double>(pixelsCount[i]) / totalPixels * i;
    }

    std::vector<T> vecCorrected(UINT16_MAX + 1, 0);
    double k = 1.0 + correction / 100.0;
    for (size_t clr = 0; clr < vecCorrected.size(); ++clr) {
        vecCorrected[clr] = static_cast<T>(std::clamp<int>(averageColor + k * (clr - averageColor), 0, UINT16_MAX));
    }

    for (T& pixel : this->pixels) {
        pixel = vecCorrected[pixel];
    }
}


template <typename T>
int PGMImage<T>::SaveTo(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return 1;
    }

    // PGM header
    file << "P5\n" << width << " " << height << "\n" << UINT16_MAX << "\n";

    for (int i = 0; i < pixels.size(); ++i) {
        uint8_t highByte = pixels[i] >> 8;
        uint8_t lowByte  = pixels[i] & 0xFF;
        file.put(highByte);
        file.put(lowByte);
    }

    file.close();
    if (!file) {
        std::cerr << "Error: Could not write to file properly." << std::endl;
        return 1;
    }
    return 0;
}
