#ifndef _DATE_CLASS_
#define _DATE_CLASS_

#include <Windows.h>

class Date {

public:
	Date();
	Date(unsigned short day, unsigned short month, unsigned short year);
	Date(const SYSTEMTIME& systemTime);
	void SetDate(unsigned short day, unsigned short month, unsigned short year);
	void SetDate(const SYSTEMTIME& systemTime);
	unsigned short Day() const;
	unsigned short Month() const;
	unsigned short Year() const;

protected:
	unsigned short m_day;
	unsigned short m_month;
	unsigned short m_year;

};

#endif