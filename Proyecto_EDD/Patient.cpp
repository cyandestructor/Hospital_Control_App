#include "Patient.h"

void Patient::SetHeight(unsigned short height) {

	m_height = height;

}

unsigned short Patient::Height() const {

	return m_height;

}

void Patient::SetWeigth(float weight) {

	if (weight >= 0) {
		m_weight = weight;
	}
	else {
		m_weight = weight * -1;
	}

}

float Patient::Weight() const {

	return m_weight;

}

void Patient::SetBloodType(BloodType bloodType) {

	m_bloodType = bloodType;

}

BloodType Patient::GetBloodType() const {

	return m_bloodType;

}

std::wstring Patient::GetBloodTypeString() const {

	wstring bloodType;

	switch (m_bloodType) {
	case BloodType::posA:
		bloodType = L"A+";
		break;
	case BloodType::negA:
		bloodType = L"A-";
		break;
	case BloodType::posB:
		bloodType = L"B+";
		break;
	case BloodType::negB:
		bloodType = L"B-";
		break;
	case BloodType::posO:
		bloodType = L"O+";
		break;
	case BloodType::negO:
		bloodType = L"O-";
		break;
	case BloodType::posAB:
		bloodType = L"AB+";
		break;
	case BloodType::negAB:
		bloodType = L"AB-";
		break;
	default:
		bloodType = L"UNDEFINED";
	}

	return bloodType;

}

void Patient::SetReference(const std::wstring& reference) {

	m_reference = reference;

}

void Patient::SetReference(const wchar_t* reference) {

	m_reference = reference;

}

const std::wstring& Patient::Reference() const {

	return m_reference;

}

void Patient::SetDoctor(unsigned long doctor) {

	m_doctor = doctor;

}

unsigned long Patient::GetDoctor() const {

	return m_doctor;

}

unsigned int Patient::Key() const {

	if (m_key != 0)
		return m_key;
	else
		return GenKey();

}

unsigned int Patient::GenKey() const {

	Date aux;
	unsigned int code = (unsigned int)(m_height + m_weight + (int)m_bloodType);
	code += this->Birthdate().Day();
	code += aux.Day() + aux.Month() + aux.Year();
	m_key = code;

	return m_key;

}

void Patient::Write(std::ostream& os) {

	Person::Write(os);
	os.write(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_height), sizeof(unsigned short));
	os.write(reinterpret_cast<char*>(&m_weight), sizeof(float));
	os.write(reinterpret_cast<char*>(&m_bloodType), sizeof(BloodType));

	WriteWstring(m_reference, os);

	//write doctor
	os.write(reinterpret_cast<char*>(&m_doctor), sizeof(unsigned long));

}

void Patient::Read(std::istream& is) {

	Person::Read(is);
	is.read(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_height), sizeof(unsigned short));
	is.read(reinterpret_cast<char*>(&m_weight), sizeof(float));
	is.read(reinterpret_cast<char*>(&m_bloodType), sizeof(BloodType));

	m_reference = ReadWstring(is);

	//read doctor
	is.read(reinterpret_cast<char*>(&m_doctor), sizeof(unsigned long));

}

bool operator==(const Patient& one, const Patient& two) {

	return one.GetName(Names::LASTNAME_FIRST) == two.GetName(Names::LASTNAME_FIRST);

}

bool operator!=(const Patient& one, const Patient& two) {

	return !(one == two);

}

bool operator<(const Patient& one, const Patient& two) {

	return  one.GetName(Names::LASTNAME_FIRST) < two.GetName(Names::LASTNAME_FIRST);

}

bool operator<=(const Patient& one, const Patient& two) {

	return !(one > two);

}

bool operator>(const Patient& one, const Patient& two) {

	return  one.GetName(Names::LASTNAME_FIRST) > two.GetName(Names::LASTNAME_FIRST);

}

bool operator>=(const Patient& one, const Patient& two) {

	return !(one < two);

}