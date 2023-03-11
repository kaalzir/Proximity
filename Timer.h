#pragma once

#include <chrono>

using namespace std::chrono;

//Timer class for benchmarking
class Timer
{
public: 
	Timer()
	{
		Reinit();
	}

	void Reinit()
	{
		m_StartTimepoint = high_resolution_clock::now();
	}

	//returns time elapsed in nanoseconds
	double Elapsed() const
	{
		time_point<high_resolution_clock> elapsed{ high_resolution_clock::now() - m_StartTimepoint };
		return time_point_cast<nanoseconds>(elapsed).time_since_epoch().count();
	}

private:
	time_point<high_resolution_clock> m_StartTimepoint;
};