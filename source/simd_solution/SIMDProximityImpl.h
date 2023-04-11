#pragma once
#include <array>
#include <vector>
#include <source/common/IProximity.h>
#include <source/common/ProximityCommon.h>
#include <source/common/Activator.h>
#include <source/common/Trigger.h>

class SIMDProximityImpl : public IProximity
{
	using Triggers = std::array<Trigger, sc_TriggersSize>;
	using Activators = std::array<Activator, sc_ActivatorsSize>;
	using ActivatorKeys = std::array<ActivatorKey, sc_ActivatorsSize>;

public:
	SIMDProximityImpl()
	{
	}

	void CreateTrigger(uint32_t index, Position position, Coordinate inRange) final;
	void CreateActivator(uint32_t index, Position position) final;

	const Trigger& GetTrigger(uint32_t index) const final { return m_Triggers[index]; }
	const Activator& GetActivator(uint32_t index) const final { return m_Activators[index]; }

	void UpdateTrigger(uint32_t index, Position position) final;
	void UpdateActivator(uint32_t index, Position position) final;

	void UpdateProximity() final;

	void Clear() final;

private:
	void FindInActivators(const Trigger& trigger, Activators& activators, ActivatorKeys& inActivators, uint32_t& inActivatorsSize);
	void UpdateInActivators(const Trigger& trigger, ActivatorKeys& inActivators, uint32_t& inActivatorsSize, ActivatorKeys& newInActivators, uint32_t& newInActivatorsSize, ActivatorKeys& newOutActivators, uint32_t& newOutActivatorsSize);

	Triggers m_Triggers;
	Activators m_Activators;
};

