#ifndef _TIME_CLASS_
#define _TIME_CLASS_

#include <string>
#include <Windows.h>

using namespace std;

class Time {

public:
	Time();
	Time(unsigned short hour, unsigned short minute);
	Time(const SYSTEMTIME& systemTime);
	void SetTime(unsigned short hour, unsigned short minute);
	unsigned short AddTime(unsigned short hour, unsigned short minute);
	unsigned short Hour() const;
	unsigned short Minute() const;

protected:
	unsigned short m_hour;
	unsigned short m_minute;

};

#endif