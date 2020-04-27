#ifndef _TIME_PERIOD_CLASS
#define _TIME_PERIOD_CLASS

#include "DateTime.h"

class TimePeriod {

public:
	TimePeriod() : m_begin(), m_end() {}
	TimePeriod(DateTime begin, DateTime end);
	void SetBegin(const DateTime& begin);
	void SetEnd(const DateTime& end);
	const DateTime& Begin() const { return m_begin; }
	const DateTime& End() const { return m_end; }

	static bool TimePeriodsOverlap(const TimePeriod& first, const TimePeriod& second);

private:
	DateTime m_begin;
	DateTime m_end;

};

#endif