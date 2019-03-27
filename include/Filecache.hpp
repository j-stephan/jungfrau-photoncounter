#pragma once

#include "jungfrau-photoncounter/Alpakaconfig.hpp"

#include <fstream>
#include <memory>


class Filecache {
private:
  std::unique_ptr<char, std::function<void(char*)>> buffer;
    char* bufferPointer;
    const std::size_t sizeBytes;
    auto getFileSize(const std::string path) const -> off_t;

public:
    Filecache(std::size_t size);
    template <typename TData, typename TAlpaka>
    auto loadMaps(const std::string& path, bool header = false)
        -> FramePackage<TData, TAlpaka>;
};

template <typename TData, typename TAlpaka>
auto Filecache::loadMaps(const std::string& path, bool header)
    -> FramePackage<TData, TAlpaka>
{
    // allocate space
    auto fileSize = getFileSize(path);
    std::size_t numFrames = fileSize / sizeof(TData);

    if (fileSize + bufferPointer >= buffer.get() + sizeBytes) {
        std::cerr << "Error: Not enough memory allocated!\n";
        exit(EXIT_FAILURE);
    }

    // load file content
    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't open file " << path << "!\n";
        exit(EXIT_FAILURE);
    }

    file.read(bufferPointer, fileSize);
    file.close();

    FramePackage<TData, TAlpaka> maps(numFrames);
    
    CpuSyncQueue streamBuf = alpakaGetHost<TAlpaka>();

    TData* dataBuf = reinterpret_cast<TData*>(bufferPointer);

    // copy data into alpaca memory
    alpakaCopy(
        streamBuf,
        maps.data,
        alpakaViewPlainPtrHost<TAlpaka, TData>(
                dataBuf,
                alpakaGetHost<TAlpaka>(),
                numFrames),
        numFrames);

    bufferPointer += fileSize;

    return maps;
}
