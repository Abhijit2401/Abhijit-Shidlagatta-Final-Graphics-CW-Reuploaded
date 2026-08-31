#pragma once

#include <windows.h>

// Timer in milliseconds with high resolution
class CHighResolutionTimer
{
public:
	CHighResolutionTimer();
	~CHighResolutionTimer();

	void Start();
	double Elapsed();

private:
	LARGE_INTEGER m_t1;
	LARGE_INTEGER m_frequency;
	bool m_started;
};