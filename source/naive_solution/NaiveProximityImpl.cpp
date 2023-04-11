#include <source/naive_solution/NaiveProximityImpl.h>

void NaiveProximityImpl::CreateTrigger(uint32_t index, Position position, Coordinate inRange)
{
	m_Triggers.emplace_back(position, m_Triggers.size(), inRange);
}

void NaiveProximityImpl::CreateActivator(uint32_t index, Position position)
{
	//m_Activators.emplace_back(position, m_Activators.size());
}

void NaiveProximityImpl::UpdateTrigger(uint32_t index, Position position)
{
	m_Triggers[index].m_Position = position;
}

void NaiveProximityImpl::UpdateActivator(uint32_t index, Position position)
{
	m_Activators[index].m_Position = position;
}

void NaiveProximityImpl::UpdateProximity()
{
    uint32_t counter{ 0 };
    for (Trigger& trigger : m_Triggers)
    {
        ActivatorKeys inActivators;
        FindInActivators(trigger, m_Activators, inActivators);
        ActivatorKeys newInActivators;
        ActivatorKeys newOutActivators;
        UpdateInActivators(trigger, inActivators, newInActivators, newOutActivators);

        //Here we would send events
        //Or save newInActivators and newOutActivators and send it outside the loop
        //But this is for illustration purposes, so I'm just increasing a counter
        //So that code is not optimized away
        counter += newInActivators[newInActivators.size() - 1] + newOutActivators[newOutActivators.size() - 1];
    }
}

void NaiveProximityImpl::FindInActivators(const Trigger& trigger, Activators& activators, ActivatorKeys& inActivators)
{
    for (const Activator& activator : activators)
    {
        if (IsInRange(trigger.m_Position, activator.m_Position, trigger.m_InRange))
        {
            //inActivators.push_back(activator.m_Key);
        }
    }
}

void NaiveProximityImpl::UpdateInActivators(const Trigger& trigger, ActivatorKeys& inActivators, ActivatorKeys& newInActivators, ActivatorKeys& newOutActivators)
{
    const std::vector<ActivatorKey>& existingKeys{ trigger.m_ActivatorKeys };
    for (const ActivatorKey& activatorKey : inActivators)
    {
        if (std::find(existingKeys.begin(), existingKeys.end(), activatorKey) == existingKeys.end())
        {
            newInActivators.push_back(activatorKey);
        }
    }
    for (const ActivatorKey& activatorKey : existingKeys)
    {
        if (std::find(inActivators.begin(), inActivators.end(), activatorKey) == inActivators.end())
        {
            newOutActivators.push_back(activatorKey);
        }
    }
}

void NaiveProximityImpl::Clear()
{
    m_Triggers.clear();
    m_Activators.clear();
}
