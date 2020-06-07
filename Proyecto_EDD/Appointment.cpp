#include "Appointment.h"

Appointment::Appointment() {

	m_doctorID = m_key = m_medOfficeKey = m_speKey = m_patientKey = 0;
	m_isDouble = m_isAttended = m_isCanceled = false;

}

void Appointment::SetPatient(unsigned int key) {

	m_patientKey = key;

}

const Patient& Appointment::GetPatientInfo(List<Patient>& patientList) const {

	return patientList.Search<unsigned int>(m_patientKey,
		[](const Patient& patient, const unsigned int& key)->bool {
			return patient.Key() == key;
		});

}

void Appointment::SetDoctor(unsigned long id) {

	m_doctorID = id;

}

unsigned long Appointment::GetDoctorKey() const {

	return m_doctorID;

}

const Doctor& Appointment::GetDoctorInfo(BinarySearchTree<Doctor>& drBST) const {

	static Doctor tmp(this->m_doctorID);

	drBST.BinarySearch(tmp, tmp);

	return tmp;

}

PhoneNumber& Appointment::GetPhoneNumber() {

	return m_phoneNumber;

}

void Appointment::SetAppointmentDateTime(const DateTime& date) {

	m_dateTime = date;

}

const DateTime& Appointment::GetDateTime() const {

	return m_dateTime;

}

void Appointment::SetMedOffice(unsigned int key) {

	m_medOfficeKey = key;

}

unsigned int Appointment::GetSpeciality(BinarySearchTree<Doctor>& drBST) const {

	if (m_speKey == 0) {
		m_speKey = this->GetDoctorInfo(drBST).GetSpeciality();
	}

	return m_speKey;
}

MedOffice& Appointment::GetMedOffice(List<MedOffice>& medOffList) {

	return medOffList.Search<unsigned int>(m_medOfficeKey,
		[](const MedOffice& medOff, const unsigned int& key)->bool {
			return medOff.Number() == key;
		});

}

void Appointment::SetDescription(std::wstring& description) {

	m_description = description;

}

const std::wstring& Appointment::GetDescription() const {

	return m_description;

}

void Appointment::Cancel(bool state) {

	if (state && m_isAttended)
		m_isAttended = false;

	m_isCanceled = state;

}

void Appointment::MarkAttended(bool state) {

	if (state && m_isCanceled)
		m_isCanceled = false;

	m_isAttended = state;

}

unsigned int Appointment::Key() const {

	//The key is generated only when the Key method is called (we avoid to generate a key for a temp object)

	if (m_key != 0)			//If the key has been generated
		return m_key;
	else
		return GenKey();

}

unsigned int Appointment::GenKey() const {

	//Get the firt three numbers of the phone number
	short a = (short)(_wtoi((m_phoneNumber.PhoneNumberString(false).substr(0, 3).c_str())));
	//Get the sum of the month, the day, the hour and the minute of the appointment date time
	short b = m_dateTime.Month() + m_dateTime.Day() + m_dateTime.Hour() + m_dateTime.Minute();

	m_key = a + b + m_patientKey;
	return m_key;
}

void Appointment::Write(std::ostream& os) {

	os.write(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_patientKey), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_medOfficeKey), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_doctorID), sizeof(unsigned long));
	os.write(reinterpret_cast<char*>(&m_dateTime), sizeof(DateTime));
	m_phoneNumber.Write(os);

	os.write(reinterpret_cast<char*>(&m_isDouble), sizeof(bool));
	os.write(reinterpret_cast<char*>(&m_isCanceled), sizeof(bool));
	os.write(reinterpret_cast<char*>(&m_isAttended), sizeof(bool));

	WriteWstring(m_description, os);

}

void Appointment::Read(std::istream& is) {

	is.read(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_patientKey), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_medOfficeKey), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_doctorID), sizeof(unsigned long));
	is.read(reinterpret_cast<char*>(&m_dateTime), sizeof(DateTime));
	m_phoneNumber.Read(is);

	is.read(reinterpret_cast<char*>(&m_isDouble), sizeof(bool));
	is.read(reinterpret_cast<char*>(&m_isCanceled), sizeof(bool));
	is.read(reinterpret_cast<char*>(&m_isAttended), sizeof(bool));

	m_description = ReadWstring(is);

}