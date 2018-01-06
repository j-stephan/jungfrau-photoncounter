#pragma once

#include <alpaka/alpaka.hpp>


// general settings
const std::size_t FRAMESPERSTAGE = 1000;
const std::size_t FRAME_HEADER_SIZE = 16;
const std::size_t FRAMEOFFSET = FRAME_HEADER_SIZE / 2;
const std::size_t DIMX = 1024;
const std::size_t DIMY = 512;
const std::size_t MAPSIZE = DIMX * DIMY;
const std::size_t SUM_FRAMES = 100;
const std::size_t DEV_FRAMES = 1000;
const std::size_t STREAMS_PER_DEV = 1;
const std::size_t PEDEMAPS = 3;
const std::size_t GAINMAPS = 3;
const float BEAMCONST = 6.2;
const float PHOTONCONST = (1. / 12.4);
const std::size_t MAXINT = std::numeric_limits<uint32_t>::max();

// data types
template <typename TData, typename TAlpaka> struct Maps {
    long unsigned int numMaps;
    alpaka::mem::buf::Buf<typename TAlpaka::DevHost,
                          TData,
                          typename TAlpaka::Dim,
                          typename TAlpaka::Size>
        data;
    bool header;
    Maps()
        : numMaps(0),
          data(alpaka::mem::buf::alloc<TData, typename TAlpaka::Size>(
              alpaka::pltf::getDevByIdx<typename TAlpaka::PltfHost>(0u),
              0lu)),
          header(false){};
};

using Data = std::uint16_t;
using Gain = double;
using Photon = std::uint16_t;
using PhotonSum = std::uint64_t;

struct Pedestal {
    std::uint32_t counter;
    Photon value;
    std::uint32_t movAvg;
};

// debug statements
#define SHOW_DEBUG false

#if (SHOW_DEBUG)
#include <iostream>
#define DEBUG(msg)                                                             \
    (std::cout << __FILE__ << "[" << __LINE__ << "]:\n\t" << msg << std::endl)
#else
#define DEBUG(msg) 
#endif

#if (SHOW_DEBUG)
#include "Bitmap.hpp"
#endif
template <typename TBuffer>
void save_image(std::string path, TBuffer* data, std::size_t frame_number)
{
#if (SHOW_DEBUG)
    Bitmap::Image img(1024ul, 512ul);
    for (std::size_t j = 0; j < 1024; j++) {
        for (std::size_t k = 0; k < 512; k++) {
            int h = int(data[(frame_number * (MAPSIZE + FRAMEOFFSET)) +
                             (k * 1024) + j + FRAMEOFFSET]) *10;
            Bitmap::Rgb color = {static_cast<unsigned char>(h & 255),
                static_cast<unsigned char>((h >> 8) & 255),
                static_cast<unsigned char>((h >> 16) & 255)};
            img(j, k) = color;
        }
    }
    img.writeToFile(path);
#endif
}
