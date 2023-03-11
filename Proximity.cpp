#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <time.h>
#include <Timer.h>
#include <vector>

using namespace std;

using ActivatorKey = uint64_t;
using TriggerKey = uint64_t;
using Coordinate = uint32_t;

struct Position
{
    Position() = default;

    Position(Coordinate x, Coordinate y)
        : m_X(x), m_Y(y)
    {
    }

    Position(const Position& other)
        : m_X(other.m_X), m_Y(other.m_Y)
    {
    }

    Coordinate m_X{};
    Coordinate m_Y{};
};

struct Activator
{
    Activator() = default;

    Activator(Position position, ActivatorKey key)
        : m_Position(position), m_Key(key)
    {
    }

    Activator(const Activator& other)
        : m_Position(other.m_Position), m_Key(other.m_Key)
    {
    }

    Position m_Position{};
    ActivatorKey m_Key{};
};

struct Trigger
{
    Trigger() = default;

    Trigger(Position position, TriggerKey key, Coordinate inRange)
        : m_Position(position), m_Key(key), m_InRange(inRange)
    {
    }

    Trigger(const Trigger& other)
        : m_Position(other.m_Position), m_Key(other.m_Key), m_InRange(other.m_InRange)
    {
    }

    vector<ActivatorKey> m_ActivatorKeys;
    Position m_Position{};
    TriggerKey m_Key{};
    Coordinate m_InRange;
    uint64_t m_Padding{};
};

template<typename T>
inline T Difference(const T& first, const T& second)
{
    return first > second ? first - second : second - first;
}

inline bool IsInRange(const Position first, const Position second, const Coordinate inRange)
{
    return Difference(first.m_X, second.m_X) < inRange || Difference(first.m_Y, second.m_Y) < inRange;
}

template<typename T>
inline T RandomCoordinate(T minCoordinate, T maxCoordinate)
{
    return rand() % (maxCoordinate - minCoordinate + 1) + minCoordinate;
}

void BuildHotspots(vector<Position>& hotspots, uint32_t size, Coordinate minCoordinate, Coordinate maxCoordinate)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        hotspots.emplace_back(RandomCoordinate(minCoordinate, maxCoordinate), RandomCoordinate(minCoordinate, maxCoordinate));
    }
}

void BuildTriggers(vector<Position>& hotspots, vector<Trigger>& triggers, uint32_t size, Coordinate coordinateVariance, Coordinate inRange)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position triggerPos(x, y);
        triggers.emplace_back(triggerPos, i, inRange);
    }
}

void BuildActivators(vector<Position>& hotspots, vector<Activator>& activators, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position activatorPos(x, y);
        activators.emplace_back(activatorPos, i);
    }
}

void MoveActivators(vector<Position>& hotspots, vector<Activator>& activators, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        activators[i].m_Position = Position(x, y);
    }
}

int main()
{
    srand(uint32_t(time(0)));

    static constexpr uint32_t sc_MinCoordinate = 1000;
    static constexpr uint32_t sc_MaxCoordinate = 15000;
    static constexpr uint32_t sc_HotspotsSize = 4;
    static constexpr uint32_t sc_TriggersSize = 10000;
    static constexpr uint32_t sc_ActivatorsSize = 64;
    static constexpr uint32_t sc_CoordinateVariance = 100;
    static constexpr uint32_t sc_TriggerInRange = 75;

    Timer timer;

    vector<Position> hotspots;
    hotspots.reserve(sc_HotspotsSize);
    BuildHotspots(hotspots, sc_HotspotsSize, sc_MinCoordinate, sc_MaxCoordinate);

    vector<Trigger> triggers;
    triggers.reserve(sc_TriggersSize);
    BuildTriggers(hotspots, triggers, sc_TriggersSize, sc_CoordinateVariance, sc_TriggerInRange);

    vector<Activator> activators;
    activators.reserve(sc_ActivatorsSize);
    BuildActivators(hotspots, activators, sc_ActivatorsSize, sc_CoordinateVariance);

    double elapsed = timer.Elapsed();
    printf("%.2lf ns \r\n%.2lf us\r\n%.2lf s\r\n", elapsed, elapsed / 1000, elapsed / 1000000);
}