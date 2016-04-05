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
    WAVE_VERTICAL,    
    WAVE_EMPTY
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
    int gapBottom, gapTop;
    float zLand, zBottom;
    NamiGenOut output;
    NamiGenType type;
    bool hasWalls;
    int wallWidth;
};

constexpr NamiGenOptions namiOptsDefault = NamiGenOptions
{
    0.0, 1.0,
    0.0, 1.0,
    256, 256,
    64, 240,
    -10.0f, 50.0f,
    NamiGenOut::GRD,
    NamiGenType::CIRCULAR_SINUSODIAL,
    false,
    3
};

static const std::vector<std::string> switches =
{
    "-t",
    "-o",
    "-lat",
    "-lon",
    "-size",
    "-gap",
    "-z",
    "-wall",
    "-w",
    "-n"
};

static const std::vector<int> switchArgCounts =
{
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    0,
    1,
    1
};

inline static NamiGenType GenTypeToEnum(const std::string& type)
{
    static const std::vector<std::string> typeStrings =
    {
        std::string("circsin"),
        std::string("circlin"),

        std::string("linr"),
        std::string("linl"),
        std::string("lint"),
        std::string("linb"),

        std::string("sinr"),
        std::string("sinl"),
        std::string("sint"),
        std::string("sinb"),

        std::string("wavecirc"),
        std::string("wavehorizontal"),
        std::string("wavevertical"),
        std::string("waveempty")
    };

    unsigned int i = 1;
    for(const std::string& currentType : typeStrings)
    {
        if(type == currentType)
        {
            return static_cast<NamiGenType>(i);
        }
        i++;
    }
    return NamiGenType::INVALID;
}

inline static NamiGenOut GenOutToEnum(const std::string& out)
{
    static const std::vector<std::string> typeStrings =
    {
        std::string("grd"),
        std::string("grdbin")
    };

    unsigned int i = 1;
    for(const std::string& currentType : typeStrings)
    {
        if(out == currentType)
        {
            return static_cast<NamiGenOut>(i);
        }
        i++;
    }
    return NamiGenOut::INVALID;
}