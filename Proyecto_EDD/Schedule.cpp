#include "Schedule.h"

Schedule::Schedule() {

	m_workDays[0] = false;	//Sunday
	m_workDays[1] = false;	//Monday
	m_workDays[2] = false;	//Tuesday
	m_workDays[3] = false;	//Wednesday
	m_workDays[4] = false;	//Thursday
	m_workDays[5] = false;	//Friday
	m_workDays[6] = false;	//Saturday

}

void Schedule::SetWorkDay(unsigned short weekDay, bool isWorkDay) {

	if (weekDay >= 0 && weekDay < 7) {
		m_workDays[weekDay] = isWorkDay;
	}

}

bool Schedule::IsWorkDay(unsigned short weekDay) const {

	if (weekDay >= 0 && weekDay < 7) {
		return m_workDays[weekDay];
	}
	else {
		return false;
	}

}

void Schedule::Reserve(const DateTime& from, const DateTime& to) {

	//TimePeriod timePeriod(from, to);
	//m_reservedTime.push_back(timePeriod);
	m_reservedTime.emplace_back(from, to);

}

void Schedule::SetFree(const DateTime& from, const DateTime& to) {

	for (unsigned int i = 0; i < m_reservedTime.size(); i++) {
		if (m_reservedTime[i].Begin() == from && m_reservedTime[i].End() == to) {
			m_reservedTime.erase(m_reservedTime.begin() + i);	//Erase the found element
			break;
		}
	}

}

void Schedule::Free() {

	for (bool day : m_workDays)
		day = false;

	m_reservedTime.clear();

}

bool Schedule::ConflictWith(const Schedule& other, SchValidate validate) const {

	bool timeConfict = false, workDayConflict = false, hourConflict = false;

	if (validate == SchValidate::SCH_TIME || validate == SchValidate::SCH_ALL) {
		//Check if there is any conflict between both schedules times
		for (auto& time : m_reservedTime) {
			if (other.IsReserved(time.Begin(), time.End())) {
				timeConfict = true;
				break;
			}
		}
	}

	if (validate == SchValidate::SCH_HOUR || validate == SchValidate::SCH_HOUR_WORKDAY) {
		//check if there is any conflict between both schedule times (hour)
		auto& otherReserved = other.GetReservedTime();
		for (auto& time : m_reservedTime) {
			for (int i = 0; i < otherReserved.size(); i++) {
				if (time.Begin().Hour() >= otherReserved[i].Begin().Hour() &&
					time.Begin().Minute() >= otherReserved[i].Begin().Minute() &&
					time.End().Hour() < otherReserved[i].End().Hour() &&
					time.End().Minute() < otherReserved[i].End().Minute()) {
					hourConflict = true;
				}
			}
		}
	}

	if (validate == SchValidate::SCH_WORKDAY || validate == SchValidate::SCH_ALL
		|| validate == SchValidate::SCH_HOUR_WORKDAY) {
		//Check if there is any conflict between both schedules workdays
		for (unsigned short i = 0; i < 7; i++) {
			if (m_workDays[i] == other.IsWorkDay(i)) {
				workDayConflict = true;
				break;
			}
		}
	}

	switch (validate) {
	case SchValidate::SCH_TIME:
		return timeConfict;							//There is a conflict in time
		break;
	case SchValidate::SCH_WORKDAY:
		return workDayConflict;						//There is a conflict in workdays
		break;
	case SchValidate::SCH_ALL:
		return timeConfict && workDayConflict;		//There is a conflict in time and workday
		break;
	case SchValidate::SCH_HOUR:
		return hourConflict;
		break;
	case SchValidate::SCH_HOUR_WORKDAY:
		return hourConflict && workDayConflict;
	default:
		return false;
	}

}

bool Schedule::IsReserved(const DateTime& from, const DateTime& to) const {

	TimePeriod temp(from, to);
	for (unsigned int i = 0; i < m_reservedTime.size(); i++) {
		if (TimePeriod::TimePeriodsOverlap(temp, m_reservedTime[i]))
			return true;
	}

	return false;
}

bool Schedule::IsAvailable(const DateTime& from, const DateTime& to) const {

	return !IsReserved(from, to);

}

void Schedule::Write(ostream& os) {

	int size = m_reservedTime.size();
	os.write(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < size; i++) {
		os.write(reinterpret_cast<char*>(&m_reservedTime[i]), sizeof(TimePeriod));
	}

	os.write(reinterpret_cast<char*>(&m_workDays), sizeof(m_workDays));

}

void Schedule::Read(istream& is) {

	int size = 0;
	is.read(reinterpret_cast<char*>(&size), sizeof(int));
	for (int i = 0; i < size; i++) {
		TimePeriod tp;
		is.read(reinterpret_cast<char*>(&tp), sizeof(TimePeriod));
		m_reservedTime.push_back(tp);
	}

	is.read(reinterpret_cast<char*>(&m_workDays), sizeof(m_workDays));

}