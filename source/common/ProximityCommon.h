#pragma once
#include <stdint.h>

using ActivatorKey = uint64_t;
using TriggerKey = uint64_t;
using Coordinate = uint16_t;

static constexpr Coordinate sc_MinCoordinate = 1000;
static constexpr Coordinate sc_MaxCoordinate = 15000;
static constexpr uint32_t sc_TriggersSize = 10048;
static constexpr uint32_t sc_ActivatorsSize = 64;
static constexpr ActivatorKey sc_InvalidActivatorKey = sc_ActivatorsSize + sc_TriggersSize;