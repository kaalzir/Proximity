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
#include <unordered_set>
#include <unordered_map>

using namespace std;


using ActivatorKey = uint64_t;
using TriggerKey = uint64_t;
using Coordinate = uint32_t;

static constexpr uint32_t sc_MinCoordinate = 1000;
static constexpr uint32_t sc_MaxCoordinate = 15000;
static constexpr uint32_t sc_HotspotsSize = 4;
static constexpr uint32_t sc_TriggersSize = 64;
static constexpr uint32_t sc_ActivatorsSize = 10000;
static constexpr ActivatorKey sc_InvalidActivatorKey = sc_ActivatorsSize + sc_TriggersSize;
static constexpr uint32_t sc_CoordinateVariance = 100;
static constexpr uint32_t sc_TriggerInRange = 75;
static constexpr uint32_t sc_RepeatCount = 100;

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
        return m_Position.m_X < other.m_Position.m_X || (m_Position.m_X == other.m_Position.m_X && m_Position.m_Y < other.m_Position.m_Y);
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
        return m_Position.m_X < other.m_Position.m_X || (m_Position.m_X == other.m_Position.m_X && m_Position.m_Y < other.m_Position.m_Y);
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

void BuildTriggers(Hotspots& hotspots, Triggers& triggers, uint32_t size, Coordinate coordinateVariance, Coordinate inRange)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position triggerPos(x, y);
        triggers[i] = Trigger(triggerPos, i, inRange);
    }
}

void BuildActivators(Hotspots& hotspots, Activators& activators, uint32_t size, Coordinate coordinateVariance)
{
    uint64_t hotspotSize{ hotspots.size() };
    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t hotspotIndex{ i % hotspotSize };
        uint32_t x{ RandomCoordinate(hotspots[hotspotIndex].m_X - coordinateVariance, hotspots[hotspotIndex].m_X + coordinateVariance) };
        uint32_t y{ RandomCoordinate(hotspots[hotspotIndex].m_Y - coordinateVariance, hotspots[hotspotIndex].m_Y + coordinateVariance) };
        Position activatorPos(x, y);
        activators[i] = Activator(activatorPos, i);
    }
}

void MoveActivators(Hotspots& hotspots, Activators& activators, uint32_t size, Coordinate coordinateVariance)
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

void FilterInActivators(unordered_map<ActivatorKey, TriggerKey>& inSet, const Trigger& trigger, const Activators& activators, uint64_t activatorsBeginIndex, uint64_t activatorsEndIndex, ActivatorKeys& inActivators, uint64_t& inActivatorsSize, double& someElapsed)
{
    //Timer timer;
    inActivatorsSize = 0;
    for (uint64_t i = activatorsBeginIndex; i < activatorsEndIndex; ++i)
    {
        if (IsInRange(trigger.m_Position.m_Y, activators[i].m_Position.m_Y, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = activators[i].m_Key;
            inSet[activators[i].m_Key] = trigger.m_Key;
        }
    }
    //someElapsed += timer.Elapsed() / sc_RepeatCount;
}

void FilterNewInOutActivators(unordered_map<ActivatorKey, TriggerKey>& inSet, Trigger& trigger, const ActivatorKeys& inActivators, uint64_t inActivatorsSize, ActivatorKeys& newInActivators, uint64_t& newInActivatorsSize, ActivatorKeys& newOutActivators, uint64_t& newOutActivatorsSize, double& someElapsed)
{
    //Timer timer;
    if (false)
    {
        unordered_set<ActivatorKey> existingSet(trigger.m_ActivatorKeys.begin(), trigger.m_ActivatorKeys.end());
        auto inActivatorsBegin = inActivators.begin();
        auto inActivatorsEnd = inActivatorsBegin + inActivatorsSize;
        unordered_set<ActivatorKey> insSet(inActivatorsBegin, inActivatorsEnd);
        vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };

        for (uint32_t i = 0; i < inActivatorsSize; ++i)
        {
            ActivatorKey activatorKey = inActivators[i];
            if (existingSet.find(activatorKey) == existingSet.end())
            {
                newInActivators[newInActivatorsSize++] = activatorKey;
            }
        }
        for (ActivatorKey activatorKey : existingKeys)
        {
            if (insSet.find(activatorKey) == insSet.end())
            {
                newOutActivators[newOutActivatorsSize++] = activatorKey;
            }
        }
        existingKeys.clear();
        //auto inActivatorsBegin = inActivators.begin();
        //auto inActivatorsEnd = inActivatorsBegin + inActivatorsSize;
        std::copy(inActivatorsBegin, inActivatorsEnd, back_inserter(existingKeys));
        return;
    }
    if (true)
    {
        vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
        for (ActivatorKey activatorKey : existingKeys)
        {
            auto it = inSet.find(activatorKey);
            if (it == inSet.end() || it->second != trigger.m_Key)
            {
                newOutActivators[newOutActivatorsSize++] = activatorKey;
            }
            else if (it != inSet.end())
            {
                it->second = sc_InvalidActivatorKey;
            }
        }

        for (uint32_t i = 0; i < inActivatorsSize; ++i)
        {
            ActivatorKey activatorKey = inActivators[i];
            if (inSet[activatorKey] != sc_InvalidActivatorKey)
            {
                newInActivators[newInActivatorsSize++] = activatorKey;
            }
        }
        existingKeys.clear();
        auto inActivatorsBegin = inActivators.begin();
        auto inActivatorsEnd = inActivatorsBegin + inActivatorsSize;
        std::copy(inActivatorsBegin, inActivatorsEnd, back_inserter(existingKeys));
    }
    else
    {
        vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
        auto existingActivatorsBegin = existingKeys.begin();
        auto existingActivatorsEnd = existingKeys.end();
        for (uint32_t i = 0; i < inActivatorsSize; ++i)
        {
            ActivatorKey activatorKey = inActivators[i];
            if (find(existingActivatorsBegin, existingActivatorsEnd, activatorKey) == existingActivatorsEnd)
            {
                newInActivators[newInActivatorsSize++] = activatorKey;
            }
        }

        auto inActivatorsBegin = inActivators.begin();
        auto inActivatorsEnd = inActivatorsBegin + inActivatorsSize;
        for (ActivatorKey activatorKey : existingKeys)
        {
            if (find(inActivatorsBegin, inActivatorsEnd, activatorKey) == inActivatorsEnd)
            {
                newOutActivators[newOutActivatorsSize++] = activatorKey;
            }
        }
        existingKeys.clear();
        std::copy(inActivatorsBegin, inActivatorsEnd, back_inserter(existingKeys));
    }
}

void UpdateProximity(Triggers& triggers, Activators& activators, uint64_t& counter, double& someElapsed)
{
    //Timer t;
    sort(activators.begin(), activators.end());
    sort(triggers.begin(), triggers.end());
    //someElapsed += t.Elapsed() / sc_RepeatCount;

    uint64_t activatorsBeginIndex{};
    uint64_t activatorsEndIndex{};
    unordered_map<ActivatorKey, TriggerKey> inSet;
    inSet.reserve(activators.size());

    for (Trigger& trigger : triggers)
    {
        //Timer timer;
        uint64_t activatorsSize{ activators.size() };
        while (activatorsBeginIndex != activatorsSize && activators[activatorsBeginIndex].m_Position.m_X < trigger.m_Position.m_X && !IsInRange(trigger.m_Position.m_X, activators[activatorsBeginIndex].m_Position.m_X, trigger.m_InRange))
        {
            ++activatorsBeginIndex;
        }
        if (activatorsBeginIndex > activatorsEndIndex)
        {
            activatorsEndIndex = activatorsBeginIndex;
        }
        while (activatorsEndIndex != activatorsSize && (activators[activatorsEndIndex].m_Position.m_X < trigger.m_Position.m_X ||  IsInRange(trigger.m_Position.m_X, activators[activatorsEndIndex].m_Position.m_X, trigger.m_InRange)))
        {
            ++activatorsEndIndex;
        }
        //someElapsed += timer.Elapsed() / sc_RepeatCount;
        //timer.Reinit();
        ActivatorKeys inActivators;
        uint64_t inActivatorsSize{};
        FilterInActivators(inSet,trigger, activators, activatorsBeginIndex, activatorsEndIndex, inActivators, inActivatorsSize, someElapsed);
        //someElapsed += timer.Elapsed() / sc_RepeatCount;

        //timer.Reinit();
        ActivatorKeys newInActivators;
        uint64_t newInActivatorsSize{};
        ActivatorKeys newOutActivators;
        uint64_t newOutActivatorsSize{};
        FilterNewInOutActivators(inSet, trigger, inActivators, inActivatorsSize, newInActivators, newInActivatorsSize, newOutActivators, newOutActivatorsSize, someElapsed);

        //someElapsed += timer.Elapsed() / sc_RepeatCount;
        counter += activatorsBeginIndex;
    }
}

int main()
{
    srand(uint32_t(time(0)));

    Timer timer;

    Hotspots hotspots;
    BuildHotspots(hotspots, sc_HotspotsSize, sc_MinCoordinate, sc_MaxCoordinate);

    Triggers triggers;
    BuildTriggers(hotspots, triggers, sc_TriggersSize, sc_CoordinateVariance, sc_TriggerInRange);

    Activators activators;
    BuildActivators(hotspots, activators, sc_ActivatorsSize, sc_CoordinateVariance);

    double elapsed = timer.Elapsed();
    printf("Build: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", elapsed, elapsed / 1000, elapsed / 1000000);

    double averageElapsed{};
    double someElapsed{};
    uint64_t counter{};
    for (uint32_t i = 0; i < sc_RepeatCount; ++i)
    {
        timer.Reinit();
        
        UpdateProximity(triggers, activators, counter, someElapsed);

        elapsed = timer.Elapsed();
        averageElapsed += elapsed / sc_RepeatCount;

        MoveActivators(hotspots, activators, sc_ActivatorsSize, sc_CoordinateVariance);
    }
    printf("%lld\r\Some: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", counter, someElapsed, someElapsed / 1000, someElapsed / 1000000);
    printf("%lld\r\nAverage: %.2lf ns \r\n%.2lf us\r\n%.2lf ms\r\n", counter, averageElapsed, averageElapsed / 1000, averageElapsed / 1000000);
}