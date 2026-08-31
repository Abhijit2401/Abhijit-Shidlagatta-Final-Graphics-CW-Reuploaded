#include "HighResolutionTimer.h"

CHighResolutionTimer::CHighResolutionTimer() : m_started(false)
{
	QueryPerformanceFrequency(&m_frequency);
}

CHighResolutionTimer::~CHighResolutionTimer()
{
}

void CHighResolutionTimer::Start()
{
	m_started = true;
	QueryPerformanceCounter(&m_t1);
}

double CHighResolutionTimer::Elapsed()
{
	if (!m_started) return 0.0;
	LARGE_INTEGER t2;
	QueryPerformanceCounter(&t2);
	return (double)(t2.QuadPart - m_t1.QuadPart) * 1000.0 / m_frequency.QuadPart;
}