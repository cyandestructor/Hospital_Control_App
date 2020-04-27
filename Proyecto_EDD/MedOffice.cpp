#include "MedOffice.h"

MedOffice::MedOffice() {

	m_number = 0;

}

Schedule& MedOffice::GetSchedule() {

	return m_schedule;

}

void MedOffice::SetNumber(const unsigned int number) {

	m_number = number;

}

unsigned int MedOffice::Number() const {

	return m_number;

}

void MedOffice::Write(std::ostream& os) {

	os.write(reinterpret_cast<char*>(&m_number), sizeof(unsigned int));
	m_schedule.Write(os);

}

void MedOffice::Read(std::istream& is) {

	is.read(reinterpret_cast<char*>(&m_number), sizeof(unsigned int));
	m_schedule.Read(is);

}