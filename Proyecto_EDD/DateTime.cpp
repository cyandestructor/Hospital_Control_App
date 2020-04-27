#include "DateTime.h"

SYSTEMTIME DateTime::GetSystemTimeStruct() const {

	SYSTEMTIME dateTime;
	memset(&dateTime, 0, sizeof(SYSTEMTIME));
	dateTime.wDay = m_day;
	dateTime.wMonth = m_month;
	dateTime.wYear = m_year;
	dateTime.wHour = m_hour;
	dateTime.wMinute = m_minute;

	return dateTime;

}

void DateTime::AddTime(unsigned short hour, unsigned short minute) {

	short addHour = (short)(minute / 60);
	short addMinute = minute % 60;
	short addDay = 0;

	addHour = (addHour + hour) % 24;
	addDay = (short)(addHour / 24);

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

	m_day += addDay;

}

int DateTime::CompareWith(const DateTime& other) const {

	FILETIME thisDate, otherDate;
	SystemTimeToFileTime(&(this->GetSystemTimeStruct()), &thisDate);
	SystemTimeToFileTime(&(other.GetSystemTimeStruct()), &otherDate);

	return (int)CompareFileTime(&thisDate, &otherDate);

}

const DateTime DateTime::RightNow() {

	SYSTEMTIME aux;
	GetLocalTime(&aux);

	DateTime rightNow(aux);

	return rightNow;

}

bool operator<(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	if (firstDateTime.CompareWith(secondDateTime) == -1) {
		return true;
	}
	else {
		return false;
	}

}

bool operator>(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	return !(firstDateTime < secondDateTime);

}

bool operator==(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	if (firstDateTime.CompareWith(secondDateTime) == 0) {
		return true;
	}
	else {
		return false;
	}

}

bool operator!=(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	return !(firstDateTime == secondDateTime);

}

bool operator<=(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	return (firstDateTime < secondDateTime) || (firstDateTime == secondDateTime);

}

bool operator>=(const DateTime& firstDateTime, const DateTime& secondDateTime) {

	return (firstDateTime > secondDateTime) || (firstDateTime == secondDateTime);

}