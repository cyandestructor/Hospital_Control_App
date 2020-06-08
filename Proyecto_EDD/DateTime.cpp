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

std::wstring DateTime::DateTimeWstring() const {
	
	wchar_t buffer[30] = L"";
	swprintf_s(buffer, 30, L"%.2u/%.2u/%u %.2u:%.2u", m_day, m_month, m_year, m_hour, m_minute);

	return std::wstring(buffer);

}

const DateTime DateTime::RightNow() {

	SYSTEMTIME aux;
	GetLocalTime(&aux);

	DateTime rightNow(aux);

	return rightNow;

}

const DateTime DateTime::DateTimeAdd(const DateTime& orig, 
	unsigned short day, unsigned short month, unsigned short year,
	unsigned short hour, unsigned short minute) {

	DateTime dt = orig;

	dt.AddTime(day, month, year, hour, minute);

	return dt;

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

std::ostream& operator<<(std::ostream& os, const DateTime& dt) {

	os << dt.m_day << "/" << dt.m_month << "/" << dt.m_year << " "
		<< std::setfill('0') << std::setw(2) << dt.m_hour << std::setw(1) << ":" << std::setw(2) << dt.m_minute;

	return os;
}

std::wostream& operator<<(std::wostream& os, const DateTime& dt) {

	os << dt.m_day << L"/" << dt.m_month << L"/" << dt.m_year << L" "
		<< std::setfill(L'0') << std::setw(2) << dt.m_hour << std::setw(1) << L":" << std::setw(2) << dt.m_minute;

	return os;
}