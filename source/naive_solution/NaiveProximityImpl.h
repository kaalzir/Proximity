#pragma once

#include <vector>
#include <source/common/IProximity.h>
#include <source/common/ProximityCommon.h>
#include <source/common/Activator.h>
#include <source/common/Trigger.h>

using Triggers = std::vector<Trigger>;
using Activators = std::vector<Activator>;
using ActivatorKeys = std::vector<ActivatorKey>;

class NaiveProximityImpl : public IProximity
{
public:
	NaiveProximityImpl()
	{
		m_Triggers.reserve(sc_TriggersSize);
		m_Activators.reserve(sc_ActivatorsSize);
	}

	void CreateTrigger(Position position, Coordinate inRange) final;
	void CreateActivator(Position position) final;

	const Trigger& GetTrigger(uint32_t index) const final { return m_Triggers[index]; }
	const Activator& GetActivator(uint32_t index) const final { return m_Activators[index]; }

	void UpdateTrigger(uint32_t index, Position position) final;
	void UpdateActivator(uint32_t index, Position position) final;

	void UpdateProximity() final;

	void Clear() final;

private:
	void FindInActivators(const Trigger& trigger, Activators& activators, ActivatorKeys& inActivators);
	void UpdateInActivators(const Trigger& trigger, ActivatorKeys& inActivators, ActivatorKeys& newInActivators, ActivatorKeys& newOutActivators);

	Triggers m_Triggers;
	Activators m_Activators;
};