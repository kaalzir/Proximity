#pragma once

#include <source/common/Position.h>
#include <source/common/ProximityCommon.h>

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