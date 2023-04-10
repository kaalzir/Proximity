#pragma once

#include <source/common/Position.h>
#include <source/common/ProximityCommon.h>
#include <vector>

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

    std::vector<ActivatorKey> m_ActivatorKeys{};
    Position m_Position{};
    TriggerKey m_Key{};
    Coordinate m_InRange{};
    uint64_t m_Padding[2]{};
};