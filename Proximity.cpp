#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <time.h>
#include <Timer.h>
#include <vector>

#include <extern\aabb\AABB.h>

using namespace std;


using ActivatorKey = uint64_t;
using TriggerKey = uint64_t;
using Coordinate = uint32_t;

static constexpr uint32_t sc_MinCoordinate = 1000;
static constexpr uint32_t sc_MaxCoordinate = 15000;
static constexpr uint32_t sc_HotspotsSize = 8;
static constexpr uint32_t sc_TriggersSize = 10000;
static constexpr uint32_t sc_ActivatorsSize = 64;
static constexpr ActivatorKey sc_InvalidActivatorKey = sc_ActivatorsSize;
static constexpr uint32_t sc_CoordinateVariance = 100;
static constexpr uint32_t sc_TriggerInRange = 75;
static constexpr uint32_t sc_RepeatCount = 1;

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

    bool operator<(const Activator& other)
    {
        return m_Position.m_X < other.m_Position.m_X;
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
    Trigger& operator=(const Trigger& other)
    {
        m_Position = other.m_Position;
        m_Key = other.m_Key;
        m_InRange = other.m_InRange;
    }

    Trigger(Trigger&& other) = default;
    Trigger& operator=(Trigger&& other) = default;


    bool operator<(const Trigger& other)
    {
        return m_Position.m_X < other.m_Position.m_X;
    }

    vector<ActivatorKey> m_ActivatorKeys{};
    Position m_Position{};
    TriggerKey m_Key{};
    Coordinate m_InRange{};
    uint64_t m_Padding[2]{};
};

using Hotspots = array<Position, sc_HotspotsSize>;
using Triggers = array<Trigger, sc_TriggersSize>;
using Activators = array<Activator, sc_ActivatorsSize>;
using ActivatorKeys = array<ActivatorKey, sc_ActivatorsSize>;

template<typename T>
inline T Difference(const T& first, const T& second)
{
    return first > second ? first - second : second - first;
}

inline bool IsInRange(const Coordinate first, const Coordinate second, const Coordinate inRange)
{
    return Difference(first, second) < inRange;
}

inline bool IsInRange(const Position first, const Position second, const Coordinate inRange)
{
    return IsInRange(first.m_X, second.m_X, inRange) || IsInRange(first.m_Y, second.m_Y, inRange);
}

template<typename T>
inline T RandomCoordinate(T minCoordinate, T maxCoordinate)
{
    return rand() % (maxCoordinate - minCoordinate + 1) + minCoordinate;
}

void BuildHotspots(Hotspots& hotspots, uint32_t size, Coordinate minCoordinate, Coordinate maxCoordinate)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        hotspots[i] = Position(RandomCoordinate(minCoordinate, maxCoordinate), RandomCoordinate(minCoordinate, maxCoordinate));
    }
}

void BuildTriggers(aabb::Tree& tree, Hotspots& hotspots, Triggers& triggers, uint32_t size, Coordinate coordinateVariance, Coordinate inRange)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position triggerPos(x, y);
        triggers[i] = Trigger(triggerPos, i, inRange);
        auto vec = vector<double>({ (double)x, (double)y });
        tree.insertParticle(i, vec, (double)inRange);
    }
}

void BuildActivators(aabb::Tree& tree, Hotspots& hotspots, Activators& activators, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position activatorPos(x, y);
        activators[i] = Activator(activatorPos, i);
        auto vec = vector<double>({ (double)x, (double)y });
        tree.insertParticle(i + sc_TriggersSize, vec, 0.05);
    }
}

void MoveActivators(aabb::Tree& tree, Hotspots& hotspots, Activators& activators, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        activators[i].m_Position = Position(x, y);
        auto vec = vector<double>({ (double)x, (double)y });
        tree.updateParticle(i + sc_TriggersSize, vec, 0.05);
    }
}

void FilterInActivators(aabb::Tree& tree, const Trigger& trigger, const Activators& activators, ActivatorKeys& inActivators, uint64_t& inActivatorsSize)
{
    inActivatorsSize = 0;
    const vector<unsigned int>& result = tree.query(trigger.m_Key);
    for (unsigned int key : result)
    {
        if (key >= sc_TriggersSize)
        {
            inActivators[inActivatorsSize++] = key - sc_TriggersSize;
        }
    }
}

void FilterNewInOutActivators(Trigger& trigger, const ActivatorKeys& inActivators, uint64_t inActivatorsSize, ActivatorKeys& newInActivators, uint64_t& newInActivatorsSize, ActivatorKeys& newOutActivators, uint64_t& newOutActivatorsSize)
{
    vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
    for (uint32_t i = 0; i < inActivatorsSize; ++i)
    {
        ActivatorKey activatorKey = inActivators[i];
        if (std::find(existingKeys.begin(), existingKeys.begin(), activatorKey) == existingKeys.end())
        {
            newInActivators[newInActivatorsSize++] = activatorKey;
        }
    }

    auto inActivatorsBegin = inActivators.begin();
    auto inActivatorsEnd = inActivatorsBegin + inActivatorsSize;
    for (const ActivatorKey& activatorKey : existingKeys)
    {
        if (std::find(inActivatorsBegin, inActivatorsEnd, activatorKey) == inActivatorsEnd)
        {
            newOutActivators[newOutActivatorsSize++] = activatorKey;
        }
    }
    existingKeys.clear();
    std::copy(inActivatorsBegin, inActivatorsEnd, back_inserter(existingKeys));
}

void UpdateProximity(aabb::Tree& tree, Triggers& triggers, Activators& activators, uint64_t& counter)
{
    for (Trigger& trigger : triggers)
    {
        ActivatorKeys inActivators;
        uint64_t inActivatorsSize{};
        FilterInActivators(tree, trigger, activators, inActivators, inActivatorsSize);

        ActivatorKeys newInActivators;
        uint64_t newInActivatorsSize{};
        ActivatorKeys newOutActivators;
        uint64_t newOutActivatorsSize{};
        FilterNewInOutActivators(trigger, inActivators, inActivatorsSize, newInActivators, newInActivatorsSize, newOutActivators, newOutActivatorsSize);
        
        counter += newInActivators.size() + newOutActivators.size();
    }
}

int main()
{
    srand(uint32_t(time(0)));

    Timer timer;

    Hotspots hotspots;
    BuildHotspots(hotspots, sc_HotspotsSize, sc_MinCoordinate, sc_MaxCoordinate);

    // Periodicity of the simulation box.
    std::vector<bool> periodicity({ true, true });

    // Size of the simulation box.
    std::vector<double> boxSize({ 2 * sc_MaxCoordinate, 2 * sc_MaxCoordinate });

    aabb::Tree tree(2, 0.05, periodicity, boxSize, sc_TriggersSize + sc_ActivatorsSize, true);

    Triggers triggers;
    BuildTriggers(tree, hotspots, triggers, sc_TriggersSize, sc_CoordinateVariance, sc_TriggerInRange);

    Activators activators;
    BuildActivators(tree, hotspots, activators, sc_ActivatorsSize, sc_CoordinateVariance);

    double elapsed = timer.Elapsed();
    printf("Build: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", elapsed, elapsed / 1000, elapsed / 1000000);

    double averageElapsed{};
    uint64_t counter{};
    for (uint32_t i = 0; i < sc_RepeatCount; ++i)
    {
        timer.Reinit();
        
        UpdateProximity(tree, triggers, activators, counter);

        elapsed = timer.Elapsed();
        averageElapsed += elapsed / sc_RepeatCount;

        MoveActivators(tree, hotspots, activators, sc_ActivatorsSize, sc_CoordinateVariance);
    }
    printf("%lld\r\nAverage: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", counter, averageElapsed, averageElapsed / 1000, averageElapsed / 1000000);
}