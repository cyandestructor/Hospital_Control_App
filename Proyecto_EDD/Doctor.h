#ifndef _DOCTOR_CLASS_
#define _DOCTOR_CLASS_

#include "Person.h"
#include "Schedule.h"
#include "Speciality.h"

class Doctor : public Person {

public:
	Doctor() :m_professionalID(0), m_speKey(0), m_medOffNum(0) {}
	Doctor(unsigned long proID) :m_professionalID(proID), m_speKey(0), m_medOffNum(0) {}
	void SetProfessionalID(unsigned long professionalID);
	std::wstring ProfessionalID();
	unsigned long ULProfessionalID();
	void SetImagePath(std::wstring imagePath);
	void SetImagePath(const wchar_t* imagePath);
	const std::wstring& GetImagePath() const;
	Schedule& GetSchedule();
	void SetSpeciality(unsigned int key);
	const unsigned int& GetSpeciality() const;
	void SetMedOfficeNumber(unsigned int num);
	unsigned int GetMedOfficeNum() const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

	friend bool operator==(const Doctor& one, const Doctor& two);
	friend bool operator!=(const Doctor& one, const Doctor& two);
	friend bool operator<(const Doctor& one, const Doctor& two);
	friend bool operator<=(const Doctor& one, const Doctor& two);
	friend bool operator>(const Doctor& one, const Doctor& two);
	friend bool operator>=(const Doctor& one, const Doctor& two);

private:
	unsigned long m_professionalID;
	unsigned int m_speKey;
	unsigned int m_medOffNum;
	std::wstring m_imagePath;
	Schedule m_schedule;

};

#endif
