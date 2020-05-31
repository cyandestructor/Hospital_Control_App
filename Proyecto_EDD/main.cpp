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

	switch (msg) {

	case WM_INITDIALOG:
		hInstance = (HINSTANCE)lParam;
		
		LoadFiles(g_appList, g_patientList, g_speList, g_medOffList, g_doctorBST);

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
		SaveFiles(g_appList, g_patientList, g_speList, g_medOffList, g_doctorBST);
		PostQuitMessage(0);
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
				ReserveApp(app, g_appList, g_medOffList);
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
			break;
		case IDC_QA_MARKAPP_CMD:
			break;
		case IDC_QA_CANCELAPP_CMD:
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
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RP_ERROR_LOG);
			}
			break;
		case IDC_RP_CLEARREG_CMD:
			ClearPatientRegister(hWnd);
			break;
		case IDC_RP_LOOKPATIENT_CMD:
			break;
		case IDC_RP_DELETEREG_CMD:
			break;
		case IDC_RP_GENREPORT_CMD:
			SavePatientReport(hWnd, g_patientList);
			break;

		}

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
				g_doctorBST.Insert(doctor);				//Insert in the doctor tree
				ClearDoctorRegister(hWnd);				//Clear controls
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
			break;
		case IDC_RM_DELETEREG_CMD:
			break;
		case IDC_RM_GENREPORT_CMD:
			SaveDoctorReport(hWnd, g_doctorBST);
			break;
		}

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
		InitRegSpeControls(hWnd);
		break;
	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDC_RS_REGSPE_CMD:
			validationError = ValidateSpeRegister(hWnd);
			if (validationError.errorCode == ErrorCode::EC_NO_ERROR) {
				GetSpeRegisterInfo(hWnd, speciality);	//Get Info
				g_speList.Push(speciality);
				ClearSpeRegister(hWnd);
			}
			else {
				InterpretValidationError(validationError, true, hWnd, IDC_RS_ERROR_LOG);
			}
			break;
		case IDC_RS_CLEARREG_CMD:
			ClearSpeRegister(hWnd);
			break;
		case IDC_RS_LOOKSPE_CMD:
			break;
		case IDC_RS_DELETESPE_CMD:
			break;
		case IDC_RS_LOOKDRLIST_CMD:
			break;
		case IDC_RS_LOOKDRINFO_CMD:
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


	
}

void IWGRegApp(HWND hWnd) {

	g_patientList.ForEach([&](Patient& pat) {
		std::wstring aux = std::to_wstring(pat.Key()) + L" "
			+ pat.GetName(Names::FIRST_NAME) + L" "
			+ pat.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, IDC_RA_SELPATIENT_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

	g_speList.ForEach([&](Speciality& spe) {
		std::wstring aux = std::to_wstring(spe.Key()) + L" " + spe.Name();
		SendDlgItemMessageW(hWnd, IDC_RA_SELSPE_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

	g_medOffList.ForEach([&](MedOffice& medoff) {
		std::wstring aux = std::to_wstring(medoff.Number()) + L" Consultorio";
		SendDlgItemMessageW(hWnd, IDC_RA_SELMO_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

	g_doctorBST.ExecutePostorder([&](Doctor& doc) {
		std::wstring aux = doc.ProfessionalID() + L" " 
			+ doc.GetName(Names::FIRST_NAME) + L" " 
			+ doc.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

}

void IWGRegPatient(HWND hWnd) {

	g_doctorBST.ExecutePostorder([&](Doctor& doc) {
		std::wstring aux = doc.ProfessionalID() + L" "
			+ doc.GetName(Names::FIRST_NAME) + L" "
			+ doc.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, IDC_RP_FIRSTDR_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

}

void IWGRegDoctor(HWND hWnd) {

	
}

void IWGRegSpe(HWND hWnd) {



}

#pragma endregion Init_With_Globals