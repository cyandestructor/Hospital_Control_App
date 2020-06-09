#include "Time.h"

Time::Time() :
	m_hour(0), m_minute(0) {}

Time::Time(unsigned short hour, unsigned short minute) {

	SetTime(hour, minute);

}

Time::Time(const SYSTEMTIME& systemTime) {

	SetTime(systemTime.wHour, systemTime.wMinute);

}

void Time::SetTime(unsigned short hour, unsigned short minute) {

	if (hour >= 0 && hour <= 23) {
		m_hour = hour;
	}

	if (minute >= 0 && hour <= 59) {
		m_minute = minute;
	}

}

unsigned short Time::AddTime(unsigned short hour, unsigned short minute) {

	unsigned short addHour = (unsigned short)(minute / 60);
	unsigned short addMinute = minute % 60;
	unsigned short addDay = (unsigned short)(hour / 24);

	addHour = (addHour + hour) % 24;
	//addDay = (unsigned short)(addHour / 24);

	m_minute += addMinute;
	if (m_minute >= 60) {
		addHour++;
		m_minute -= 60;
	}

	m_hour += addHour;
	if (m_hour >= 24) {
		addDay++;
		m_hour -= 24;
	}

	return addDay;

}

unsigned short Time::Hour() const {

	return m_hour;

}

unsigned short Time::Minute() const {

	return m_minute;

}

short Time::CompareTimes(const Time& one, const Time& two) {
	
	if (one.Hour() > two.Hour()) {
		return 1;
	}
	else if (one.Hour() == two.Hour()) {
		if (one.Minute() > two.Minute()) {
			return 1;
		}
		else if (one.Minute() == two.Minute()) {
			return 0;
		}
	}

	return -1;

}