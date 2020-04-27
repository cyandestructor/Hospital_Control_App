#include "Doctor.h"

void Doctor::SetProfessionalID(unsigned long professionalID) {

	m_professionalID = professionalID;

}

std::wstring Doctor::ProfessionalID() {

	return std::to_wstring(m_professionalID);

}

unsigned long Doctor::ULProfessionalID() {

	return m_professionalID;

}

void Doctor::SetImagePath(std::wstring imagePath) {

	m_imagePath = imagePath;

}

void Doctor::SetImagePath(const wchar_t* imagePath) {

	m_imagePath = imagePath;

}

const std::wstring& Doctor::GetImagePath() const {

	return m_imagePath;

}

Schedule& Doctor::GetSchedule() {

	return m_schedule;

}

void Doctor::SetSpeciality(unsigned int key) {

	m_speKey = key;

}

const unsigned int& Doctor::GetSpeciality() const {

	return m_speKey;

}

void Doctor::SetMedOfficeNumber(unsigned int num) {

	m_medOffNum = num;

}

unsigned int Doctor::GetMedOfficeNum() const {

	return m_medOffNum;

}

void Doctor::Write(std::ostream& os) {

	Person::Write(os);
	os.write(reinterpret_cast<char*>(&m_professionalID), sizeof(unsigned long));
	WriteWstring(m_imagePath, os);
	m_schedule.Write(os);
	os.write(reinterpret_cast<char*>(&m_speKey), sizeof(unsigned int));

}

void Doctor::Read(std::istream& is) {

	Person::Read(is);
	is.read(reinterpret_cast<char*>(&m_professionalID), sizeof(unsigned long));
	m_imagePath = ReadWstring(is);
	m_schedule.Read(is);
	is.read(reinterpret_cast<char*>(&m_speKey), sizeof(unsigned int));

}

bool operator==(const Doctor& one, const Doctor& two) {

	return one.m_professionalID == two.m_professionalID;

}

bool operator!=(const Doctor& one, const Doctor& two) {

	return !(one == two);

}

bool operator<(const Doctor& one, const Doctor& two) {

	return one.m_professionalID < two.m_professionalID;

}

bool operator<=(const Doctor& one, const Doctor& two) {

	return !(one > two);

}

bool operator>(const Doctor& one, const Doctor& two) {

	return one.m_professionalID > two.m_professionalID;

}

bool operator>=(const Doctor& one, const Doctor& two) {

	return !(one < two);

}