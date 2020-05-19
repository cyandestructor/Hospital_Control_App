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

void DateTime::AddTime(unsigned short day, unsigned short month, unsigned short year,
	unsigned short hour, unsigned short minute) {

	unsigned short addDays = Time::AddTime(hour, minute);
	Date::AddTime(day + addDays, month, year);

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