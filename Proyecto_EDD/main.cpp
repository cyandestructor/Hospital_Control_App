#include <Windows.h>
#include <gdiplus.h>
#include <CommCtrl.h>
#include "resource.h"
#include "WindowFunctionality.h"

#define TIMER_CLOCK 300

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

List<Appointment> g_appList;
List<Patient> g_patientList;
List<MedOffice> g_medOffList;
List<Speciality> g_speList;

BinarySearchTree<Doctor> g_doctorBST;

BOOL CALLBACK MainWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK QueryAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegPatientWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegMedWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegSpeWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ViewAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ViewPatientWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ViewMedWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ViewSpeWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void IWGQueryApp(HWND hWnd);
void IWGRegApp(HWND hWnd);
void IWGRegPatient(HWND hWnd);
void IWGRegDoctor(HWND hWnd);
void IWGRegSpe(HWND hWnd);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE prevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow) {

	MSG msg;
	HWND hDlg;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiPlusToken;

	//Initialize GDI+
	GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, NULL);

	//Main Window/Dialog
	hDlg = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_MAIN), HWND_DESKTOP,
		(DLGPROC)MainWinProc, (LPARAM)hInstance);

	ShowWindow(hDlg, SW_SHOWDEFAULT);

	//Message Loop
	while (GetMessageW(&msg, NULL, NULL, NULL) == TRUE) {

		if (!IsDialogMessageW(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

	}

	//Shutdown GDI+
	GdiplusShutdown(gdiPlusToken);
	return 0;

}

#pragma region Window_Procedures

BOOL CALLBACK MainWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static HWND subwindows[5];
	static HWND visible;
	static HINSTANCE hInstance;
	/*
	subwindows[0] : Registro de citas
	subwindows[1] : Consulta de citas
	subwindows[2] :	Registro de pacientes
	subwindows[3] : Registro de médicos
	subwindows[4] :	Registro de especialidad
	*/
		//MedOffice mo1, mo2;

	switch (msg) {

	case WM_INITDIALOG:
		hInstance = (HINSTANCE)lParam;
		CreateAppDirectory();
		LoadFiles(g_appList, g_patientList, g_speList, g_medOffList, g_doctorBST);

		/*mo1.SetNumber(1);
		mo2.SetNumber(2);
		g_medOffList.Push(mo2);
		g_medOffList.Push(mo1);*/

		//Registrar citas
		subwindows[0] = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_REG_APP),
			hWnd, (DLGPROC)RegAppWinProc, NULL);
		ShowWindow(subwindows[0], SW_SHOW);		//Shown...
		visible = subwindows[0];				//.. by default
		//Consultar citas
		subwindows[1] = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_QUERY_APP),
			hWnd, (DLGPROC)QueryAppWinProc, NULL);
		//Registrar pacientes
		subwindows[2] = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_REG_PATIENT),
			hWnd, (DLGPROC)RegPatientWinProc, NULL);
		//Registrar médicos
		subwindows[3] = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_REG_MED),
			hWnd, (DLGPROC)RegMedWinProc, NULL);
		//Registrar especialidad
		subwindows[4] = CreateDialogParamW(hInstance, MAKEINTRESOURCEW(IDD_REG_SPE),
			hWnd, (DLGPROC)RegSpeWinProc, NULL);
		//Initiate clock
		SetTimer(hWnd, TIMER_CLOCK, 1000, (TIMERPROC)NULL);
		//LOGIN
		DialogBoxParamW(hInstance, MAKEINTRESOURCEW(IDD_LOGIN), hWnd, LoginWinProc, (LPARAM)hWnd);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case CMD_REG_APP:
			SwitchVisible(visible, subwindows[0]);
			break;
		case CMD_QUERY_APP:
			SwitchVisible(visible, subwindows[1]);
			break;
		case CMD_REG_PATIENT:
			SwitchVisible(visible, subwindows[2]);
			break;
		case CMD_REG_MED:
			SwitchVisible(visible, subwindows[3]);
			break;
		case CMD_REG_SPE:
			SwitchVisible(visible, subwindows[4]);
			break;
		case CMD_EXIT:
			SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			break;
		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		if (MessageBoxW(hWnd, L"¿Está seguro de que quiere salir?", L"Salir", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK) {
			SaveFiles(g_appList, g_patientList, g_speList, g_medOffList, g_doctorBST);
			PostQuitMessage(0);
		}
		break;
	case WM_TIMER:
		Clock(hWnd);
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK RegAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Appointment app;
	ValidationError validationError;

	switch (msg) {

	case WM_INITDIALOG:
		InitRegAppControls(hWnd, IWGRegApp);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_RA_RESERVEAPP_CMD:
			validationError = ValidateAppRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetAppRegisterInfo(hWnd, app);	//Get info from controls
				if (ReserveApp(app, g_appList, g_medOffList))
					ClearAppRegister(hWnd);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RA_ERROR_LOG);
			}
			break;
		case IDC_RA_CLEARAPP_CMD:
			ClearAppRegister(hWnd);
			break;
		}

		switch (HIWORD(wParam)) {
		case CBN_SELCHANGE:
			switch (LOWORD(wParam)) {

			case IDC_RA_SELPATIENT_COMBO:
				SelectPatient(hWnd, IDC_RA_SELPATIENT_COMBO, g_patientList);
				break;
			case IDC_RA_SELSPE_COMBO:
				FilterDoctorsWithSpeciality(hWnd, g_doctorBST);
				break;
			case IDC_RA_SELDR_COMBO:
				SelectDoctor(hWnd, IDC_RA_SELDR_COMBO, g_doctorBST);
				break;

			}
			break;
		}

		break;
	case WM_SHOWWINDOW:
		if (wParam)
			IWGRegApp(hWnd);	//Update the Window
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK QueryAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static List<Appointment> queryBuffer;	//Buffer for main query
	List<Appointment> auxBuffer;			//Buffer for secondary query
	int queryType = -1;

	switch (msg) {

	case WM_INITDIALOG:
		InitQueryAppControls(hWnd);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_QA_SEARCHDR_CMD:
			QASearchDoctor(hWnd, g_doctorBST);
			break;
		case IDC_QA_SEARCHAPP_CMD:

			queryType = (int)SendDlgItemMessageW(hWnd, IDC_QA_ATYPE_COMBO, CB_GETCURSEL, NULL, NULL);
			switch (queryType) {
			case 0:
				QueryCode(hWnd, g_appList, queryBuffer);
				SaveQueryFile(queryBuffer, g_patientList, g_doctorBST, L"CodeQuery.txt");
				break;
			case 1:
				QueryWeek(hWnd, g_appList, queryBuffer);
				SaveQueryFile(queryBuffer, g_patientList, g_doctorBST, L"WeekQuery.txt");
				break;
			case 2:
				QueryDoctorMonth(hWnd, g_appList, queryBuffer);
				SaveQueryFile(queryBuffer, g_patientList, g_doctorBST, L"DoctorQuery.txt");
				break;
			case 3:
				QuerySpeciality(hWnd, g_appList, g_doctorBST, queryBuffer);
				SaveQueryFile(queryBuffer, g_patientList, g_doctorBST, L"SpeQuery.txt");
				break;
			default:
				queryBuffer.Clear();
			}
			ShowQuery(hWnd, queryBuffer);
			EnableWindow(GetDlgItem(hWnd, IDC_QA_MO_COMBO), TRUE);	//Enable the Medical office query
			break;
		case IDC_QA_CLEARAPP_CMD:
			ClearQuery(hWnd, queryBuffer);
			EnableWindow(GetDlgItem(hWnd, IDC_QA_MO_COMBO), FALSE);	//Disable the Medical office query
			break;
		case IDC_QA_LOOKAPP_CMD:
			ViewSelectedAppointment(hWnd, g_appList, (DLGPROC)ViewAppWinProc);
			break;
		case IDC_QA_MARKAPP_CMD:
			MarkApp(hWnd, g_appList, g_medOffList);
			break;
		case IDC_QA_CANCELAPP_CMD:
			CancelApp(hWnd, g_appList, g_medOffList);
			break;
		
		}

		switch (HIWORD(wParam)) {
		case CBN_SELCHANGE:

			switch (LOWORD(wParam)) {
			case IDC_QA_ATYPE_COMBO:
				SelectQueryType(hWnd, LOWORD(wParam));
				break;
			case IDC_QA_MO_COMBO:
				QueryMedOffice(hWnd, queryBuffer, auxBuffer);
				ShowQuery(hWnd, auxBuffer);
				break;
			}

			break;
		}

		break;
	case WM_SHOWWINDOW:
		if (wParam)
			IWGQueryApp(hWnd);	//UPDATE THE WINDOW
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK RegPatientWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Patient patient;
	ValidationError validationError;

	switch (msg) {

	case WM_INITDIALOG:
		InitRegPatientControls(hWnd, IWGRegPatient);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_RP_REGPATIENT_CMD:
			validationError = ValidatePatientRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetPatientRegisterInfo(hWnd, patient);
				g_patientList.Push(patient);
				ClearPatientRegister(hWnd);
				UpdatePatientList(hWnd, g_patientList, IDC_PATIENT_LIST);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RP_ERROR_LOG);
			}
			break;
		case IDC_RP_CLEARREG_CMD:
			ClearPatientRegister(hWnd);
			break;
		case IDC_RP_LOOKPATIENT_CMD:
			ViewSelectedPatient(hWnd, g_patientList, (DLGPROC)ViewPatientWinProc);
			UpdatePatientList(hWnd, g_patientList, IDC_PATIENT_LIST);
			break;
		case IDC_RP_DELETEREG_CMD:
			DeleteSelectedPatient(hWnd, g_patientList, g_appList);
			UpdatePatientList(hWnd, g_patientList, IDC_PATIENT_LIST);
			break;
		case IDC_RP_GENREPORT_CMD:
			SavePatientReport(hWnd, g_patientList);
			break;

		}

		break;
	case WM_SHOWWINDOW:
		if (wParam)
			IWGRegPatient(hWnd);	//UPDATE THE WINDOW
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK RegMedWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	ValidationError validationError;
	Doctor doctor;
	
	switch (msg) {

	case WM_INITDIALOG:
		InitRegMedControls(hWnd, IWGRegDoctor);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_RM_REGMED_CMD:
			validationError = ValidateDoctorRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetDoctorRegisterInfo(hWnd, doctor);	//Get Info from controls
				if (ValidateDoctorPerMO(hWnd, g_doctorBST, doctor)) {
					g_doctorBST.Insert(doctor);				//Insert in the doctor tree
					ClearDoctorRegister(hWnd);				//Clear controls
					UpdateDoctorList(hWnd, g_doctorBST, IDC_DR_LIST);
				}
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RM_ERROR_LOG);
			}
			break;
		case IDC_RM_CLEARREG_CMD:
			ClearDoctorRegister(hWnd);
			break;
		case IDC_RM_LOADIMG_CMD:
			LoadDoctorImg(hWnd, 150, 150);
			break;
		case IDC_RM_LOOKDR_CMD:
			ViewSelectedDoctor(hWnd, g_doctorBST, (DLGPROC)ViewMedWinProc);
			UpdateDoctorList(hWnd, g_doctorBST, IDC_DR_LIST);
			break;
		case IDC_RM_DELETEREG_CMD:
			DeleteSelectedDoctor(hWnd, g_doctorBST, g_appList);
			UpdateDoctorList(hWnd, g_doctorBST, IDC_DR_LIST);
			break;
		case IDC_RM_GENREPORT_CMD:
			SaveDoctorReport(hWnd, g_doctorBST);
			break;
		}

		break;
	case WM_SHOWWINDOW:
		if (wParam)
			IWGRegDoctor(hWnd);	//UPDATE THE WINDOW
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK RegSpeWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Speciality speciality;
	ValidationError validationError;

	switch (msg) {

	case WM_INITDIALOG:
		InitRegSpeControls(hWnd, IWGRegSpe);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_RS_REGSPE_CMD:
			validationError = ValidateSpeRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetSpeRegisterInfo(hWnd, speciality);	//Get Info
				g_speList.Push(speciality);
				ClearSpeRegister(hWnd);
				UpdateSpecialityList(hWnd, g_speList, IDC_SPE_LIST);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RS_ERROR_LOG);
			}
			break;
		case IDC_RS_CLEARREG_CMD:
			ClearSpeRegister(hWnd);
			break;
		case IDC_RS_LOOKSPE_CMD:
			ViewSelectedSpeciality(hWnd, g_speList, (DLGPROC)ViewSpeWinProc);
			UpdateSpecialityList(hWnd, g_speList, IDC_SPE_LIST);
			break;
		case IDC_RS_DELETESPE_CMD:
			DeleteSelectedSpeciality(hWnd, g_speList, g_doctorBST);
			UpdateSpecialityList(hWnd, g_speList, IDC_SPE_LIST);
			break;
		case IDC_RS_LOOKDRLIST_CMD:
			ShowSpecialityDoctors(hWnd, g_doctorBST);
			break;
		case IDC_RS_LOOKDRINFO_CMD:
			ViewSelectedDoctor(hWnd, g_doctorBST, (DLGPROC)ViewMedWinProc);
			SendDlgItemMessageW(hWnd, IDC_DR_LIST, LB_RESETCONTENT, NULL, NULL);
			break;

		}

		break;
	case WM_SHOWWINDOW:
		if (wParam)
			IWGRegSpe(hWnd);	//UPDATE THE WINDOW
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK ViewAppWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	//ValidationError validationError;
	static Appointment* appInfo = nullptr;
	Doctor appDr;
	Patient appPatient;
	Speciality appSpe;

	switch (msg) {

	case WM_INITDIALOG:
		appInfo = (Appointment*)lParam;	//Get the info passed through the parameter
		appDr = appInfo->GetDoctorInfo(g_doctorBST);
		appPatient = appInfo->GetPatientInfo(g_patientList);
		//TODO GET SPE INFO
		ShowAppointmentInfo(hWnd, *appInfo, appPatient, appDr, appSpe);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_RETURN_CMD:
			SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			break;

		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		appInfo = nullptr;
		EndDialog(hWnd, 1);
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK ViewPatientWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	ValidationError validationError;
	static Patient* patientInfo = nullptr;

	switch (msg) {

	case WM_INITDIALOG:
		patientInfo = (Patient*)lParam;	//Get the info passed through the parameter
		InitRegPatientControls(hWnd);
		ShowPatientInfo(hWnd, *patientInfo);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		
		case IDC_EDIT_PATIENT_CMD:
			EnablePatientEdit(hWnd, true);
			break;
		case IDC_SAVE_EDIT_PAT_CMD:
			//EDITION PROCESS
			validationError = ValidatePatientRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				EditPatient(hWnd, *patientInfo);
				SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RP_ERROR_LOG);
			}
			break;
		case IDC_CANCEL_EDIT_PAT_CMD:
			EnablePatientEdit(hWnd, false);
			ShowPatientInfo(hWnd, *patientInfo);
			break;
		case IDC_RETURN_CMD:
			SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			break;
		
		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		patientInfo = nullptr;
		EndDialog(hWnd, 1);
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK ViewMedWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	ValidationError validationError;
	static Doctor* drInfo = nullptr;

	switch (msg) {

	case WM_INITDIALOG:
		drInfo = (Doctor*)lParam;	//Get the info passed through the parameter
		InitRegMedControls(hWnd);
		ShowDoctorInfo(hWnd, *drInfo);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_EDIT_DR_CMD:
			EnableDoctorEdit(hWnd, true);
			break;
		case IDC_SAVE_EDIT_DR_CMD:
			validationError = ValidateDoctorEdition(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				if (ValidateDoctorPerMO(hWnd, g_doctorBST, *drInfo)) {
					EditDoctor(hWnd, *drInfo);
					SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
				}
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RM_ERROR_LOG);
			}
			break;
		case IDC_CANCEL_EDIT_DR_CMD:
			EnableDoctorEdit(hWnd, false);
			ShowDoctorInfo(hWnd, *drInfo);
			break;
		case IDC_VM_LOADIMG_CMD:
			LoadDoctorImg(hWnd, 150, 150);
			break;
		case IDC_RETURN_CMD:
			SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			break;

		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		drInfo = nullptr;	//Clear the pointer
		EndDialog(hWnd, 1);
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK ViewSpeWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	ValidationError validationError;
	static Speciality* speInfo = nullptr;

	switch (msg) {

	case WM_INITDIALOG:
		speInfo = (Speciality*)lParam;	//Get the info passed through the parameter
		InitRegSpeControls(hWnd);
		ShowSpecialityInfo(hWnd, *speInfo);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_EDIT_SPE_CMD:
			EnableSpecialityEdit(hWnd, true);
			break;
		case IDC_SAVE_EDIT_SPE_CMD:

			validationError = ValidateSpeRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetSpeRegisterInfo(hWnd, *speInfo);	//Get Info
				SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RS_ERROR_LOG);
			}

			break;
		case IDC_CANCEL_EDIT_SPE_CMD:
			EnableSpecialityEdit(hWnd, false);
			ShowSpecialityInfo(hWnd, *speInfo);
			break;
		case IDC_RETURN_CMD:
			SendMessageW(hWnd, WM_CLOSE, NULL, NULL);
			break;

		}

		break;
	case WM_CLOSE:
	case WM_DESTROY:
		speInfo = nullptr;
		EndDialog(hWnd, 1);
		break;
	default:
		return FALSE;

	}

	return TRUE;

}

BOOL CALLBACK LoginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	User user;
	static HWND parent = NULL;

	switch (msg) {
	case WM_INITDIALOG:
		parent = (HWND)lParam;
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_LOGIN_CMD:
			if (CheckUserCredentials(hWnd, user)) {
				SetDlgItemTextW(parent, IDC_MAIN_USERNAME, user.UserName().c_str());
				SetDlgItemTextW(parent, IDC_MAIN_USER_FULLNAME, user.Name().c_str());
				EndDialog(hWnd, 1);
			}
			break;
		case IDC_EXIT_CMD:
			PostQuitMessage(0);	//END APP
			break;
		}

		break;
	default:
		return FALSE;

	}

	return TRUE;


}

#pragma endregion

#pragma region Init_With_Globals

void IWGQueryApp(HWND hWnd) {

	UpdateSpecialityList(hWnd, g_speList, IDC_QA_SPE_COMBO, true);
	UpdateMedicalOfficeList(hWnd, g_medOffList, IDC_QA_MO_COMBO, true);

}

void IWGRegApp(HWND hWnd) {

	UpdatePatientList(hWnd, g_patientList, IDC_RA_SELPATIENT_COMBO, true);
	UpdateSpecialityList(hWnd, g_speList, IDC_RA_SELSPE_COMBO, true);
	UpdateMedicalOfficeList(hWnd, g_medOffList, IDC_RA_SELMO_COMBO, true);
	//UpdateDoctorList(hWnd, g_doctorBST, IDC_RA_SELDR_COMBO, true);
	SendDlgItemMessageW(hWnd, IDC_RA_SELSPE_COMBO, CB_INSERTSTRING, 0, (LPARAM)L"TODOS");

}

void IWGRegPatient(HWND hWnd) {

	UpdateDoctorList(hWnd, g_doctorBST, IDC_RP_FIRSTDR_COMBO, true);
	UpdatePatientList(hWnd, g_patientList, IDC_PATIENT_LIST);
}

void IWGRegDoctor(HWND hWnd) {

	UpdateSpecialityList(hWnd, g_speList, IDC_RM_SPE_COMBO, true);
	UpdateMedicalOfficeList(hWnd, g_medOffList, IDC_RM_MO_COMBO, true);
	UpdateDoctorList(hWnd, g_doctorBST, IDC_DR_LIST);

}

void IWGRegSpe(HWND hWnd) {

	UpdateSpecialityList(hWnd, g_speList, IDC_SPE_LIST);
	//UpdateDoctorList(hWnd, g_doctorBST, IDC_DR_LIST);

}

#pragma endregion Init_With_Globals