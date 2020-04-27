#ifndef _SPECIALITY_CLASS_
#define _SPECIALITY_CLASS_

#include "HasString.h"

class Speciality {

public:
	Speciality() :m_name(L""), m_description(L""), m_key(0) {}
	void SetName(const std::wstring& name);
	void SetName(const wchar_t* name);
	const std::wstring& Name() const;
	void SetDescription(const std::wstring& description);
	void SetDescription(const wchar_t* description);
	const std::wstring& Description() const;
	unsigned int Key() const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

private:
	mutable unsigned int m_key;
	std::wstring m_name;
	std::wstring m_description;

	unsigned int GenKey() const;

};

#endif