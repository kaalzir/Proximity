#include <source/simd_solution/SIMDProximityImpl.h>

#include <source/common/Timer.h>
#include <intrin.h>
#include <iterator>

double IProximity::ms_FindInActivatorsElapsed = 0;
double IProximity::ms_FindInActivatorsSIMDElapsed = 0;
double IProximity::ms_FindInActivatorsIfsElapsed = 0;
double IProximity::ms_UpdateInActivators = 0;
uint64_t IProximity::ms_Counter = 0;

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
    uint64_t counter{ 0 };
    ActivatorKeys inActivators;
    ActivatorKeys newInActivators;
    newInActivators.fill(0);
    ActivatorKeys newOutActivators;
    newOutActivators.fill(0);

    for (Trigger& trigger : m_Triggers)
    {
        uint32_t inActivatorsSize{ 0 };
        uint32_t newInActivatorsSize{ 0 };
        uint32_t newOutActivatorsSize{ 0 };
        inActivatorsSize = 0;
        uint32_t activatorsSize{ (uint32_t)m_Activators.size() };
        std::vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
        uint32_t existingActivatorsSize{ (uint32_t)existingKeys.size() };
        uint32_t existingActivatorsSIMDSize{ existingActivatorsSize - existingActivatorsSize % 4 };
        uint64_t counter{};

        __m128i triggerVec = _mm_set1_epi32((trigger.m_Position.m_Y << 16) + trigger.m_Position.m_X);
        __m128i inRangeVec = _mm_set1_epi16(trigger.m_InRange);
        for (uint32_t activatorIndex = 0; activatorIndex < activatorsSize; activatorIndex += 4)
        {
            __m128i activatorVec = _mm_load_si128((__m128i*) & m_Activators[activatorIndex]);

            activatorVec = _mm_sub_epi16(_mm_max_epi16(activatorVec, triggerVec), _mm_min_epi16(activatorVec, triggerVec));
            __m128i compareResultIn = _mm_cmplt_epi16(activatorVec, inRangeVec);
            __m128i compareResultInShifted = _mm_bsrli_si128(compareResultIn, 2);
            __m128i resultMask = _mm_and_si128(compareResultIn, compareResultInShifted);
            __m128i activatorKeysVec = _mm_set_epi32(activatorIndex + 3, activatorIndex + 2, activatorIndex + 1, activatorIndex);
            __m128i inActivatorKeysVec = _mm_and_si128(resultMask, activatorKeysVec);
            __m128i outActivatorKeysVec = _mm_andnot_si128(resultMask, activatorKeysVec);
            for (uint32_t existingActivatorsIndex = 0; existingActivatorsIndex < existingActivatorsSIMDSize; existingActivatorsIndex += 4)
            {
                __m128i existingActivatorVec = _mm_load_si128((__m128i*) & existingKeys[activatorIndex]);
            }
        }
        //IProximity::ms_UpdateInActivators += timer.Elapsed();

        //Here we would send events
        //Or save newInActivators and newOutActivators and send it outside the loop
        //But this is for illustration purposes, so I'm just increasing a counter
        //So that code is not optimized away
        counter += newInActivators[newInActivatorsSize == 0 ? 0 : newInActivatorsSize - 1] + newOutActivators[newOutActivatorsSize == 0 ? 0 : newOutActivatorsSize - 1];
    }
    IProximity::ms_Counter += counter;
}

void SIMDProximityImpl::FindInActivators(const Trigger& trigger, Activators& activators, ActivatorKeys& inActivators, uint32_t& inActivatorsSize)
{
    inActivatorsSize = 0;
    uint32_t activatorsSize{ (uint32_t)activators.size() };
    uint64_t counter{};

    __m128i triggerVec = _mm_set1_epi32((trigger.m_Position.m_Y << 16) + trigger.m_Position.m_X);
    __m128i inRangeVec = _mm_set1_epi16(trigger.m_InRange);
    for (uint32_t activatorIndex = 0; activatorIndex < activatorsSize; activatorIndex += 4)
    {
        __m128i activatorVec = _mm_load_si128((__m128i*) & activators[activatorIndex]);

        activatorVec = _mm_sub_epi16(_mm_max_epi16(activatorVec, triggerVec), _mm_min_epi16(activatorVec, triggerVec));
        __m128i compareResultIn = _mm_cmplt_epi16(activatorVec, inRangeVec);
        __m128i compareResultInShifted = _mm_bsrli_si128(compareResultIn, 2);
        __m128i resultMask = _mm_and_si128(compareResultIn, compareResultInShifted);

        uint64_t lowerBytesMask;
        _mm_storel_epi64((__m128i*) & lowerBytesMask, resultMask);
        uint32_t addedIndex = _mm_popcnt_u64(lowerBytesMask) / 16;
        resultMask = _mm_bsrli_si128(resultMask, 8);

        __m128i activatorKeysVec = _mm_set_epi32(activatorIndex + 3, activatorIndex + 2, activatorIndex + 1, activatorIndex);
        uint64_t lowerBytesAct;
        _mm_storel_epi64((__m128i*) & lowerBytesAct, activatorKeysVec);
        uint64_t lowerresult = _pext_u64(lowerBytesAct, lowerBytesMask);
        __m128i actualResult = _mm_set_epi64x(lowerresult, lowerresult);

        _mm_store_si128((__m128i*) & inActivators[inActivatorsSize], actualResult);
        inActivatorsSize += addedIndex;

        uint64_t upperBytesMask;
        _mm_storel_epi64((__m128i*) & upperBytesMask, resultMask);
        addedIndex = _mm_popcnt_u64(upperBytesMask) / 16;

        activatorKeysVec = _mm_bsrli_si128(activatorKeysVec, 8);
        uint64_t upperBytesAct;
        _mm_storel_epi64((__m128i*) & upperBytesAct, activatorKeysVec);

        uint64_t upperResult = _pext_u64(upperBytesAct, upperBytesMask);
        actualResult = _mm_set_epi64x(upperResult, upperResult);

        _mm_store_si128((__m128i*) & inActivators[inActivatorsSize], actualResult);
        inActivatorsSize += addedIndex;
    }
   /* uint32_t index = 0;
    for (const Activator& activator : activators)
    {
        if (IsInRange(trigger.m_Position, activator.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = index;
        }
        ++index;
    }*/
    
    /*for (uint32_t index = 0; index < activatorsSize; index += 4)
    {
        const Activator& activator1 = activators[index];
        const Activator& activator2 = activators[index + 1];
        const Activator& activator3 = activators[index + 2];
        const Activator& activator4 = activators[index + 3];
        if (IsInRange(trigger.m_Position, activator1.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = index;
        }
        if (IsInRange(trigger.m_Position, activator2.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = index;
        }
        if (IsInRange(trigger.m_Position, activator3.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = index;
        }
        if (IsInRange(trigger.m_Position, activator4.m_Position, trigger.m_InRange))
        {
            inActivators[inActivatorsSize++] = index;
        }
    }*/
}

void SIMDProximityImpl::UpdateInActivators(Trigger& trigger, ActivatorKeys& inActivators, uint32_t& inActivatorsSize, ActivatorKeys& newInActivators, uint32_t& newInActivatorsSize, ActivatorKeys& newOutActivators, uint32_t& newOutActivatorsSize)
{
    newInActivatorsSize = 0;
    newOutActivatorsSize = 0;
    std::vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
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
        if (activatorKey != 0 && std::find(inActivators.begin(), inActivators.begin() + inActivatorsSize, activatorKey) == inActivators.end())
        {
            newOutActivators[newOutActivatorsSize++] = activatorKey;
        }
    }

    existingKeys.clear();
    std::copy(inActivators.begin(), inActivators.begin() + inActivatorsSize, std::back_inserter(existingKeys));
}

void SIMDProximityImpl::Clear()
{
}
