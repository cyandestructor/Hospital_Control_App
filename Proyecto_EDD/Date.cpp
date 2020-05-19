#include "Date.h"

Date::Date() {

	SetToday();

}

Date::Date(unsigned short day, unsigned short month, unsigned short year) {

	if (!SetDate(day, month, year))
		SetToday();

}

Date::Date(const SYSTEMTIME& systemTime) {

	if (!SetDate(systemTime.wDay, systemTime.wMonth, systemTime.wYear))
		SetToday();

}

bool Date::SetDate(unsigned short day, unsigned short month, unsigned short year) {

	bool fine = true;

	if (month <= 0 || month > 12)
		fine = false;
	else {
		unsigned short dpm = DaysPerMonth(month, year);
		if (day <= 0 || day > dpm)
			fine = false;
	}

	if (fine) {
		m_day = day;
		m_month = month;
		m_year = year;
	}

	return fine;

}

bool Date::SetDate(const SYSTEMTIME& systemTime) {

	return SetDate(systemTime.wDay, systemTime.wMonth, systemTime.wYear);

}

void Date::SetToday() {

	SYSTEMTIME rightNow;
	GetSystemTime(&rightNow);

	m_day = rightNow.wDay;
	m_month = rightNow.wMonth;
	m_year = rightNow.wYear;

}

void Date::AddTime(unsigned short days, unsigned short months, unsigned short years) {

	bool mCorrect = false, dCorrect = false;
	//Start by adding the days
	m_day += days;
	while (!dCorrect) {
		dCorrect = AddDays();
	}
	//The day is correct, now add the months
	m_month += months;
	while (!mCorrect) {
		mCorrect = AddMonths();
	}
	//Finally add the years
	m_year += years;
	//Do extra corrections if needed
	ValidateDate();

}

unsigned short Date::Day() const {

	return m_day;

}

unsigned short Date::Month() const {

	return m_month;

}

unsigned short Date::Year() const {

	return m_year;

}

unsigned short Date::DaysPerMonth(unsigned short month, unsigned short year) {

	/*
	30 days: april (4), june (6), september (9), november (11)
	31 days: january (1), march (3), may (5), july (7), august (8), october (10), december (12)
	28/29 days: february
	*/

	bool leap = year % 4 == 0;

	switch (month) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
		break;
	case 2:
		if (leap)
			return 29;
		else
			return 28;
		break;
	default:
		return 0;
	}

}

bool Date::AddDays() {

	//Do the verification process when days are added to the date (it works within a loop)
	unsigned short dpm = DaysPerMonth(m_month, m_year);	//Get the total number of days within the current date's month
	bool flag = false;

	if (m_day <= dpm)
		flag = true;	//If the current day is a valid day number for the current month
	else
		m_day -= dpm;	//Substract the number of dpm for the current month to the current day

	if (!flag) {
		m_month++;		//Still not a correct day number, the previous operation if false, implies an increase in month
		AddMonths();
	}

	return flag;

}

bool Date::AddMonths() {

	//Do the verification process when months are added to the date (it works within a loop)
	if (m_month <= 12)
		return true;	//The month is correct
	else {
		m_month -= 12;	//Substract 12 to the current month to add a year
		m_year++;
	}

	return false;		//The month is still invalid

}

void Date::ValidateDate() {

	/*
	30 days: april, june, september, november
	31 days: january, march, may, july, august, october, december
	28/29 days: february
	*/

	unsigned short dpm = DaysPerMonth(m_month, m_year);
	if (m_day > dpm) {
		unsigned short add = dpm - m_day;
		m_day -= add;
		AddTime(add, 0, 0);
	}

}