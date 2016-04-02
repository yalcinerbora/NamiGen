#pragma once

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "NamiGenOptions.h"

constexpr float NAMI_PI = 3.1415927f;

static float CircleSampleSinusodial(float distance, const NamiGenOptions& opts);
static float CircleSampleLinear(float distance, const NamiGenOptions& opts);

inline static float CircleSample(int x, int y, const NamiGenOptions& opts)
{
    // Float casted points
    float centerPointX = static_cast<float>(opts.sizeX) * 0.5f;
    float centerPointY = static_cast<float>(opts.sizeY) * 0.5f;
    float xFloat = static_cast<float>(opts.sizeX);
    float yFloat = static_cast<float>(opts.sizeY);

    // Distance
    float distance = std::sqrtf(std::abs(xFloat - centerPointX) *
                                std::abs(xFloat - centerPointX) +
                                std::abs(yFloat - centerPointY) *
                                std::abs(yFloat - centerPointY));

    // Edge Cases
    if(distance <= opts.gapIn)
        return opts.zMax;
    else if(distance >= opts.gapOut)
        return opts.zMin;

    if(opts.type == NamiGenType::CIRCULAR_SINUSODIAL)
        return CircleSampleSinusodial(distance, opts);
    else if(opts.type == NamiGenType::CIRCULAR_LINEAR)
        return CircleSampleLinear(distance, opts);
}

inline static float CircleSampleSinusodial(float distance, const NamiGenOptions& opts)
{
    distance -= static_cast<float>(opts.gapIn) / static_cast<float>(opts.gapOut);
    distance = 1.0f - distance;
    distance *= NAMI_PI * 0.5f;

    // Distance is reversed and in between [0, PI/2]
    // Sample sin function
    float bathy = std::sin(distance);
    bathy *= opts.zMax - opts.zMin;
    bathy += opts.zMin;
    return bathy;
}

inline static float CircleSampleLinear(float distance, const NamiGenOptions& opts)
{
    distance -= static_cast<float>(opts.gapIn) / static_cast<float>(opts.gapOut);
    distance = 1.0f - distance;
    
    // Distance is reversed and in between [0, 1]
    // Sample linearly
    float bathy = distance;
    bathy *= opts.zMax - opts.zMin;
    bathy += opts.zMin;
    return bathy;
}

inline static float LinearSample(int x, int y, const NamiGenOptions& opts)
{

}

inline static float SinusodialSample(int x, int y, const NamiGenOptions& opts)
{

}

inline static float WaveSample(int x, int y, const NamiGenOptions& opts)
{

}

inline static int NumDecimalDigit(float f)
{
    int number = static_cast<int>(f);
    if(number == 0) return 1;
    int digits = 0;
    if(number < 0) digits = 1;
    while(number)
    {
        number /= 10;
        digits++;
    }
    return digits;
}

// Out Functions
inline static void OutGRD(const float* data,
                          const NamiGenOptions& opts,
                          double min, double max,
                          const std::string& fileName)
{
    static const std::string FOURCC_GRD = "DSAA";

    std::ofstream fileOut; 
    fileOut.open(fileName);
    fileOut << FOURCC_GRD << "\n";
    fileOut << opts.sizeX << "   " << opts.sizeY << "\n";
    fileOut.precision(7);
    fileOut.setf(std::ios::fixed);
    fileOut << opts.latMin << "   " << opts.latMax << "\n";
    fileOut << opts.lonMin << "   " << opts.lonMax << "\n";
    fileOut << min << "  " << max << "\n";

    const float* dataPtr = data;
    unsigned lineCounter = 1;
    for(unsigned int i = 0; i < opts.sizeX * opts.sizeY; i++)
    {
        fileOut << *dataPtr;
        if(i % opts.sizeX == opts.sizeX - 1)
        {
            fileOut << "\n";
            lineCounter = 0;
        }
        if(i % opts.sizeY == opts.sizeY - 1)
        {
            fileOut << "\n";
            lineCounter = 0;
        }
        else
        {
            if(lineCounter % 10 == 0)
            {
                fileOut << "\n";
            }
            else
            {
                static const int mostLeadingSpaces = 6;
                std::string spaces;
                int spaceCount = NumDecimalDigit(*dataPtr);
                spaceCount = std::max(1, mostLeadingSpaces - spaceCount);
                for(int j = 0; j < spaceCount; j++)
                    spaces += " ";
                fileOut << spaces;
            }
        }
        lineCounter++;
        dataPtr += 1;
    }
    printf("%s MM(%f, %f)\n", fileName.c_str(), min, max);
}

inline static void OutGRDBin(const float* data,
                             const NamiGenOptions& opts,
                             double min, double max,
                             const std::string& fileName)
{
    static const std::string FOURCC_GRD_BIN = "DSBB";

    std::ofstream outFile(fileName, std::ofstream::binary);

    // Writing Header
    outFile.write(FOURCC_GRD_BIN.c_str(), FOURCC_GRD_BIN.size());

    // TODO: Does endianness fuck up this code? (fetching short sized chunk from integer)
    outFile.write(reinterpret_cast<const char*>(&opts.sizeX), sizeof(unsigned short));
    outFile.write(reinterpret_cast<const char*>(&opts.sizeY), sizeof(unsigned short));

    outFile.write(reinterpret_cast<const char*>(&opts.lonMin), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&opts.lonMax), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&opts.latMax), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&opts.latMin), sizeof(double));

    outFile.write(reinterpret_cast<const char*>(&min), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&max), sizeof(double));

    // Writing Floats
    const float* dataPtr = data;
    for(unsigned int i = 0; i < opts.sizeX * opts.sizeY; i++)
    {
        outFile.write(reinterpret_cast<const char*>(dataPtr), sizeof(float));
        dataPtr += 1;
    }
    printf("%s MM(%f, %f)\n", fileName.c_str(), min, max);
}