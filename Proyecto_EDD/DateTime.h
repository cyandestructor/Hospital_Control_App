#ifndef _DATE_TIME_CLASS_
#define _DATE_TIME_CLASS_

#include <Windows.h>
#include "Date.h"
#include "Time.h"

class DateTime : public Date, public Time {

public:
	DateTime() : Date(), Time() {}
	DateTime(const SYSTEMTIME& systemTime) : Date(systemTime), Time(systemTime) {}
	SYSTEMTIME GetSystemTimeStruct() const;
	void AddTime(unsigned short hour, unsigned short minute) override;
	int CompareWith(const DateTime& other) const;

	static const DateTime RightNow();

	friend bool operator==(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator!=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator<=(const DateTime& firstDateTime, const DateTime& secondDateTime);
	friend bool operator>=(const DateTime& firstDateTime, const DateTime& secondDateTime);

};

#endif