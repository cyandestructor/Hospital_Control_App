#ifndef _PERSON_CLASS_
#define _PERSON_CLASS_

#include "Date.h"
#include "PhoneNumber.h"

enum class Names {

	FIRST_NAME, SECOND_NAME, FIRST_LASTNAME, SECOND_LASTNAME, FULL_NAME, LASTNAME_FIRST

};

enum class Gender {

	MALE, FEMELE, OTHER

};

class Person {

public:
	Person();
	void SetName(const std::wstring& name, 
		const std::wstring& secondName, 
		const std::wstring& firstLastname, 
		const std::wstring& secondLastname);
	void SetName(const wchar_t* name,
		const wchar_t* secondName,
		const wchar_t* firstLastname,
		const wchar_t* secondLastname);
	std::wstring GetName(const Names name) const;
	void SetBirthdate(const Date& date);
	const Date& Birthdate() const;
	unsigned short Age() const;
	void SetGender(Gender gender);
	Gender GetGender() const;
	std::wstring GetGenderString() const;
	PhoneNumber& GetPhoneNumber();
	void Write(std::ostream& os);
	void Read(std::istream& is);

private:
	std::wstring m_firstName;
	std::wstring m_secondName;
	std::wstring m_firstLastname;
	std::wstring m_secondLastname;

	Date m_birthdate;
	Gender m_gender;
	PhoneNumber m_phoneNumber;

};

#endif
