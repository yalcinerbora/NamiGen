#include <iostream>
#include <vector>
#include <string>

#include "NamiGenOptions.h"

static NamiGenType GenTypeToEnum(const std::string& type)
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
        std::string("wavevertical")
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

static NamiGenOut GenOutToEnum(const std::string& out)
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
    "-w"
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
    1
};

static void PrintHelp()
{
    std::cout << "NamiDance Wave-Bathy Generator" << std::endl;
    std::cout << "-t <type>\t\t: Generation type (default \"circsin\")" << std::endl;
    std::cout << "\tcircsin\t\t: Circular bathymetry with edges are sinusodial" << std::endl;
    std::cout << "\tcirclin\t\t: Circular bathymetry with edges are linear" << std::endl;
    std::cout << "\tlinr\t\t: Flat right bathymetry with edges are linear" << std::endl;
    std::cout << "\tlinl\t\t: Flat left bathymetry with edges are linear" << std::endl;
    std::cout << "\tlint\t\t: Flat top bathymetry with edges are linear" << std::endl;
    std::cout << "\tlinb\t\t: Flat bottom bathymetry with edges are linear" << std::endl;
    std::cout << "\tsinr\t\t: Flat right bathymetry with edges are sinusodial" << std::endl;
    std::cout << "\tsinl\t\t: Flat left bathymetry with edges are sinusodial" << std::endl;
    std::cout << "\tsint\t\t: Flat top bathymetry with edges are sinusodial" << std::endl;
    std::cout << "\tsinb\t\t: Flat bottom bathymetry with edges are sinusodial" << std::endl;
    std::cout << "\twavecirc\t: Circular wave" << std::endl;
    std::cout << "\twavehorizontal\t: Flat horizontal wave" << std::endl;
    std::cout << "\twavevertical\t: Flat vertical wave" << std::endl;
    std::cout << std::endl;
    std::cout << "-o <type>\t\t: Output type (default \"grd\")" << std::endl;
    std::cout << "\tgrd\t\t: ASCII grd file" << std::endl;
    std::cout << "\tgrdbin\t\t: Binary grd file" << std::endl;
    std::cout << std::endl;
    std::cout << "-lat <X> <Y>\t\t: Latitude values (default 0.0 1.0f)" << std::endl;
    std::cout << "-lon <X> <Y>\t\t: Longitude values (default 0.0 1.0f)" << std::endl;
    std::cout << "-size <X> <Y>\t\t: Grid size (default 256 256)" << std::endl;
    std::cout << "-gap <Out> <In>\t\t: Logic dependant value (default 64 240)" << std::endl;
    std::cout << "-z <Min> <Max>\t\t: Min and Max eta (default -50.0f 10.0f)" << std::endl;
    std::cout << "-wall\t\t\t: Puts walls on the borders (default off)" << std::endl;
    std::cout << "-w <width>\t\t: Wall width in terms of grids (default 3pt)" << std::endl;
}

static void PrintOptions(const NamiGenOptions& options)
{
    std::cout << "Lat\t: " << options.latMin << " " << options.latMax << std::endl;
    std::cout << "Lon\t: " << options.lonMin << " " << options.lonMax << std::endl;
    std::cout << "Size\t: " << options.sizeX << " " << options.sizeY << std::endl;
    std::cout << "Gap\t: " << options.gapOut << " " << options.gapIn << std::endl;
    std::cout << "Z\t: " << options.zMin << " " << options.zMax << std::endl;
    std::cout << "Type\t: " << static_cast<int>(options.type) << std::endl;
    std::cout << "Out\t: " << static_cast<int>(options.output) << std::endl;
    std::cout << "Walls\t: " << ((options.hasWalls) ? std::string("true") : std::string("false")) << std::endl;
    std::cout << "Width\t: " << options.wallWidth << std::endl;
}

constexpr NamiGenOptions namiOptsDefault = NamiGenOptions
{
    0.0, 1.0,
    0.0, 1.0,
    256, 256,
    64, 240,
    -50.0f, 10.0f,
    NamiGenOut::GRD,
    NamiGenType::CIRCULAR_SINUSODIAL,
    false,
    3
};

int main(int argc, const char* argv[])
{
    NamiGenOptions namiOptions = namiOptsDefault;
    if(argc == 1)
    {
        PrintHelp();
    }
    else
    {
        // Consume Args
        for(int i = 1; i < argc; i++)
        {
            const std::string arg = argv[i];

            // Traverse Switches
            int argId = 0;
            for(const std::string& sw : switches)
            {
                if(arg == sw)
                {
                    // Found a valid switch
                    // Check if we can consume enough args
                    if(argc - i <= switchArgCounts[argId])
                    {
                        // not enough arg count to open this
                        std::cout << "Invalid arg count" << std::endl;
                        return 0;
                    }
                    else
                    {
                        // Arg Parse
                        if(arg == switches[0]) // -l
                        {
                            NamiGenType type = GenTypeToEnum(argv[i + 1]);
                            if(type == NamiGenType::INVALID)
                            {
                                std::cout << "Invalid " << switches[0] << " switch" << std::endl;
                                return 0;
                            }
                            namiOptions.type = type;
                        }
                        else if(arg == switches[1]) // -o
                        {
                            NamiGenOut outType = GenOutToEnum(argv[i + 1]);
                            if(outType == NamiGenOut::INVALID)
                            {
                                std::cout << "Invalid " << switches[1] << " switch" << std::endl;
                                return 0;
                            }
                            namiOptions.output = outType;
                        }
                        else if(arg == switches[2]) // -lat
                        {
                            namiOptions.latMin = std::stod(argv[i + 1]);
                            namiOptions.latMax = std::stod(argv[i + 2]);
                        }
                        else if(arg == switches[3]) // -lon
                        {
                            namiOptions.lonMin = std::stod(argv[i + 1]);
                            namiOptions.lonMax = std::stod(argv[i + 2]);
                        }
                        else if(arg == switches[4]) // -size
                        {
                            namiOptions.sizeX = std::stoi(argv[i + 1]);
                            namiOptions.sizeY = std::stoi(argv[i + 2]);
                        }
                        else if(arg == switches[5]) // -gap
                        {
                            namiOptions.gapOut = std::stoi(argv[i + 1]);
                            namiOptions.gapIn = std::stoi(argv[i + 2]);
                        }
                        else if(arg == switches[6]) // -z
                        {
                            namiOptions.zMin = std::stof(argv[i + 1]);
                            namiOptions.zMax = std::stof(argv[i + 2]);
                        }
                        else if(arg == switches[7]) // -wall
                        {
                            namiOptions.hasWalls = true;
                        }
                        else if(arg == switches[8]) // -w
                        {
                            namiOptions.wallWidth = std::stoi(argv[i + 1]);
                        }
                        i += switchArgCounts[argId];
                        break;
                    }
                }
                argId++;
            }
            if(argId == switches.size())
            {
                std::cout << "Invalid Switch" << std::endl;
                return 0;
            }
        }
        PrintOptions(namiOptions);













    }
    return 0;
}