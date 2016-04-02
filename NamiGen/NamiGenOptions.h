#pragma once

enum class NamiGenType
{
    INVALID,
    CIRCULAR_SINUSODIAL,
    CIRCULAR_LINEAR,

    // Linear
    LINEAR_R,
    LINEAR_L,
    LINEAR_T,
    LINEAR_B,

    // Sinusodial
    SINUSODIAL_R,
    SINUSODIAL_L,
    SINUSODIAL_T,
    SINUSODIAL_B,

    // Waves (using this formulate)
    WAVE_CIRCULAR,
    WAVE_HORIZONTAL,
    WAVE_VERTICAL
};

enum class NamiGenOut
{
    INVALID,
    GRD,
    GRD_BIN
};

struct NamiGenOptions
{
    double latMin, latMax;
    double lonMin, lonMax;
    int sizeX, sizeY;
    int gapOut, gapIn;
    float zMin, zMax;
    NamiGenOut output;
    NamiGenType type;
    bool hasWalls;
    int wallWidth;
};