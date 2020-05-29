#ifndef _DATE_TIME_CLASS_
#define _DATE_TIME_CLASS_

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include "Date.h"
#include "Time.h"

class DateTime : public Date, public Time {

public:
	DateTime() : Date(), Time() {}
	DateTime(const SYSTEMTIME& systemTime) : Date(systemTime), Time(systemTime) {}
	DateTime(unsigned short day, unsigned short month, unsigned short year,
		unsigned short hour, unsigned short minute) : Date(day, month, year), Time(hour, minute) {}
	SYSTEMTIME GetSystemTimeStruct() const;
	void AddTime(unsigned short day, unsigned short month, unsigned short year,
		unsigned short hour, unsigned short minute);
	int CompareWith(const DateTime& other) const;

	static const DateTime RightNow();
	static const DateTime DateTimeAdd(const DateTime& orig, 
		unsigned short day, unsigned short month, unsigned short year,
		unsigned short hour, unsigned short minute);

	friend std::ostream& operator<<(std::ostream& os, const DateTime& dt);
	friend bool operator==(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator!=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>=(const DateTime& firstDateTime, const DateTime& secondDateTime);

};

std::ostream& operator<<(std::ostream& os, const DateTime& dt) {

	os << dt.m_day << "/" << dt.m_month << "/" << dt.m_year << " "
		<< std::setfill('0') << std::setw(2) << dt.m_hour << std::setw(1) << ":" << std::setw(2) << dt.m_minute;

	return os;
}

#endif