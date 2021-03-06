#ifndef _SCHEDULE_CLASS_
#define _SCHEDULE_CLASS_

#include <vector>
#include <fstream>
#include "DateTime.h"
#include "TimePeriod.h"

enum class SchValidate {
	SCH_WORKDAY, SCH_HOUR, SCH_HOUR_WORKDAY, SCH_TIME, SCH_ALL
};

class Schedule {

public:
	Schedule();
	void SetWorkDay(unsigned short weekDay, bool isWorkDay);
	bool IsWorkDay(unsigned short weekDay) const;
	const bool* GetWorkDays() const { return m_workDays; }
	const std::vector<TimePeriod>& GetReservedTime() const { return m_reservedTime; }
	bool ConflictWith(const Schedule& other, SchValidate validate) const;
	bool IsReserved(const DateTime& from, const DateTime& to) const;
	bool IsAvailable(const DateTime& from, const DateTime& to) const;
	void Reserve(const DateTime& from, const DateTime& to);
	void SetFree(const DateTime& from, const DateTime& to);
	void Free();
	void Write(ostream& os);
	void Read(istream& is);

private:
	std::vector<TimePeriod> m_reservedTime;
	bool m_workDays[7];

};

#endif