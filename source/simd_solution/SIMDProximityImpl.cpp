#include <source/simd_solution/SIMDProximityImpl.h>

#include <source/common/Timer.h>
#include <intrin.h>

double IProximity::ms_FindInActivatorsElapsed = 0;
double IProximity::ms_FindInActivatorsSIMDElapsed = 0;
double IProximity::ms_FindInActivatorsIfsElapsed = 0;
double IProximity::ms_FindNewInElapsed = 0;

void SIMDProximityImpl::CreateTrigger(uint32_t index, Position position, Coordinate inRange)
{
    m_Triggers[index] = Trigger(position, m_Triggers.size(), inRange);
}

void SIMDProximityImpl::CreateActivator(uint32_t index, Position position)
{
    m_Activators[index] = Activator(position, m_Activators.size());
}

void SIMDProximityImpl::UpdateTrigger(uint32_t index, Position position)
{
    m_Triggers[index].m_Position = position;
}

void SIMDProximityImpl::UpdateActivator(uint32_t index, Position position)
{
    m_Activators[index].m_Position = position;
}

void SIMDProximityImpl::UpdateProximity()
{
    uint32_t counter{ 0 };
    ActivatorKeys inActivators;
    ActivatorKeys newInActivators;
    ActivatorKeys newOutActivators;

    for (Trigger& trigger : m_Triggers)
    {
        uint32_t inActivatorsSize{ 0 };
        uint32_t newInActivatorsSize{ 0 };
        uint32_t newOutActivatorsSize{ 0 };

        FindInActivators(trigger, m_Activators, inActivators, inActivatorsSize);
        UpdateInActivators(trigger, inActivators, inActivatorsSize, newInActivators, newInActivatorsSize, newOutActivators, newOutActivatorsSize);

        //Here we would send events
        //Or save newInActivators and newOutActivators and send it outside the loop
        //But this is for illustration purposes, so I'm just increasing a counter
        //So that code is not optimized away
        counter += newInActivators[newInActivatorsSize - 1] + newOutActivators[newOutActivatorsSize == 0 ? 0 : newOutActivatorsSize - 1];
    }
}

void SIMDProximityImpl::FindInActivators(const Trigger& trigger, Activators& activators, ActivatorKeys& inActivators, uint32_t& inActivatorsSize)
{
    struct alignas(16) SIMDResult
    {
        uint16_t result0;
        uint16_t result1;
        uint16_t result2;
        uint16_t result3;
        uint16_t result4;
        uint16_t result5;
        uint16_t result6;
        uint16_t result7;
    };

    __m128i triggerXVec = _mm_set1_epi16(trigger.m_Position.m_X);
    __m128i triggerYVec = _mm_set1_epi16(trigger.m_Position.m_Y);
    __m128i inRangeVec = _mm_set1_epi16(trigger.m_InRange);
    uint32_t activatorsSize{ (uint32_t)activators.size() };
    for (uint32_t activatorIndex = 0; activatorIndex < activatorsSize; activatorIndex += 8)
    {
        const Position& activatorPos{ activators[activatorIndex].m_Position };
        const Position& activatorPos1{ activators[activatorIndex + 1].m_Position };
        const Position& activatorPos2{ activators[activatorIndex + 2].m_Position };
        const Position& activatorPos3{ activators[activatorIndex + 3].m_Position };
        const Position& activatorPos4{ activators[activatorIndex + 4].m_Position };
        const Position& activatorPos5{ activators[activatorIndex + 5].m_Position };
        const Position& activatorPos6{ activators[activatorIndex + 6].m_Position };
        const Position& activatorPos7{ activators[activatorIndex + 7].m_Position };

        __m128i activatorXVec = _mm_set_epi16(activatorPos.m_X, activatorPos1.m_X, activatorPos2.m_X, activatorPos3.m_X, activatorPos4.m_X, activatorPos5.m_X, activatorPos6.m_X, activatorPos7.m_X);
        
        activatorXVec = _mm_sub_epi16(_mm_max_epi16(activatorXVec, triggerXVec), _mm_min_epi16(activatorXVec, triggerXVec));
        __m128i compareResultXIn = _mm_cmplt_epi16(activatorXVec, inRangeVec);

        __m128i activatorYVec = _mm_set_epi16(activatorPos.m_Y, activatorPos1.m_Y, activatorPos2.m_Y, activatorPos3.m_Y, activatorPos4.m_Y, activatorPos5.m_Y, activatorPos6.m_Y, activatorPos7.m_Y);
        activatorYVec = _mm_sub_epi16(_mm_max_epi16(activatorYVec, triggerYVec), _mm_min_epi16(activatorYVec, triggerYVec));

        __m128i compareResultYIn = _mm_cmplt_epi16(activatorYVec, inRangeVec);

        SIMDResult result;
        _mm_store_si128((__m128i*) &result, _mm_and_si128(compareResultXIn, compareResultYIn));

        if (result.result0)
        {
            inActivators[inActivatorsSize++] = activatorIndex;
        }
        if (result.result1)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 1;
        }
        if (result.result2)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 2;
        }
        if (result.result3)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 3;
        }
        if (result.result4)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 4;
        }
        if (result.result5)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 5;
        }
        if (result.result6)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 6;
        }
        if (result.result7)
        {
            inActivators[inActivatorsSize++] = activatorIndex + 7;
        }
    }

    /*inActivatorsSize = 0;
    for (const Activator& activator : activators)
    {
        if (IsInRange(trigger.m_Position, activator.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = activator.m_Key;
        }
    }*/
}

void SIMDProximityImpl::UpdateInActivators(const Trigger& trigger, ActivatorKeys& inActivators, uint32_t& inActivatorsSize, ActivatorKeys& newInActivators, uint32_t& newInActivatorsSize, ActivatorKeys& newOutActivators, uint32_t& newOutActivatorsSize)
{
    newInActivatorsSize = 0;
    newOutActivatorsSize = 0;
    const std::vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
    uint32_t index{ 0 };
    for (const ActivatorKey& activatorKey : inActivators)
    {
        if (index++ == inActivatorsSize)
        {
            break;
        }
        if (std::find(existingKeys.begin(), existingKeys.end(), activatorKey) == existingKeys.end())
        {
            newInActivators[newInActivatorsSize++] = activatorKey;
        }
    }
    for (const ActivatorKey& activatorKey : existingKeys)
    {
        if (std::find(inActivators.begin(), inActivators.begin() + inActivatorsSize, activatorKey) == inActivators.end())
        {
            newOutActivators[newOutActivatorsSize++] = activatorKey;
        }
    }
}

void SIMDProximityImpl::Clear()
{
}
