#include "PhoneNumber.h"

PhoneNumber::PhoneNumber() {

	m_phoneNumber = L"0000000000";

}

PhoneNumber::PhoneNumber(const std::wstring& phoneNumber) {

	SetPhoneNumber(phoneNumber);

}

PhoneNumber::PhoneNumber(const wchar_t* phoneNumber) {

	SetPhoneNumber(phoneNumber);

}

void PhoneNumber::SetPhoneNumber(const std::wstring& phoneNumber) {

	if (phoneNumber.length() == 10) {
		bool areDigits = true;
		for (unsigned int i = 0; i < phoneNumber.length(); i++) {
			if (!iswdigit(phoneNumber[i])) {
				areDigits = false;
				break;
			}
		}
		
		if (areDigits) {
			m_phoneNumber = phoneNumber;
		}
	}

}

void PhoneNumber::SetPhoneNumber(const wchar_t* phoneNumber) {

	std::wstring temp(phoneNumber);

	SetPhoneNumber(temp);

}

std::wstring PhoneNumber::PhoneNumberString(bool formatting) const {

	std::wstring phoneNumberString;

	if (formatting) {
		//use formatting
		phoneNumberString =
			m_phoneNumber.substr(0, 2) + L"-" +			//##-
			m_phoneNumber.substr(2, 4) + L"-" +			//####-
			m_phoneNumber.substr(5, 4);					//####
	}
	else {
		phoneNumberString = m_phoneNumber;
	}

	return phoneNumberString;

}

void PhoneNumber::Write(std::ostream& os) {

	WriteWstring(m_phoneNumber, os);

}

void PhoneNumber::Read(std::istream& is) {

	m_phoneNumber = ReadWstring(is);

}