#include "Appointment.h"

Appointment::Appointment() {

	m_doctorID = m_key = m_medOfficeKey = m_patientKey = 0;

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

const MedOffice& Appointment::GetMedOffice(List<MedOffice>& medOffList) const {

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

	m_key = a + b;
	return m_key;
}

void Appointment::Write(std::ostream& os) {

	os.write(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_patientKey), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_medOfficeKey), sizeof(unsigned int));
	os.write(reinterpret_cast<char*>(&m_doctorID), sizeof(unsigned long));
	os.write(reinterpret_cast<char*>(&m_dateTime), sizeof(DateTime));
	m_phoneNumber.Write(os);

	WriteWstring(m_description, os);

}

void Appointment::Read(std::istream& is) {

	is.read(reinterpret_cast<char*>(&m_key), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_patientKey), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_medOfficeKey), sizeof(unsigned int));
	is.read(reinterpret_cast<char*>(&m_doctorID), sizeof(unsigned long));
	is.read(reinterpret_cast<char*>(&m_dateTime), sizeof(DateTime));
	m_phoneNumber.Read(is);

	m_description = ReadWstring(is);

}