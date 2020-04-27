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

void Time::AddTime(unsigned short hour, unsigned short minute) {

	short addHour = (short)(minute / 60);
	short addMinute = minute % 60;

	addHour = (addHour + hour) % 24;

	m_minute += addMinute;
	if (m_minute >= 60) {
		addHour++;
		m_minute -= 60;
	}

	m_hour += addHour;
	if (m_hour >= 24) {
		m_hour -= 24;
	}

}

unsigned short Time::Hour() const {

	return m_hour;

}

unsigned short Time::Minute() const {

	return m_minute;

}