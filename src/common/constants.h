#ifndef COMMON_CONSTANTS_H
#define COMMON_CONSTANTS_H

#include <array>

#include "TpcAccess.h"

// This header is used for defining constants that are shared between projects
// Define them here if there is no clear "owner project" or where we don't want a direct dependency
// Mainly intended for sharing data between TpcAccess and the other projects

namespace common {

constexpr std::array<TPC_Parameter, 2> kAvailableValuesParams = {
  tpc_parBlkBlockLength,
  tpc_parScopeBlockLength,
};

}

#endif  // COMMON_CONSTANTS_H
