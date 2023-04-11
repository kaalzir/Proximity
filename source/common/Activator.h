#pragma once

#include <source/common/Position.h>
#include <source/common/ProximityCommon.h>

struct Activator
{
    Activator() = default;

    Activator(Position position, ActivatorKey key)
        : m_Position(position)
    {
    }

    Activator(const Activator& other)
        : m_Position(other.m_Position)
    {
    }

    bool operator<(const Activator& other)
    {
        return m_Position.m_X < other.m_Position.m_X || (m_Position.m_X == other.m_Position.m_X && m_Position.m_Y < other.m_Position.m_Y);
    }

    Position m_Position{};
};