#pragma once

#include <source/common/Activator.h>
#include <source/common/Trigger.h>
#include <source/common/ProximityCommon.h>
#include <source/common/Position.h>

class IProximity
{
public:
	virtual void CreateTrigger(uint32_t index, Position position, Coordinate inRange) = 0;
	virtual void CreateActivator(uint32_t index, Position position) = 0;

	virtual const Trigger& GetTrigger(uint32_t index) const = 0;
	virtual const Activator& GetActivator(uint32_t index) const = 0;

	virtual void UpdateTrigger(uint32_t index, Position position) = 0;
	virtual void UpdateActivator(uint32_t index, Position position) = 0;

	virtual void UpdateProximity() = 0;

	virtual void Clear() = 0;

	static double ms_FindInActivatorsElapsed;
	static double ms_FindInActivatorsSIMDElapsed;
	static double ms_FindInActivatorsIfsElapsed;
	static double ms_FindNewInElapsed;
};