#pragma once

#include <cstdint>

#include <alpaka/alpaka.hpp>

//#############################################################################
//! A cheap wrapper around a C-style array in heap memory.
// was: std::uint64_t
template <typename T, unsigned long size> struct CheapArray {
    T data[size];

    //-----------------------------------------------------------------------------
    //! Access operator.
    //!
    //! \param index The index of the element to be accessed.
    //!
    //! Returns the requested element per reference.
    ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE T& operator[](uint64_t index)
    {
        return data[index];
    }

    //-----------------------------------------------------------------------------
    //! Access operator.
    //!
    //! \param index The index of the element to be accessed.
    //!
    //! Returns the requested element per constant reference.
    ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE const T&
    operator[](uint64_t index) const
    {
        return data[index];
    }
};
