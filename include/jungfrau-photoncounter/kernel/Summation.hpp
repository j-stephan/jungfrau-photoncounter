#include "../Config.hpp"

struct SummationKernel {
    template <typename TAcc,
              typename TData,
              typename TNumFrames,
              typename TNumSumFrames,
              typename TSumMap>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc,
                                  TData const* const data,
                                  TNumSumFrames const numSumFrames,
                                  TNumFrames const numFrames,
                                  TSumMap* const sum) const -> void
    {
        auto const globalThreadIdx =
            alpaka::idx::getIdx<alpaka::Grid, alpaka::Threads>(acc);
        auto const globalThreadExtent =
            alpaka::workdiv::getWorkDiv<alpaka::Grid, alpaka::Threads>(acc);

        auto const linearizedGlobalThreadIdx =
            alpaka::idx::mapIdx<1u>(globalThreadIdx, globalThreadExtent);

        auto id = linearizedGlobalThreadIdx[0u];

        // check range
        if (id >= MAPSIZE)
            return;
        
        for (TNumFrames i = 0; i < numFrames; ++i) {
            if (i % numSumFrames == 0)
                sum[i / numSumFrames].data[id] = data[i].data[id];
            else
                sum[i / numSumFrames].data[id] += data[i].data[id];
        }
    }
};
