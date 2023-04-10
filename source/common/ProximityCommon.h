#pragma once
#include <stdint.h>

using ActivatorKey = uint64_t;
using TriggerKey = uint64_t;
using Coordinate = uint32_t;

static constexpr uint32_t sc_MinCoordinate = 1000;
static constexpr uint32_t sc_MaxCoordinate = 15000;
static constexpr uint32_t sc_TriggersSize = 64;
static constexpr uint32_t sc_ActivatorsSize = 10000;
static constexpr ActivatorKey sc_InvalidActivatorKey = sc_ActivatorsSize + sc_TriggersSize;