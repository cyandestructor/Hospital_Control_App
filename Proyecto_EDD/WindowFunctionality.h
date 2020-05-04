#ifndef _WINDOW_FUNC_
#define _WINDOW_FUNC_

#include <Windows.h>
#include "Appointment.h"

#define MAIN_FOLDER L"\\SistemaClínica"
#define DATA_FOLDER L"\\Data"
#define ID_FLOAT_EDIT_CONTROL	66000
#define ID_ALPHA_EDIT_CONTROL	66001
#define WM_ADD_APP				(WM_USER + 1)

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

//Register Appointment Window

void InitRegAppControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetAppRegisterInfo(HWND hWnd, Appointment& app);
ValidationError ValidateAppRegister(HWND hWnd);
void ClearAppRegister(HWND hWnd);
void SelectPatient(HWND hWnd, int comboBoxID, List<Patient>& patList);
void SelectDoctor(HWND hWnd, int comboBoxID, BinarySearchTree<Doctor>& drList);
void FilterDoctorsWithSpeciality(HWND hWnd, BinarySearchTree<Doctor>& drBST);

//Register Doctor Window

void InitRegMedControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetDoctorRegisterInfo(HWND hWnd, Doctor& doctor);
ValidationError ValidateDoctorRegister(HWND hWnd);
void ClearDoctorRegister(HWND hWnd);
void LoadDoctorImg(HWND hWnd, int width, int height);
bool GetImageFilename(HWND parent, std::wstring& buffer);
void SaveDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST);
void GenDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST, const std::wstring& filePath);

//Register Patient Window

void InitRegPatientControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetPatientRegisterInfo(HWND hWnd, Patient& patient);
ValidationError ValidatePatientRegister(HWND hWnd);
void ClearPatientRegister(HWND hWnd);
void SavePatientReport(HWND hWnd, List<Patient>& patList);
void GenPatientReport(HWND hWnd, List<Patient>& patList, const std::wstring& filepath);

//Register Speciality Window

void InitRegSpeControls(HWND hWnd, void(*initWithGlobals)(HWND) = 0);
void GetSpeRegisterInfo(HWND hWnd, Speciality& speciality);
ValidationError ValidateSpeRegister(HWND hWnd);
void ClearSpeRegister(HWND hWnd);

//File Management

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
unsigned int GetKeyFromCB(HWND hWnd, int cbID = 0);
unsigned long GetIDFromCB(HWND hWnd, int cbID = 0);
std::wstring DocumentsDirectory();

//Control Subclasses

LRESULT CALLBACK AlphaOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData);
LRESULT CALLBACK FloatOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData);

#endif