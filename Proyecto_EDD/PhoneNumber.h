#ifndef _PHONE_NUMBER_CLASS
#define _PHONE_NUMBER_CLASS

#include "HasString.h"

class PhoneNumber {

public:
	PhoneNumber();
	PhoneNumber(const std::wstring& phoneNumber);
	PhoneNumber(const wchar_t* phoneNumber);
	void SetPhoneNumber(const std::wstring& phoneNumber);
	void SetPhoneNumber(const wchar_t* phoneNumber);
	std::wstring PhoneNumberString(bool formatting) const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

private:
	std::wstring m_phoneNumber;

};

#endif
