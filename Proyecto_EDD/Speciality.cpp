#include "Speciality.h"

void Speciality::SetName(const std::wstring& name) {

	m_name = name;

}

void Speciality::SetName(const wchar_t* name) {

	m_name = name;

}

const std::wstring& Speciality::Name() const {

	return m_name;

}

void Speciality::SetDescription(const std::wstring& description) {

	m_description = description;

}

void Speciality::SetDescription(const wchar_t* description) {

	m_description = description;

}

const std::wstring& Speciality::Description() const {

	return m_description;

}

unsigned int Speciality::Key() const {

	if (m_key != 0)
		return m_key;
	else
		return GenKey();

}

unsigned int Speciality::GenKey() const {

	unsigned int a = m_name.length(), b = m_description.length(), c = 0;
	
	if (a > 0 && b > 0) {
		c = (unsigned int)m_name[0];
		c += (unsigned int)m_description[0];
	}

	m_key = a + b + c;
	return m_key;

}

void Speciality::Write(std::ostream& os) {

	os.write(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	WriteWstring(m_name, os);
	WriteWstring(m_description, os);

}

void Speciality::Read(std::istream& is) {

	is.read(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	m_name = ReadWstring(is);
	m_description = ReadWstring(is);

}