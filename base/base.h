#pragma once

#include <chrono>
using namespace std::chrono_literals;
#include <cstddef>
#include <cstdint>
#include <thread>

#if _M_IX86_FP >= 1
#define SSE_ENABLED
#endif

#include "logger.h"
#include "types/angle.h"
#include "types/bitfield.h"
#include "types/color.h"
#include "types/dimension.h"
#include "types/matrix.h"
#include "types/vector.h"
