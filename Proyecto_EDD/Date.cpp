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

unsigned short Date::WeekDay() const {

	return (YearCode() + MonthCode() + CenturyCode() + m_day - LeapCode()) % 7;

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

	bool leap = IsLeap(year);

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

bool Date::IsLeap(unsigned short year) const {

	bool leap = false;

	if (year > 1700)
		leap = (year % 4 == 0) && ((year % 100 != 0) || (year % 400) == 0);
	else
		leap = year % 4 == 0;

	return leap;

}

unsigned short Date::CenturyCode() const {

	unsigned short codes[] = { 4,2,0,6 };
	unsigned short cCode = 0;

	if (m_year > 1700) {
		unsigned short i = (unsigned short)((m_year - 1700) / 100) % 4;
		cCode = codes[i];
	}
	else {
		unsigned short cNumber = (unsigned short)m_year / 100;
		cCode = (18 - cNumber) % 7;
	}

	return cCode;

}

unsigned short Date::YearCode() const {

	unsigned short ltd = m_year % 100;	//year's last two digits
	unsigned short yCode = (ltd + (unsigned short)(ltd / 4)) % 7;

	return yCode;

}

unsigned short Date::MonthCode() const {

	unsigned short code = 0;

	switch (m_month) {
	case 1:
	case 10:
		code = 0;
		break;
	case 2:
	case 3:
	case 11:
		code = 3;
		break;
	case 4:
	case 7:
		code = 6;
		break;
	case 5:
		code = 1;
		break;
	case 6:
		break;
		code = 4;
	case 8:
		code = 2;
		break;
	case 9:
	case 12:
		code = 5;
		break;
	}

	return code;

}

unsigned short Date::LeapCode() const {

	if (IsLeap(m_year) && (m_month <= 2))
		return 1;
	else
		return 0;
}