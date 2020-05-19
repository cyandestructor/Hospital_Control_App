#ifndef _DATE_CLASS_
#define _DATE_CLASS_

#include <Windows.h>

class Date {

public:
	Date();
	Date(unsigned short day, unsigned short month, unsigned short year);
	Date(const SYSTEMTIME& systemTime);
	bool SetDate(unsigned short day, unsigned short month, unsigned short year);
	bool SetDate(const SYSTEMTIME& systemTime);
	void SetToday();
	virtual void AddTime(unsigned short days, unsigned short months, unsigned short years);
	unsigned short Day() const;
	unsigned short Month() const;
	unsigned short Year() const;

protected:
	unsigned short m_day;
	unsigned short m_month;
	unsigned short m_year;

	unsigned short DaysPerMonth(unsigned short month, unsigned short year);
	bool AddDays();
	bool AddMonths();
	virtual void ValidateDate();

};

#endif