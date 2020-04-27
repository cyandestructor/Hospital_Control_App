#include "Date.h"

Date::Date() {

	SYSTEMTIME rightNow;
	GetSystemTime(&rightNow);

	m_day = rightNow.wDay;
	m_month = rightNow.wMonth;
	m_year = rightNow.wYear;

}

Date::Date(unsigned short day, unsigned short month, unsigned short year){

	SetDate(day, month, year);

}

Date::Date(const SYSTEMTIME& systemTime) {

	SetDate(systemTime.wDay, systemTime.wMonth, systemTime.wYear);

}

void Date::SetDate(unsigned short day, unsigned short month, unsigned short year) {

	if (month > 0 && month <= 12) {

		m_month = month;

		if (day > 0) {

			if (month == 1 ||
				month == 3 ||
				month == 5 ||
				month == 7 ||
				month == 8 ||
				month == 10 ||
				month == 12) {

				if (day <= 31) {
					m_day = day;
				}

			}
			else {

				if (month == 2 && day <= 29) {
					m_day = day;
				}
				
				if (month != 2 && day <= 30) {
					m_day = day;
				}

			}

			if (year > 0) {
				m_year = year;
			}
		}

	}

}

void Date::SetDate(const SYSTEMTIME& systemTime) {

	SetDate(systemTime.wDay, systemTime.wMonth, systemTime.wYear);

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