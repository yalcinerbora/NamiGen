#pragma once

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "NamiGenOptions.h"

constexpr float NAMI_PI = 3.1415927f;
constexpr float NAMI_E = 2.7182817f;

static float SecH(float radians);
static float CircleSampleSinusodial(float distance, const NamiGenOptions& opts);
static float CircleSampleLinear(float distance, const NamiGenOptions& opts);
static float SampleSinusodial(float norm, const NamiGenOptions& opts);
static float SampleLinear(float norm, const NamiGenOptions& opts);

inline static float CircleSample(int x, int y, const NamiGenOptions& opts)
{
    // Float casted points
    float centerPointX = static_cast<float>(opts.sizeX) * 0.5f;
    float centerPointY = static_cast<float>(opts.sizeY) * 0.5f;
    float xFloat = static_cast<float>(x);
    float yFloat = static_cast<float>(y);

    // Distance
    float distance = std::sqrtf(std::abs(xFloat - centerPointX) *
                                std::abs(xFloat - centerPointX) +
                                std::abs(yFloat - centerPointY) *
                                std::abs(yFloat - centerPointY));

    // Edge Cases
    if(distance > static_cast<float>(opts.gapTop / 2))
        return opts.zLand;
    else if(distance < static_cast<float>(opts.gapBottom / 2))
        return opts.zBottom;

    if(opts.type == NamiGenType::CIRCULAR_SINUSODIAL)
        return CircleSampleSinusodial(distance, opts);
    else
        return CircleSampleLinear(distance, opts);
}

inline static float CircleSampleSinusodial(float distance, const NamiGenOptions& opts)
{
    distance -= static_cast<float>(opts.gapBottom / 2); 
    distance /= static_cast<float>(opts.gapTop / 2 - opts.gapBottom / 2);
    distance = 1.0f - distance;
    distance *= NAMI_PI;
    distance -= NAMI_PI;

    // Distance is reversed and in between [0, PI/2]
    // Sample sin function
    float bathy = std::cos(distance) * 0.5f + 0.5f;
    bathy *= opts.zBottom - opts.zLand;
    bathy += opts.zLand;
    return bathy;
}

inline static float CircleSampleLinear(float distance, const NamiGenOptions& opts)
{
    distance -= static_cast<float>(opts.gapBottom / 2);
    distance /= static_cast<float>(opts.gapTop / 2 - opts.gapBottom / 2);
    distance = 1.0f - distance;
    
    // Distance is reversed and in between [0, 1]
    // Sample linearly
    float bathy = distance;
    bathy *= (opts.zBottom - opts.zLand);
    bathy += opts.zLand;
    return bathy;
}

inline static float SampleFlat(int x, int y, const NamiGenOptions& opts)
{
    float value;
    switch(opts.type)
    {
        case NamiGenType::LINEAR_L:
        case NamiGenType::SINUSODIAL_L:
            value = static_cast<float>(opts.sizeX - 1 - x);
            break;
        case NamiGenType::LINEAR_R:
        case NamiGenType::SINUSODIAL_R:
            value = static_cast<float>(x);
            break;
        case NamiGenType::LINEAR_T:
        case NamiGenType::SINUSODIAL_T:
            value = static_cast<float>(opts.sizeY - 1 - y);
            break;
        case NamiGenType::LINEAR_B:
        case NamiGenType::SINUSODIAL_B:
            value = static_cast<float>(y);
            break;
    }
    if(value > opts.gapTop)
        return opts.zLand;
    else if(value < opts.gapBottom)
        return opts.zBottom;

    float normDist = static_cast<float>(value);
    normDist -= opts.gapBottom;
    normDist /= (opts.gapTop - opts.gapBottom);
    normDist = 1.0f - normDist;

    if(opts.type == NamiGenType::LINEAR_L ||
       opts.type == NamiGenType::LINEAR_R ||
       opts.type == NamiGenType::LINEAR_T ||
       opts.type == NamiGenType::LINEAR_B)
    {
        return SampleLinear(normDist, opts);
    }
    else if(opts.type == NamiGenType::SINUSODIAL_L ||
            opts.type == NamiGenType::SINUSODIAL_R ||
            opts.type == NamiGenType::SINUSODIAL_T ||
            opts.type == NamiGenType::SINUSODIAL_B)
    {
        return SampleSinusodial(normDist, opts);
    }
    return 0.0f;
}

inline static float SampleLinear(float norm, const NamiGenOptions& opts)
{
    float bathy = norm;
    bathy *= (opts.zBottom - opts.zLand);
    bathy += opts.zLand;
    return bathy;
}

inline static float SampleSinusodial(float norm, const NamiGenOptions& opts)
{
    norm *= NAMI_PI;
    norm -= NAMI_PI;

    // Distance is reversed and in between [0, PI/2]
    // Sample sin function
    float bathy = std::cos(norm) * 0.5f + 0.5f;
    bathy *= opts.zBottom - opts.zLand;
    bathy += opts.zLand;
    return bathy;
}

inline static float WaveSample(int x, int y, const NamiGenOptions& opts)
{
    float centerDist;
    float H = std::abs(opts.zLand);
    float d = opts.zBottom;
    switch(opts.type)
    {
        case NamiGenType::WAVE_CIRCULAR:
        {
            float xDist = static_cast<float>(x - opts.gapBottom);
            float yDist = static_cast<float>(y - opts.gapTop);
            centerDist = std::sqrt(xDist * xDist + yDist * yDist);
            break;
        }
        case NamiGenType::WAVE_HORIZONTAL:
        {
            centerDist = static_cast<float>(y - opts.gapBottom);
            break;
        }
        case NamiGenType::WAVE_VERTICAL:
        {
            centerDist = static_cast<float>(x - opts.gapBottom);
            break;
        }
        case NamiGenType::WAVE_EMPTY:
            return 0.0f;
    }
    float coshTerm = std::cosh((std::sqrt(0.75f * H / d / d / d) * centerDist));
    return H / (coshTerm * coshTerm);
}

// Out Functions
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
    for(int i = 0; i < opts.sizeX * opts.sizeY; i++)
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
    outFile.write(reinterpret_cast<const char*>(&opts.latMin), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&opts.latMax), sizeof(double));

    outFile.write(reinterpret_cast<const char*>(&min), sizeof(double));
    outFile.write(reinterpret_cast<const char*>(&max), sizeof(double));

    // Writing Floats
    const float* dataPtr = data;
    for(int i = 0; i < opts.sizeX * opts.sizeY; i++)
    {
        outFile.write(reinterpret_cast<const char*>(dataPtr), sizeof(float));
        dataPtr += 1;
    }
    printf("%s MM(%f, %f)\n", fileName.c_str(), min, max);
}