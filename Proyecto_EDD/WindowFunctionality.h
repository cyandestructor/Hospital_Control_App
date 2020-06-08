#ifndef _WINDOW_FUNC_
#define _WINDOW_FUNC_

#include <Windows.h>
#include "Appointment.h"

#define MAIN_FOLDER L"\\SistemaClínica"
#define DATA_FOLDER L"\\Data"
#define IMG_FOLDER L"\\Img"
#define ID_FLOAT_EDIT_CONTROL	66000
#define ID_ALPHA_EDIT_CONTROL	66001

enum class ErrorCode {
	EC_EMPTY, EC_NO_SEL, EC_INVALID_SIZE, EC_INVALID_DATE, EC_INVALID_FORMAT, EC_NO_ERROR
};

struct ValidationError {
	ErrorCode errorCode;
	HWND errorSrc;
};

//Appointment Query Window

void InitQueryAppControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void SelectQueryType(HWND hWnd, int comboBoxID);
void QASearchDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST);
void MarkApp(HWND hWnd, List<Appointment>& appList, List<MedOffice>& moList);
void CancelApp(HWND hWnd, List<Appointment>& appList, List<MedOffice>& moList);
void QueryDoctorMonth(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer);
void QuerySpeciality(HWND hWnd, List<Appointment>& appList, BinarySearchTree<Doctor>& drBST, List<Appointment>& buffer);
void QueryWeek(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer);
void QueryCode(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer);
void QueryMedOffice(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer);
bool GetAppByDoctor(List<Appointment>& appList, unsigned long proID, List<Appointment>& buffer, bool chain);
bool GetAppBySpe(List<Appointment>& appList, unsigned int speKey,
	BinarySearchTree<Doctor>& drBST, List<Appointment>& buffer, bool chain);
bool GetAppByMonth(List<Appointment>& appList, unsigned short month, List<Appointment>& buffer, bool chain);
bool GetAppByMedicalOffice(List<Appointment>& appList, unsigned int moKey, List<Appointment>& buffer, bool chain);
bool GetAppByCode(List<Appointment>& appList, unsigned int appCode, List<Appointment>& buffer, bool chain);
bool GetAppByWeek(List<Appointment>& appList, const DateTime& dateTimeRef, List<Appointment>& buffer, bool chain);
void ShowQuery(HWND hWnd, List<Appointment>& appList);
void SaveQueryFile(List<Appointment>& appList, List<Patient>& patList, BinarySearchTree<Doctor>& drBST, const wchar_t* fileName);
void ClearQuery(HWND hWnd, List<Appointment>& qBuffer);
void ViewSelectedAppointment(HWND hWnd, List<Appointment>& appList, DLGPROC viewerProc);
TimePeriod GetWeekFromDate(const DateTime& dateTimeRef);

//Register Appointment Window

void InitRegAppControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetAppRegisterInfo(HWND hWnd, Appointment& app);
ValidationError ValidateAppRegister(HWND hWnd);
bool ValidDrSchAppTime(const Appointment& app, BinarySearchTree<Doctor>& drBST);
bool ReserveApp(Appointment& app, List<Appointment>& appList, List<MedOffice>& moList);
void ClearAppRegister(HWND hWnd);
void SelectPatient(HWND hWnd, int comboBoxID, List<Patient>& patList);
void SelectDoctor(HWND hWnd, int comboBoxID, BinarySearchTree<Doctor>& drList);
void FilterDoctorsWithSpeciality(HWND hWnd, BinarySearchTree<Doctor>& drBST);

//Register Doctor Window

void InitRegMedControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetDoctorRegisterInfo(HWND hWnd, Doctor& doctor);
ValidationError ValidateDoctorRegister(HWND hWnd);
bool ValidateDoctorPerMO(HWND hWnd, BinarySearchTree<Doctor>& drBST, Doctor& toValidate);
unsigned int AppointmentsPerDoctor(unsigned long drId, List<Appointment>& appList,
	std::vector<Appointment>* buffer = nullptr);
void ClearDoctorRegister(HWND hWnd);
void LoadDoctorImg(HWND hWnd, int width, int height);
bool GetImageFilename(HWND parent, std::wstring& buffer);
void ViewSelectedDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST, DLGPROC viewerProc);
void DeleteSelectedDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST, List<Appointment>& appList);
void SaveDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST);
void GenDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST, const std::wstring& filePath);

//Register Patient Window

void InitRegPatientControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetPatientRegisterInfo(HWND hWnd, Patient& patient);
ValidationError ValidatePatientRegister(HWND hWnd);
void ClearPatientRegister(HWND hWnd);
Patient& FindPatientWithKey(List<Patient>& patList, unsigned int key);
void ViewSelectedPatient(HWND hWnd, List<Patient>& patList, DLGPROC viewerProc);
void DeleteSelectedPatient(HWND hWnd, List<Patient>& patList, List<Appointment>& appList);
void SavePatientReport(HWND hWnd, List<Patient>& patList);
void GenPatientReport(HWND hWnd, List<Patient>& patList, const std::wstring& filepath);
unsigned int AppointmentsPerPatient(unsigned int patKey, List<Appointment>& appList, 
	std::vector<Appointment>* buffer = nullptr);

//Register Speciality Window

void InitRegSpeControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetSpeRegisterInfo(HWND hWnd, Speciality& speciality);
ValidationError ValidateSpeRegister(HWND hWnd);
void ClearSpeRegister(HWND hWnd);
void ViewSelectedSpeciality(HWND hWnd, List<Speciality>& speList, DLGPROC viewerProc);
void DeleteSelectedSpeciality(HWND hWnd, List<Speciality>& speList, BinarySearchTree<Doctor>& drBST);
unsigned int DoctorsPerSpeciality(unsigned int key, BinarySearchTree<Doctor>& drBST, std::vector<Doctor>* buffer = 0);
void ShowSpecialityDoctors(HWND hWnd, BinarySearchTree<Doctor>& drBST);

//View Appointment Window

void ShowAppointmentInfo(HWND hWnd, Appointment& app, Patient& patient, Doctor& doctor, Speciality& spe);

//View Patient Window

void ShowPatientInfo(HWND hWnd, Patient& pat);
void EditPatient(HWND hWnd, Patient& pat);
void EnablePatientEdit(HWND hWnd, bool enable);

//View Doctor Window

void ShowDoctorInfo(HWND hWnd, Doctor& dr);
void EditDoctor(HWND hWnd, Doctor& dr);
ValidationError ValidateDoctorEdition(HWND hWnd);
void EnableDoctorEdit(HWND hWnd, bool enable);

//View Speciality Window

void ShowSpecialityInfo(HWND hWnd, Speciality& spe);
void EnableSpecialityEdit(HWND hWnd, bool enable);

//File Management

void CreateAppDirectory();
void LoadFiles(List<Appointment>& appList,
	List<Patient>& patList,
	List<Speciality>& speList,
	List<MedOffice>& medOffList,
	BinarySearchTree<Doctor>& drBST);
void SaveFiles(List<Appointment>& appList,
	List<Patient>& patList,
	List<Speciality>& speList,
	List<MedOffice>& medOffList,
	BinarySearchTree<Doctor>& drBST);

//Miscelaneous

void SwitchVisible(HWND& currentVisible, HWND& toVisible);
void Clock(HWND hWnd);
int GetWindowTextWstring(HWND hWnd, std::wstring& buffer);
void InterpretValidationError(const ValidationError& toInterpret, bool setFocus, HWND hWnd = 0,
	int errorLogID = 0, std::wstring* buffer = 0);
void SetPictureControlImg(const wchar_t* filePath, int width, int height, HWND hWnd, int pictureControlId = 0);
unsigned int GetKeyFromWstring(const std::wstring& wstring);
int ComboBoxGetWstring(std::wstring& buffer, int index, HWND hWnd, int cbID = 0);
int ListBoxGetWstring(std::wstring& buffer, int index, HWND hWnd, int lbID = 0);
unsigned int GetKeyFromCB(HWND hWnd, int cbID = 0);
unsigned int GetKeyFromLB(HWND hWnd, int lbID = 0);
unsigned long GetIDFromCB(HWND hWnd, int cbID = 0);
unsigned long GetIDFromLB(HWND hWnd, int lbID = 0);
std::wstring DocumentsDirectory();
void UpdateDoctorList(HWND hWnd, BinarySearchTree<Doctor>& drBST, int controlID, bool cb = false);
void UpdatePatientList(HWND hWnd, List<Patient>& patList, int controlID, bool cb = false);
void UpdateSpecialityList(HWND hWnd, List<Speciality>& speList, int controlID, bool cb = false);
void UpdateAppointmentList(HWND hWnd, List<Appointment>& appList, int controlID, bool cb = false);
void UpdateMedicalOfficeList(HWND hWnd, List<MedOffice>& moList, int controlID, bool cb = false);

//Control Subclasses

LRESULT CALLBACK AlphaOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData);
LRESULT CALLBACK FloatOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData);

#endif