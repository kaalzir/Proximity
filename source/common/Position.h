#pragma once
#include <cstdlib>
#include <source/common/ProximityCommon.h>


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
    return IsInRange(first.m_X, second.m_X, inRange) && IsInRange(first.m_Y, second.m_Y, inRange);
}

inline Coordinate RandomCoordinate(Coordinate minCoordinate, Coordinate maxCoordinate)
{
    return rand() % (maxCoordinate - minCoordinate + 1) + minCoordinate;
}