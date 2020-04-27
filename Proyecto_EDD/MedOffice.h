#ifndef _MED_OFFICE_CLASS_
#define _MED_OFFICE_CLASS_

#include <iostream>
#include "Schedule.h"

class MedOffice {

public:
	MedOffice();
	Schedule& GetSchedule();
	void SetNumber(const unsigned int number);
	unsigned int Number() const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

private:
	unsigned int m_number;
	Schedule m_schedule;

};

#endif