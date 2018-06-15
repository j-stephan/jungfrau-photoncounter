#include "../Config.hpp"


struct CalibrationKernel {
    template <typename TAcc, typename TData, typename TPedestal, typename TNum>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc,
                                  TData const* const data,
                                  TPedestal* const pede,
                                  TNum const numframes) const -> void
    {
        auto const globalThreadIdx =
            alpaka::idx::getIdx<alpaka::Grid, alpaka::Threads>(acc);
        auto const globalThreadExtent =
            alpaka::workdiv::getWorkDiv<alpaka::Grid, alpaka::Threads>(acc);

        auto const linearizedGlobalThreadIdx =
            alpaka::idx::mapIdx<1u>(globalThreadIdx, globalThreadExtent);

        auto id = linearizedGlobalThreadIdx[0u];

        uint16_t stage = 0;

        for (std::size_t counter = 0; counter < numframes; ++counter) {
            while (pede[(stage * MAPSIZE) + id].counter >= FRAMESPERSTAGE) {
                if (!pede[(stage * MAPSIZE) + id].value) {
                    pede[(stage * MAPSIZE) + id].movAvg /=
                        pede[(stage * MAPSIZE) + id].counter;
                    pede[(stage * MAPSIZE) + id].value =
                        pede[(stage * MAPSIZE) + id].movAvg;
                }

                if (stage < 2)
                    ++stage;
            }

            pede[(stage * MAPSIZE) + id].movAvg +=
                data[(MAPSIZE * (counter)) + id +
                     (FRAMEOFFSET * (counter + 1u))] &
                0x3fff;
            pede[(stage * MAPSIZE) + id].counter++;
        }
    }
};
