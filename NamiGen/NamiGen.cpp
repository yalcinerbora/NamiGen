#include <iostream>
#include <vector>
#include <string>

#include "NamiGenOptions.h"
#include "NamiGenFunctions.h"

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
    std::cout << "\tduhis\t\t: Special double wedge" << std::endl;
    std::cout << "\twavecirc\t: Circular wave" << std::endl;
    std::cout << "\twavehorizontal\t: Flat horizontal wave" << std::endl;
    std::cout << "\twavevertical\t: Flat vertical wave" << std::endl;
    std::cout << "\twaveempty\t: Empty wave" << std::endl;
    std::cout << std::endl;
    std::cout << "-o <type>\t\t: Output type (default \"grd\")" << std::endl;
    std::cout << "\tgrd\t\t: ASCII grd file" << std::endl;
    std::cout << "\tgrdbin\t\t: Binary grd file" << std::endl;
    std::cout << std::endl;
    std::cout << "-n <name>\t\t: Output file name type (default \"output(_bin).grd\")" << std::endl;
    std::cout << "-lat <X> <Y>\t\t: Latitude values (default 0.0 1.0f)" << std::endl;
    std::cout << "-lon <X> <Y>\t\t: Longitude values (default 0.0 1.0f)" << std::endl;
    std::cout << "-size <X> <Y>\t\t: Grid size (default 256 256)" << std::endl;
    std::cout << "-gap <Bottom> <Top>\t: Logic dependant value (default 64 240)" << std::endl;
    std::cout << "-z <Land> <Bottom>\t: Land and Bottom eta (default -10.0f 50.0f)" << std::endl;
    std::cout << "-wall\t\t\t: Puts walls on the borders (default off)" << std::endl;
    std::cout << "-w <width>\t\t: Wall width in terms of grids (default 3pt)" << std::endl;
    std::cout << "-tana <width>\t\t: tangent of the linear slope (default 1, tan(pi/4))" << std::endl;
}

static void PrintOptions(const NamiGenOptions& options)
{
    std::cout << "Lat\t: " << options.latMin << " " << options.latMax << std::endl;
    std::cout << "Lon\t: " << options.lonMin << " " << options.lonMax << std::endl;
    std::cout << "Size\t: " << options.sizeX << " " << options.sizeY << std::endl;
    std::cout << "Gap\t: " << options.gapBottom << " " << options.gapTop << std::endl;
    std::cout << "Z\t: " << options.zLand << " " << options.zBottom << std::endl;
    std::cout << "Type\t: " << static_cast<int>(options.type) << std::endl;
    std::cout << "Out\t: " << static_cast<int>(options.output) << std::endl;
    std::cout << "Walls\t: " << ((options.hasWalls) ? std::string("true") : std::string("false")) << std::endl;
    std::cout << "Width\t: " << options.wallWidth << std::endl;
    std::cout << "Tana\t: " << options.tana << std::endl;
}

int main(int argc, const char* argv[])
{
    NamiGenOptions namiOptions = namiOptsDefault;
    std::string outputFileName = "output";
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
                            namiOptions.gapBottom = std::stoi(argv[i + 1]);
                            namiOptions.gapTop = std::stoi(argv[i + 2]);
                        }
                        else if(arg == switches[6]) // -z
                        {
                            namiOptions.zLand = std::stof(argv[i + 1]);
                            namiOptions.zBottom = std::stof(argv[i + 2]);
                        }
                        else if(arg == switches[7]) // -wall
                        {
                            namiOptions.hasWalls = true;
                        }
                        else if(arg == switches[8]) // -w
                        {
                            namiOptions.wallWidth = std::stoi(argv[i + 1]);
                        }
                        else if(arg == switches[9]) // -n
                        {
                            outputFileName = argv[i + 1];
                        }
                        else if(arg == switches[10]) // -tana
                        {
                            namiOptions.tana = std::stof(argv[i + 1]);
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

        // Empty
        std::cout << "Using These Parameters" << std::endl;
        PrintOptions(namiOptions);
        std::cout << "----------" << std::endl;

        // Allocation and Traversal
        float min = FLT_MAX;
        float max = -FLT_MAX;
        std::vector<float> grdData(namiOptions.sizeX * 
                                   namiOptions.sizeY);
        for(int i = 0; i < grdData.size(); i++)
        {
            int x = i % namiOptions.sizeX;
            int y = i / namiOptions.sizeX;

            // Wave Segment
            switch(namiOptions.type)
            {
                case NamiGenType::WAVE_CIRCULAR:
                case NamiGenType::WAVE_HORIZONTAL:
                case NamiGenType::WAVE_VERTICAL:
                case NamiGenType::WAVE_EMPTY:
                {
                    grdData[i] = WaveSample(x, y, namiOptions);
                    min = std::min(grdData[i], min);
                    max = std::max(grdData[i], max);
                    continue;
                }
            }
            // Bathy Segment
            // Wall
            if(namiOptions.type == NamiGenType::DUHIS &&
               y < namiOptions.wallWidth ||
               y >= (namiOptions.sizeY - namiOptions.wallWidth))
            {
                grdData[i] = namiOptions.zLand;             
            }
            else if(namiOptions.type != NamiGenType::DUHIS &&
                    (namiOptions.hasWalls &&
                     (x < namiOptions.wallWidth ||
                      x >= (namiOptions.sizeX - namiOptions.wallWidth) ||
                      y < namiOptions.wallWidth ||
                      y >= (namiOptions.sizeY - namiOptions.wallWidth))))
            {
                grdData[i] = namiOptions.zLand;
            }
            else
            {
                switch(namiOptions.type)
                {
                    case NamiGenType::CIRCULAR_LINEAR:
                    case NamiGenType::CIRCULAR_SINUSODIAL:
                    {
                        grdData[i] = CircleSample(x, y, namiOptions);
                        break;
                    }
                    case NamiGenType::LINEAR_L:
                    case NamiGenType::LINEAR_R:
                    case NamiGenType::LINEAR_T:
                    case NamiGenType::LINEAR_B:
                    case NamiGenType::SINUSODIAL_L:
                    case NamiGenType::SINUSODIAL_R:
                    case NamiGenType::SINUSODIAL_T:
                    case NamiGenType::SINUSODIAL_B:
                    {
                        grdData[i] = SampleFlat(x, y, namiOptions);
                        break;
                    }
                    case NamiGenType::DUHIS:
                    {
                        grdData[i] = SampleDuhis(x, y, namiOptions);
                        break;
                    }
                }
            }
            min = std::min(grdData[i], min);
            max = std::max(grdData[i], max);
        }

        // Generation Complete Now Write
        if(namiOptions.output == NamiGenOut::GRD)
        {
            outputFileName += ".grd";
            OutGRD(grdData.data(),
                   namiOptions,
                   min, max,
                   outputFileName);
        }
        else
        {
            outputFileName += "_bin.grd";
            OutGRDBin(grdData.data(),
                      namiOptions,
                      min, max,
                      outputFileName);
        }
    }
    return 0;
}  