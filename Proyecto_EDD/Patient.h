#ifndef _PATIENT_CLASS_
#define _PATIENT_CLASS_

#include "Person.h"
#include "Doctor.h"

enum class BloodType {
	undef, posA, negA, posB, negB, posO, negO, posAB, negAB
};

class Patient : public Person{

public:
	Patient() : m_bloodType(BloodType::undef), m_height(0), m_weight(0), m_key(0), m_doctor(0) {}
	void SetHeight(unsigned short height);
	unsigned short Height() const;
	void SetWeigth(float weight);
	float Weight() const;
	void SetBloodType(BloodType bloodType);
	BloodType GetBloodType() const;
	std::wstring GetBloodTypeString() const;
	void SetReference(const std::wstring& reference);
	void SetReference(const wchar_t* reference);
	const std::wstring& Reference() const;
	void SetDoctor(unsigned long id);
	unsigned long GetDoctor() const;
	unsigned int Key() const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

	friend bool operator==(const Patient& one, const Patient& two);
	friend bool operator!=(const Patient& one, const Patient& two);
	friend bool operator<(const Patient& one, const Patient& two);
	friend bool operator<=(const Patient& one, const Patient& two);
	friend bool operator>(const Patient& one, const Patient& two);
	friend bool operator>=(const Patient& one, const Patient& two);

private:
	mutable unsigned int m_key;
	unsigned short m_height;
	float m_weight;
	BloodType m_bloodType;
	std::wstring m_reference;
	unsigned long m_doctor;

	unsigned int GenKey() const;

};

#endif