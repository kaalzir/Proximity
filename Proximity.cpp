#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <source/common/IProximity.h>
#include <source/common/Position.h>
#include <source/common/ProximityCommon.h>
#include <source/common/Timer.h>
#include <source/naive_solution/NaiveProximityImpl.h>
#include <stdint.h>
#include <tests/UnitTests.h>
#include <time.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

static constexpr uint32_t sc_HotspotsSize = 4;
static constexpr uint32_t sc_CoordinateVariance = 100;
static constexpr uint32_t sc_TriggerInRange = 75;
static constexpr uint32_t sc_RepeatCount = 100;
static constexpr uint32_t sc_ActivatorUpdateRate = 4; // 1 in n activators will be updated each frame
static constexpr uint32_t sc_TriggerUpdateRate = 4; // 1 in n triggers will be updated each frame

using Hotspots = array<Position, sc_HotspotsSize>;

void BuildHotspots(Hotspots& hotspots, uint32_t size, Coordinate minCoordinate, Coordinate maxCoordinate)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        hotspots[i] = Position(RandomCoordinate(minCoordinate, maxCoordinate), RandomCoordinate(minCoordinate, maxCoordinate));
    }
}

void BuildTriggers(Hotspots& hotspots, IProximity& proximity, uint32_t size, Coordinate coordinateVariance, Coordinate inRange)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position triggerPos(x, y);
        proximity.CreateTrigger(triggerPos, inRange);
    }
}

void BuildActivators(Hotspots& hotspots, IProximity& proximity, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position activatorPos(x, y);
        proximity.CreateActivator(activatorPos);
    }
}

void MoveActivators(Hotspots& hotspots, IProximity& proximity, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        proximity.UpdateActivator(i, Position(x, y));
    }
}

void MoveTriggers(Hotspots& hotspots, IProximity& proximity, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        proximity.UpdateTrigger(i, Position(x, y));
    }
}

int main()
{
    srand(uint32_t(time(0)));

    Timer timer;

    IProximity* proximity = new NaiveProximityImpl();

    Hotspots hotspots;
    BuildHotspots(hotspots, sc_HotspotsSize, sc_MinCoordinate, sc_MaxCoordinate);

    BuildTriggers(hotspots, *proximity, sc_TriggersSize, sc_CoordinateVariance, sc_TriggerInRange);
    BuildActivators(hotspots, *proximity, sc_ActivatorsSize, sc_CoordinateVariance);

    double elapsed = timer.Elapsed();
    printf("Build: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", elapsed, elapsed / 1000, elapsed / 1000000);

    double averageElapsed{};
    double firstFrame{};
    double maxFrame{};
    double minFrame{};
    for (uint32_t i = 0; i < sc_RepeatCount; ++i)
    {
        timer.Reinit();
        
        proximity->UpdateProximity();

        elapsed = timer.Elapsed();
        averageElapsed += elapsed / sc_RepeatCount;

        if (i == 0)
        {
            firstFrame = elapsed;
            maxFrame = elapsed;
            minFrame = elapsed;
        }

        if (elapsed < minFrame)
        {
            minFrame = elapsed;
        }

        if (elapsed > maxFrame)
        {
            maxFrame = elapsed;
        }

        MoveActivators(hotspots, *proximity, sc_ActivatorsSize, sc_CoordinateVariance);
        MoveTriggers(hotspots, *proximity, sc_TriggersSize, sc_CoordinateVariance);
    }
    printf("FirstFrame: %.3lf ms \r\nMinFrame: %.3lf ms\r\nMaxFrame:%.3lf ms\r\n", firstFrame / 1000000, minFrame / 1000000, maxFrame / 1000000);
    printf("Average: %.3lf ns \r\n%.3lf us\r\n%.3lf ms\r\n", averageElapsed, averageElapsed / 1000, averageElapsed / 1000000);

    delete(proximity);
}