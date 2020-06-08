#include "Person.h"

Person::Person() {

	m_gender = Gender::OTHER;
	m_phoneNumber = L"0000000000";

}

void Person::SetName(const std::wstring& name,
	const std::wstring& secondName,
	const std::wstring& firstLastname,
	const std::wstring& secondLastname) {

	m_firstName = name;
	m_secondName = secondName;
	m_firstLastname = firstLastname;
	m_secondLastname = secondLastname;

}

void Person::SetName(const wchar_t* name,
	const wchar_t* secondName,
	const wchar_t* firstLastname,
	const wchar_t* secondLastname) {

	m_firstName = name;
	m_secondName = secondName;
	m_firstLastname = firstLastname;
	m_secondLastname = secondLastname;

}

std::wstring Person::GetName(const Names name) const {

	std::wstring returnedName;

	switch (name) {

	case Names::FIRST_NAME:
		returnedName = m_firstName;
		break;
	case Names::SECOND_NAME:
		returnedName = m_secondName;
		break;
	case Names::FIRST_LASTNAME:
		returnedName = m_firstLastname;
		break;
	case Names::SECOND_LASTNAME:
		returnedName = m_secondLastname;
		break;
	case Names::FULL_NAME:
		returnedName = m_firstName + L" " + m_secondName + L" " + m_firstLastname + L" " + m_secondLastname;
		break;
	case Names::LASTNAME_FIRST:
		returnedName = m_firstLastname + L" " + m_secondLastname + L" " + m_firstName + L" " + m_secondName;
		break;
	default:
		returnedName = L"UNDEFINED";

	}

	return returnedName;

}

void Person::SetBirthdate(const Date& date) {

	m_birthdate = date;

}

const Date& Person::Birthdate() const {

	return m_birthdate;

}

int Person::Age() const {

	Date today;	//Date initiallized with today's date
	int diffDays = today.DifferenceWith(this->m_birthdate);

	return (int)diffDays / 360;

}

void Person::SetGender(Gender gender) {

	m_gender = gender;

}

Gender Person::GetGender() const {

	return m_gender;

}

std::wstring Person::GetGenderString() const {

	std::wstring gender;

#ifdef _SET_LANGUAGE_SPANISH_
	if (m_gender == Gender::MALE) {
		gender = L"Masculino";
	}
	else if (m_gender == Gender::FEMELE) {
		gender = L"Femenino";
	}
	else if (m_gender == Gender::OTHER) {
		gender = L"Otro";
	}
#else
	if (m_gender == Gender::MALE) {
		gender = L"Male";
	}
	else if (m_gender == Gender::FEMELE) {
		gender = L"Female";
	}
	else if (m_gender == Gender::OTHER) {
		gender = L"Other";
	}
#endif

	return gender;

}

PhoneNumber& Person::GetPhoneNumber() {

	return m_phoneNumber;

}

void Person::Write(std::ostream& os) {

	WriteWstring(m_firstName, os);
	WriteWstring(m_secondName, os);
	WriteWstring(m_firstLastname, os);
	WriteWstring(m_secondLastname, os);

	os.write(reinterpret_cast<char*>(&m_birthdate), sizeof(Date));
	os.write(reinterpret_cast<char*>(&m_gender), sizeof(Gender));
	
	m_phoneNumber.Write(os);

}

void Person::Read(std::istream& is) {

	m_firstName = ReadWstring(is);
	m_secondName = ReadWstring(is);
	m_firstLastname = ReadWstring(is);
	m_secondLastname = ReadWstring(is);

	is.read(reinterpret_cast<char*>(&m_birthdate), sizeof(Date));
	is.read(reinterpret_cast<char*>(&m_gender), sizeof(Gender));

	m_phoneNumber.Read(is);

}