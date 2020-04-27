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

bool Schedule::IsWorkDay(unsigned short weekDay) {

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

bool Schedule::IsReserved(const DateTime& from, const DateTime& to) {

	TimePeriod temp(from, to);
	for (unsigned int i = 0; i < m_reservedTime.size(); i++) {
		if (TimePeriod::TimePeriodsOverlap(temp, m_reservedTime[i]))
			return true;
	}

	return false;
}

bool Schedule::IsAvailable(const DateTime& from, const DateTime& to) {

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