#include "TimePeriod.h"

TimePeriod::TimePeriod(DateTime begin, DateTime end) {

	if (begin < end) {
		m_begin = begin;
		m_end = end;
	}
	else {
		m_begin = end;
		m_end = begin;
	}

}

void TimePeriod::SetBegin(const DateTime& begin) {

	if (begin < m_end) {
		m_begin = begin;
	}
	else {
		m_begin = m_end;
		m_end = begin;
	}

}

void TimePeriod::SetEnd(const DateTime& end) {

	if (end > m_begin) {
		m_end = end;
	}
	else {
		m_end = m_begin;
		m_begin = end;
	}

}

bool TimePeriod::TimePeriodsOverlap(const TimePeriod& first, const TimePeriod& second) {

	if ((first.Begin() < second.Begin() && first.End() < second.Begin()) ||
		(first.Begin() > second.End() && first.End() > second.End())) {
		return false;
	}
	else {
		return true;
	}

}