#ifndef _DATE_TIME_CLASS_
#define _DATE_TIME_CLASS_

#include <Windows.h>
#include <iostream>
#include <string>
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
	std::wstring DateTimeWstring() const;

	static const DateTime RightNow();
	static const DateTime DateTimeAdd(const DateTime& orig, 
		unsigned short day, unsigned short month, unsigned short year,
		unsigned short hour, unsigned short minute);

	friend std::ostream& operator<<(std::ostream& os, const DateTime& dt);
	friend std::wostream& operator<<(std::wostream& os, const DateTime& dt);
	friend bool operator==(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator!=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>=(const DateTime& firstDateTime, const DateTime& secondDateTime);

};

#endif