#include "Kernel.hpp"

__global__ void calculate(uint16_t mapsize, uint16_t* pede, double* gain,
                          uint16_t* data, uint16_t num, float* energy)
{
    __shared__ uint16_t sPede[3 * mapsize];
    __shared__ uint16_t sGain[3 * mapsize];

    uint16_t id = blockIdx.x * blockDim.x + threadIdx.x;

    sPede[id] = pede[id];
    sPede[mapsize + id] = pede[mapsize + id];
    sPede[(mapsize * 2) + id] = pede[(mapsize * 2) + id];
    sGain[id] = gain[id];
    sGain[mapsize + id] = gain[mapsize + id];
    sGain[(mapsize * 2) + id] = gain[(mapsize * 2) + id];

    __syncthreads();

    for (int i = 0; i < num; i++) {
        uint16_t dataword = data[(mapsize * i) + id];

        switch ((dataword & 0xc000) >> 14) {
        case 0:
            energy[(mapsize * i) + id] =
                (dataword & 0x3fff - sPede[id]) * sGain[id];
            break;
        case 1:
            energy[(mapsize * i) + id] =
                (sPede[mapsize + id] - dataword & 0x3fff) * sGain[mapsize + id];
            break;
        case 3:
            energy[(mapsize * i) + id] =
                (sPede[(2 * mapsize) + id] - dataword & 0x3fff) *
                sGain[(2 * mapsize) + id];
            break;
        default:
            energy[(mapsize * i) + id] = 0;
            break;
        }
    }
}

__global__ void calibrate(uint16_t mapsize, uint16_t* data, uint16_t num,
                          uint16_t* pede)
{
    uint16_t id = blockIdx.x * blockDim.x + threadIdx.x;

    for (int i = 0; i < 1000; i++) {
        if (i == 0) {
            pede[id] = data[(mapsize * i) + id] & 0x3fff;
        }
        else {
            pede[id] += data[(mapsize * i) + id] & 0x3fff;
        }
    }

    for (int i = 1000; i < num; i++) {
        pede[id] =
            (pede[id] + data[(mapsize * i) + id] & 0x3fff) - (pede[id] / i);
    }

    pede[id] = round((double)pede[id] / 1000);
}
