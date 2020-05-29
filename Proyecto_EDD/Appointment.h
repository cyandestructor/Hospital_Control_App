#ifndef _APPOINTMENT_CLASS_
#define _APPOINTMENT_CLASS_

#include "Patient.h"
#include "List.h"
#include "BinarySearchTree.h"
#include "Doctor.h"
#include "DateTime.h"
#include "PhoneNumber.h"
#include "MedOffice.h"

class Appointment {

public:
	Appointment();
	void SetPatient(unsigned int key);
	const Patient& GetPatientInfo(List<Patient>& patientList) const;
	void SetDoctor(unsigned long id);
	unsigned long GetDoctorKey() const;
	const Doctor& GetDoctorInfo(BinarySearchTree<Doctor>& drBST) const;
	PhoneNumber& GetPhoneNumber();
	void SetAppointmentDateTime(const DateTime& date);
	const DateTime& GetDateTime() const;
	void SetMedOffice(unsigned int key);
	const MedOffice& GetMedOffice(List<MedOffice>& medOffList) const;
	unsigned int GetMedOfficeKey() const { return m_medOfficeKey; }
	unsigned int GetSpeciality(BinarySearchTree<Doctor>& drBST) const;
	void SetDescription(std::wstring& description);
	const std::wstring& GetDescription() const;
	void SetDouble(bool isDouble) { m_isDouble = isDouble; }
	bool IsDouble()const { return m_isDouble; }
	unsigned int Key() const;
	void Write(std::ostream& os);
	void Read(std::istream& is);

private:
	mutable unsigned int m_key;
	mutable unsigned int m_speKey;
	unsigned int m_patientKey;
	unsigned int m_medOfficeKey;
	unsigned long m_doctorID;
	bool m_isDouble;
	DateTime m_dateTime;
	PhoneNumber m_phoneNumber;
	std::wstring m_description;

	unsigned int GenKey() const;

};

#endif