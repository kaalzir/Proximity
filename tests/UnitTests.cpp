#include <tests/UnitTests.h>

#include <source/common/ProximityCommon.h>

bool RunNewActivatorJoinedTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunNewTriggerJoinedTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunActivatorLeftTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunTriggerLeftTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunTriggerMovingTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunActivatorsMovingTest(IProximity& proximity)
{
	proximity.Clear();
	return false;
}

bool RunUnitTests(IProximity& proximity)
{
	bool result{ true };
	result &= RunNewActivatorJoinedTest(proximity);
	result &= RunNewTriggerJoinedTest(proximity);
	result &= RunActivatorLeftTest(proximity);
	result &= RunTriggerLeftTest(proximity);
	result &= RunTriggerMovingTest(proximity);
	result &= RunActivatorsMovingTest(proximity);
	return result;
}