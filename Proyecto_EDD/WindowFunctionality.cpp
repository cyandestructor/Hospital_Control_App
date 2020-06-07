#include "WindowFunctionality.h"
#include <wchar.h>
#include <CommCtrl.h>
#include <gdiplus.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include "resource.h"
#include "Heap.h"

#pragma comment (lib, "Comctl32.lib")

#pragma region Window_Functionality

#pragma region Miscelaneous

void SwitchVisible(HWND& currentVisible, HWND& toVisible) {

	if (IsWindowVisible(currentVisible) && !IsWindowVisible(toVisible)) {
		ShowWindow(currentVisible, SW_HIDE);
		ShowWindow(toVisible, SW_SHOW);
	}

	currentVisible = toVisible;

}

void Clock(HWND hWnd) {

	wchar_t date[12];
	wchar_t time[10];
	SYSTEMTIME now;
	memset(&now, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&now);
	wsprintf(date, L" %02d/%02d/%d", now.wDay, now.wMonth, now.wYear);
	wsprintf(time, L" %02d:%02d:%02d", now.wHour, now.wMinute, now.wSecond);

	SetWindowTextW(GetDlgItem(hWnd, IDC_MAIN_TODAY), date);
	SetWindowTextW(GetDlgItem(hWnd, IDC_MAIN_TIME), time);

}

int GetWindowTextWstring(HWND hWnd, std::wstring& buffer) {

	const unsigned int textSize = GetWindowTextLengthW(hWnd);
	if (textSize > 0) {
		buffer.resize(textSize + 1);
		GetWindowTextW(hWnd, &buffer[0], textSize + 1);
		buffer.resize(textSize);
	}
	else {
		buffer = L"";
	}

	return textSize;

}

void InterpretValidationError(const ValidationError& toInterpret, bool setFocus, HWND hWnd,
	int errorLogID, std::wstring* buffer) {

	std::wstring errDesc;

	switch (toInterpret.errorCode) {

	case ErrorCode::EC_EMPTY:
		errDesc = L"El campo de información está vacío";
		break;
	case ErrorCode::EC_INVALID_DATE:
		errDesc = L"La fecha indicada es inválida";
		break;
	case ErrorCode::EC_INVALID_FORMAT:
		errDesc = L"El formato de la información es incorrecta";
		break;
	case ErrorCode::EC_INVALID_SIZE:
		errDesc = L"El tamaño de la información es incorrecto";
		break;
	case ErrorCode::EC_NO_SEL:
		errDesc = L"No se ha seleccionado la información";
		break;
	case ErrorCode::EC_NO_ERROR:
		errDesc = L"Información incorrecta";
		break;

	}

	//Dialog with control ID specified
	if (hWnd != 0 && errorLogID != 0) {
		SetDlgItemTextW(hWnd, errorLogID, errDesc.c_str());
	}

	//Window only
	if (hWnd != 0 && errorLogID == 0) {
		SetWindowTextW(hWnd, errDesc.c_str());
	}

	//Buffer specified
	if (buffer != 0) {
		(*buffer) = errDesc;
	}

	if (setFocus) {
		SetFocus(toInterpret.errorSrc);
	}

}

void SetPictureControlImg(const wchar_t* filePath, int width, int height, HWND hWnd, int pictureControlId) {

	Gdiplus::Bitmap imgOriginal(filePath);
	Gdiplus::Bitmap imgResize(width, height);

	Gdiplus::Graphics gScale(&imgResize);
	gScale.DrawImage(&imgOriginal, 0, 0, width, height);

	HBITMAP hBitMap;
	imgResize.GetHBITMAP(NULL, &hBitMap);

	if (pictureControlId != 0)
		SendDlgItemMessageW(hWnd, pictureControlId, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitMap);
	else
		SendMessageW(hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitMap);


}

unsigned int GetKeyFromWstring(const std::wstring& wstring) {

	short spaceIndex = (short)wstring.find_first_of(L' ');
	unsigned int keyValue = (unsigned int)(_wtoi(wstring.substr(0, spaceIndex).c_str()));

	return keyValue;

}

int ComboBoxGetWstring(std::wstring& buffer, int index, HWND hWnd, int cbID) {

	int textL;

	if (cbID != 0) {
		textL = (int)SendDlgItemMessageW(hWnd, cbID, CB_GETLBTEXTLEN, index, NULL);
		buffer.resize(textL + 1);
		SendDlgItemMessageW(hWnd, cbID, CB_GETLBTEXT, index, (LPARAM)&buffer[0]);
	}
	else {
		textL = (int)SendMessageW(hWnd, CB_GETLBTEXTLEN, index, NULL);
		buffer.resize(textL + 1);
		SendMessageW(hWnd, CB_GETLBTEXT, index, (LPARAM)&buffer[0]);
	}

	buffer.resize(textL);
	return textL;

}

int ListBoxGetWstring(std::wstring& buffer, int index, HWND hWnd, int lbID) {

	HWND listBox = hWnd;
	int textL;

	if (lbID != 0)
		listBox = GetDlgItem(hWnd, lbID);

	textL = (int)SendMessageW(hWnd, LB_GETTEXTLEN, index, NULL);
	buffer.resize(textL + 1);
	SendMessageW(hWnd, LB_GETTEXT, index, (LPARAM)&buffer[0]);
	buffer.resize(textL);

	return textL;

}

unsigned int GetKeyFromCB(HWND hWnd, int cbID) {

	HWND comboBox = hWnd;
	unsigned int key = 0;

	if (cbID != 0)
		comboBox = GetDlgItem(hWnd, cbID);

	int sel = SendMessageW(comboBox, CB_GETCURSEL, NULL, NULL);

	if (sel != CB_ERR) {
		std::wstring aux;
		ComboBoxGetWstring(aux, sel, comboBox);
		key = GetKeyFromWstring(aux);
	}

	return key;

}

unsigned int GetKeyFromLB(HWND hWnd, int lbID) {

	HWND listBox = hWnd;
	unsigned int key = 0;

	if (lbID != 0)
		listBox = GetDlgItem(hWnd, lbID);

	int sel = SendMessageW(listBox, LB_GETCURSEL, NULL, NULL);

	if (sel != LB_ERR) {
		std::wstring aux;
		ListBoxGetWstring(aux, sel, listBox);
		key = GetKeyFromWstring(aux);
	}

	return key;

}

unsigned long GetIDFromCB(HWND hWnd, int cbID) {

	HWND comboBox = hWnd;
	std::wstring aux;

	if (cbID != 0)
		comboBox = GetDlgItem(hWnd, cbID);

	int sel = SendMessageW(comboBox, CB_GETCURSEL, NULL, NULL);

	if (sel != CB_ERR) {
		ComboBoxGetWstring(aux, sel, comboBox);
		int spaceIndex = aux.find_first_of(L' ');
		return std::stoul(aux.substr(0, spaceIndex));
	}

	return 0;

}

unsigned long GetIDFromLB(HWND hWnd, int lbID) {

	HWND listBox = hWnd;
	std::wstring aux;

	if (lbID != 0)
		listBox = GetDlgItem(hWnd, lbID);

	int sel = SendMessageW(listBox, LB_GETCURSEL, NULL, NULL);

	if (sel != LB_ERR) {
		ListBoxGetWstring(aux, sel, listBox);
		int spaceIndex = aux.find_first_of(L' ');
		return std::stoul(aux.substr(0, spaceIndex));
	}

	return 0;

}

std::wstring DocumentsDirectory() {

	wchar_t* buffer = nullptr;
	std::wstring temp;

	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &buffer) == S_OK)
		temp = buffer;

	CoTaskMemFree(buffer);
	return temp;

}

void UpdateDoctorList(HWND hWnd, BinarySearchTree<Doctor>& drBST, int controlID, bool cb) {
	
	int clear = LB_RESETCONTENT;
	int msg = LB_ADDSTRING;
	if (cb) {
		msg = CB_ADDSTRING;
		clear = CB_RESETCONTENT;
	}

	SendDlgItemMessageW(hWnd, controlID, clear, NULL, NULL);

	drBST.ExecutePostorder([&](Doctor& doc) {
		std::wstring aux = doc.ProfessionalID() + L" "
			+ doc.GetName(Names::FIRST_NAME) + L" "
			+ doc.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, controlID, msg, NULL, (LPARAM)aux.c_str());
		});

}

void UpdatePatientList(HWND hWnd, List<Patient>& patList, int controlID, bool cb) {

	int clear = LB_RESETCONTENT;
	int msg = LB_ADDSTRING;
	if (cb) {
		msg = CB_ADDSTRING;
		clear = CB_RESETCONTENT;
	}

	SendDlgItemMessageW(hWnd, controlID, clear, NULL, NULL);

	patList.ForEach([&](Patient& pat) {
		std::wstring aux = std::to_wstring(pat.Key()) + L" "
			+ pat.GetName(Names::FIRST_NAME) + L" "
			+ pat.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, controlID, msg, NULL, (LPARAM)aux.c_str());
		});

}

void UpdateSpecialityList(HWND hWnd, List<Speciality>& speList, int controlID, bool cb) {

	int clear = LB_RESETCONTENT;
	int msg = LB_ADDSTRING;
	if (cb) {
		msg = CB_ADDSTRING;
		clear = CB_RESETCONTENT;
	}

	SendDlgItemMessageW(hWnd, controlID, clear, NULL, NULL);

	speList.ForEach([&](Speciality& spe) {
		std::wstring aux = std::to_wstring(spe.Key()) + L" " + spe.Name();
		SendDlgItemMessageW(hWnd, controlID, msg, NULL, (LPARAM)aux.c_str());
		});

}

void UpdateAppointmentList(HWND hWnd, List<Appointment>& appList, int controlID, bool cb) {

	int clear = LB_RESETCONTENT;
	int msg = LB_ADDSTRING;
	if (cb) {
		msg = CB_ADDSTRING;
		clear = CB_RESETCONTENT;
	}

	SendDlgItemMessageW(hWnd, controlID, clear, NULL, NULL);

	appList.ForEach([&](Appointment& app) {
		DateTime appDT = app.GetDateTime();
		std::wstring aux = std::to_wstring(app.Key()) + L" CITA" + appDT.DateTimeWstring();
		SendDlgItemMessageW(hWnd, controlID, msg, NULL, (LPARAM)aux.c_str());
		});

}

void UpdateMedicalOfficeList(HWND hWnd, List<MedOffice>& moList, int controlID, bool cb) {

	int clear = LB_RESETCONTENT;
	int msg = LB_ADDSTRING;
	if (cb) {
		msg = CB_ADDSTRING;
		clear = CB_RESETCONTENT;
	}

	SendDlgItemMessageW(hWnd, controlID, clear, NULL, NULL);

	moList.ForEach([&](MedOffice& medoff) {
		std::wstring aux = std::to_wstring(medoff.Number()) + L" Consultorio";
		SendDlgItemMessageW(hWnd, controlID, msg, NULL, (LPARAM)aux.c_str());
		});

}

std::wstring CopyImageFile(const std::wstring& imgFile, const std::wstring& directory) {

	static int index = 0;

	std::wstring newFilePath;
	BOOL created;
	DWORD error;
	std::wstring filename;

	do {

		filename = std::to_wstring(index) + L".JPG";
		newFilePath = directory + L"\\" + filename;

		created = CopyFileW(imgFile.c_str(), newFilePath.c_str(), TRUE);
		error = GetLastError();

		index++;

	} while (!created);

	return newFilePath;

}

#pragma endregion Miscelaneous

#pragma region Query_App_Window

void InitQueryAppControls(HWND hWnd, void(*initWithGlobals)(HWND)) {

	if (initWithGlobals)
		initWithGlobals(hWnd);

	SendDlgItemMessageW(hWnd, IDC_QA_APPCODE_EDIT, EM_SETLIMITTEXT, 10, NULL);	//APP CODE EDIT CONTROL
	SendDlgItemMessageW(hWnd, IDC_QA_PID_EDIT, EM_SETLIMITTEXT, 15, NULL);		//PRO ID EDIT CONTROL

	{	//Query Type combo box
		wchar_t queryTypes[4][19] = { L"Por código de cita", L"Por semana",
		L"Por médico", L"Por especialidad" };

		for (int i = 0; i < 4; i++) {
			SendDlgItemMessageW(hWnd, IDC_QA_ATYPE_COMBO, CB_ADDSTRING, NULL, (LPARAM)queryTypes[i]);
		}

	}

	{	//Month combo box
		wchar_t months[12][11] = { L"Enero", L"Febrero", L"Marzo", L"Abril", L"Mayo",
		L"Junio", L"Julio", L"Agosto", L"Septiembre", L"Octubre", L"Noviembre", L"Diciembre" };

		for (int i = 0; i < 12; i++) {
			SendDlgItemMessageW(hWnd, IDC_QA_MONTH_COMBO, CB_ADDSTRING, NULL, (LPARAM)months[i]);
		}
	}

}

void SelectQueryType(HWND hWnd, int comboBoxID) {

	//Disable everything
	EnableWindow(GetDlgItem(hWnd, IDC_QA_APPCODE_EDIT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_WEEK_DTP), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_PID_EDIT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_SEARCHDR_CMD), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_MONTH_COMBO), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_SPE_COMBO), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_QA_WEEK_DTP2), FALSE);

	//Enable on selection
	int selection = (int)SendDlgItemMessageW(hWnd, comboBoxID, CB_GETCURSEL, NULL, NULL);

	switch (selection) {
	case 0:				//Por código de cita
		EnableWindow(GetDlgItem(hWnd, IDC_QA_APPCODE_EDIT), TRUE);
		break;
	case 1:				//Por semana
		EnableWindow(GetDlgItem(hWnd, IDC_QA_WEEK_DTP), TRUE);
		break;
	case 2:				//Por médico
		EnableWindow(GetDlgItem(hWnd, IDC_QA_PID_EDIT), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_QA_SEARCHDR_CMD), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_QA_MONTH_COMBO), TRUE);
		break;
	case 3:				//Por especialidad
		EnableWindow(GetDlgItem(hWnd, IDC_QA_SPE_COMBO), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_QA_WEEK_DTP2), TRUE);
		break;
	}

	//Enable make query button
	EnableWindow(GetDlgItem(hWnd, IDC_QA_SEARCHAPP_CMD), TRUE);

}

void QASearchDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST) {
	
	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_QA_PID_EDIT), aux);
	unsigned long proID = stoul(aux);
	Doctor temp(proID);
	if (drBST.BinarySearch(temp, temp)) {
		SetDlgItemTextW(hWnd, IDC_QA_DRNAME_EDIT, temp.GetName(Names::FULL_NAME).c_str());	//Show Doctor name
		SetDlgItemTextW(hWnd, IDC_QA_DRSPE_EDIT, std::to_wstring(temp.GetSpeciality()).c_str());	//Show Doctor spe code
	}
	else {
		SetDlgItemTextW(hWnd, IDC_QA_DRNAME_EDIT, L"NOT ENCONTRADO");
		SetDlgItemTextW(hWnd, IDC_QA_DRSPE_EDIT, L"0000");
	}
	
}

void MarkApp(HWND hWnd, List<Appointment>& appList, List<MedOffice>& moList) {

	unsigned int appKey = GetKeyFromLB(hWnd, IDC_QA_APP_LIST);

	Appointment& retVal = appList.Search<unsigned int>(appKey, [](const Appointment& app, const unsigned int& key) {
		return app.Key() == key;
		});

	retVal.MarkAttended(true);

	//Free the time
	unsigned int moKey = retVal.GetMedOfficeKey();
	DateTime appBegin = retVal.GetDateTime();
	DateTime appEnd;

	if (retVal.IsDouble())
		appEnd = DateTime::DateTimeAdd(appBegin, 0, 0, 0, 0, 40);
	else
		appEnd = DateTime::DateTimeAdd(appBegin, 0, 0, 0, 0, 20);

	MedOffice& appMO = retVal.GetMedOffice(moList);
	appMO.GetSchedule().SetFree(appEnd, appEnd);

}

void CancelApp(HWND hWnd, List<Appointment>& appList, List<MedOffice>& moList) {

	unsigned int appKey = GetKeyFromLB(hWnd, IDC_QA_APP_LIST);

	Appointment& retVal = appList.Search<unsigned int>(appKey, [](const Appointment& app, const unsigned int& key) {
		return app.Key() == key;
		});

	retVal.Cancel(true);

	//Free the time
	unsigned int moKey = retVal.GetMedOfficeKey();
	DateTime appBegin = retVal.GetDateTime();
	DateTime appEnd;

	if (retVal.IsDouble())
		appEnd = DateTime::DateTimeAdd(appBegin, 0, 0, 0, 0, 40);
	else
		appEnd = DateTime::DateTimeAdd(appBegin, 0, 0, 0, 0, 20);

	MedOffice& appMO = retVal.GetMedOffice(moList);
	appMO.GetSchedule().SetFree(appEnd, appEnd);

}

void QueryDoctorMonth(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer) {

	//Get Dr profesional ID
	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_QA_PID_EDIT), aux);
	unsigned long proID = std::stoul(aux);

	//Get Month
	unsigned short month = (unsigned short)SendDlgItemMessageW(hWnd, IDC_QA_MONTH_COMBO, CB_GETCURSEL, NULL, NULL) + 1;

	//QUERY
	List<Appointment> tempBuffer;
	GetAppByDoctor(appList, proID, tempBuffer, false);
	GetAppByMonth(tempBuffer, month, buffer, false);	//	Filtering

}

void QuerySpeciality(HWND hWnd, List<Appointment>& appList, BinarySearchTree<Doctor>& drBST, List<Appointment>& buffer) {

	//Get speciality key
	unsigned int speKey = GetKeyFromCB(hWnd, IDC_QA_SPE_COMBO);

	//Get the dateTime from dtp
	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(SYSTEMTIME));
	SendDlgItemMessageW(hWnd, IDC_QA_WEEK_DTP2, DTM_GETSYSTEMTIME, NULL, (LPARAM)&st);
	DateTime queryDT(st);

	//QUERY
	List<Appointment> tempBuffer;
	GetAppBySpe(appList, speKey, drBST, tempBuffer, false);	//Filtering
	GetAppByWeek(tempBuffer, queryDT, buffer, false);

}

void QueryWeek(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer) {

	SYSTEMTIME st;
	ZeroMemory(&st, sizeof(SYSTEMTIME));
	SendDlgItemMessageW(hWnd, IDC_QA_WEEK_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&st);
	DateTime queryDT(st);

	GetAppByWeek(appList, queryDT, buffer, false);

}

void QueryCode(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer) {

	std::wstring aux;
	GetWindowTextWstring(hWnd, aux);
	unsigned int appCode = (unsigned int)stoi(aux);
	GetAppByCode(appList, appCode, buffer, false);

}

void QueryMedOffice(HWND hWnd, List<Appointment>& appList, List<Appointment>& buffer) {

	unsigned int moKey = GetKeyFromCB(hWnd, IDC_QA_MO_COMBO);

	GetAppByMedicalOffice(appList, moKey, buffer, false);

}

bool GetAppByDoctor(List<Appointment>& appList, unsigned long proID, List<Appointment>& buffer, bool chain) {

	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	appList.ForEach([&](Appointment& app) {
		if (app.GetDoctorKey() == proID) {
			buffer.Push(app);
			found = true;
		}
		});

	return found;

}

bool GetAppBySpe(List<Appointment>& appList, unsigned int speKey,
	BinarySearchTree<Doctor>& drBST, List<Appointment>& buffer, bool chain) {

	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	appList.ForEach([&](Appointment& app) {
		if (app.GetSpeciality(drBST) == speKey) {
			buffer.Push(app);
			found = true;
		}
		});

	return found;

}

bool GetAppByMonth(List<Appointment>& appList, unsigned short month, List<Appointment>& buffer, bool chain) {

	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	if (month >= 0 && month <= 12) {

		appList.ForEach([&](Appointment& app) {
			DateTime appDT = app.GetDateTime();
			if (appDT.Month() == month) {
				buffer.Push(app);
				found = true;
			}
			});

	}

	return found;

}

bool GetAppByWeek(List<Appointment>& appList, const DateTime& dateTimeRef, List<Appointment>& buffer, bool chain) {

	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	TimePeriod week = GetWeekFromDate(dateTimeRef);

	appList.ForEach([&](Appointment& app) {
		DateTime appDT = app.GetDateTime();
		if (appDT >= week.Begin() && appDT <= week.End()) {		//if the app dateTime is in between
			buffer.Push(app);
			found = true;
		}
		});

	return found;
}

bool GetAppByCode(List<Appointment>& appList, unsigned int appCode, List<Appointment>& buffer, bool chain) {

	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	appList.ForEach([&](Appointment& app) {
		if (app.Key() == appCode) {
			buffer.Push(app);
			found = true;
		}
		});

	return found;

}

bool GetAppByCode(List<Appointment>& appList, unsigned int appCode, Appointment& retVal) {

	bool found = false;

	retVal = appList.Search<unsigned int>(appCode, [](const Appointment& app, const unsigned int& key) {
		return app.Key() == key;
		});

	if (retVal.Key() != 0)
		found = true;

	return found;

}

bool GetAppByMedicalOffice(List<Appointment>& appList, unsigned int moKey, List<Appointment>& buffer, bool chain) {
	
	bool found = false;
	if (!chain)
		buffer.Clear();		//Clear the buffer

	appList.ForEach([&](Appointment& app) {
		if (app.GetMedOfficeKey() == moKey) {
			buffer.Push(app);
			found = true;
		}
		});

	return found;

}

void ShowQuery(HWND hWnd, List<Appointment>& appList) {

	//clear the list
	SendDlgItemMessageW(hWnd, IDC_QA_APP_LIST, LB_RESETCONTENT, NULL, NULL);

	appList.ForEach([&](Appointment& app) {
		DateTime appDT = app.GetDateTime();
		std::wstring aux = std::to_wstring(app.Key()) + L" CITA" + appDT.DateTimeWstring();
		SendDlgItemMessageW(hWnd, IDC_QA_APP_LIST, LB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

}

void SaveQueryFile(List<Appointment>& appList, List<Patient>& patList, BinarySearchTree<Doctor>& drBST, const wchar_t* fileName) {

	std::wofstream queryFile;

	std::wstring appDirectory = DocumentsDirectory() + MAIN_FOLDER;
	int res = _wmkdir(appDirectory.c_str());
	appDirectory += L"\\Query";
	res = _wmkdir(appDirectory.c_str());
	std::wstring fullpath = appDirectory + L"\\" + fileName;

	queryFile.open(fullpath, ios::out | ios::trunc);

	if (queryFile.is_open()) {

		appList.ForEach([&](Appointment& app) {

			Doctor appDr = app.GetDoctorInfo(drBST);
			Patient appPat = app.GetPatientInfo(patList);

			queryFile << L"-----------------------------------------------" << std::endl;
			queryFile << L"# " << app.Key() << std::endl;
			queryFile << L"Fecha: " << app.GetDateTime() << std::endl;
			queryFile << L"# Consultorio: " << app.GetMedOfficeKey() << std::endl;
			queryFile << L"Cod. Especialidad: " << appDr.GetSpeciality() << std::endl;
			queryFile << L"Fecha: " << app.GetDateTime() << std::endl;
			queryFile << L"Médico: " << appDr.GetName(Names::FULL_NAME) << std::endl;
			queryFile << L"Paciente: " << appPat.GetName(Names::FULL_NAME) << std::endl;
			queryFile << L"Tel: " << appPat.GetPhoneNumber().PhoneNumberString(true) << std::endl;
			queryFile << L"Atendida: " << ((app.IsAttended()) ? L"si" : L"no") << std::endl;
			queryFile << L"Cancelada: " << ((app.IsCanceled() ? L"si" : L"no")) << std::endl;
			queryFile << L"-----------------------------------------------" << std::endl;

			});

		queryFile.close();
		std::wstring msgBox = L"Se generó un archivo de texto con su consulta en: " + fullpath;
		MessageBoxW(NULL, msgBox.c_str(), L"Guardado completo", MB_ICONEXCLAMATION | MB_OK);

	}
	else {
		MessageBoxW(NULL, L"El archivo no se pudo abrir", L"Error al abrir el archivo", MB_ICONEXCLAMATION | MB_OK);
	}

}

void ClearQuery(HWND hWnd, List<Appointment>& qBuffer) {

	//CLEAR THE LIST
	SendDlgItemMessageW(hWnd, IDC_QA_APP_LIST, LB_RESETCONTENT, NULL, NULL);

	//SET THE COMBO BOXES SELECTION TO UNSPECIFIED
	SendDlgItemMessageW(hWnd, IDC_QA_ATYPE_COMBO, CB_SETCURSEL, -1, NULL);
	SendDlgItemMessageW(hWnd, IDC_QA_SPE_COMBO, CB_SETCURSEL, -1, NULL);
	SendDlgItemMessageW(hWnd, IDC_QA_MONTH_COMBO, CB_SETCURSEL, -1, NULL);
	SendDlgItemMessageW(hWnd, IDC_QA_MO_COMBO, CB_SETCURSEL, -1, NULL);

	//CLEAR SOME EDIT CONTROLS
	SetDlgItemTextW(hWnd, IDC_QA_DRNAME_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_QA_DRSPE_EDIT, L"");

	//DISABLE CONTROLS
	HWND controls[6];

	controls[0] = GetDlgItem(hWnd, IDC_QA_APPCODE_EDIT);
	controls[1] = GetDlgItem(hWnd, IDC_QA_PID_EDIT);
	controls[2] = GetDlgItem(hWnd, IDC_QA_SEARCHDR_CMD);
	controls[3] = GetDlgItem(hWnd, IDC_QA_MONTH_COMBO);
	controls[4] = GetDlgItem(hWnd, IDC_QA_WEEK_DTP);
	controls[5] = GetDlgItem(hWnd, IDC_QA_SPE_COMBO);

	for (HWND control : controls) {
		EnableWindow(control, FALSE);
	}

	//DISABLE MAKE QUERY BUTTON
	EnableWindow(GetDlgItem(hWnd, IDC_QA_SEARCHAPP_CMD), FALSE);

	//CLEAR THE BUFFER
	qBuffer.Clear();

}

void ViewSelectedAppointment(HWND hWnd, List<Appointment>& appList, DLGPROC viewerProc) {

	unsigned int key = GetKeyFromLB(hWnd, IDC_QA_APP_LIST);

	Appointment& appRef = appList.Search<unsigned int>(key, [](const Appointment& app, const unsigned int& _key) {
		return app.Key() == _key;
		});

	CreateDialogParamW(NULL, MAKEINTRESOURCEW(IDD_VIEW_APP), hWnd, viewerProc, (LPARAM)&appRef);

}

TimePeriod GetWeekFromDate(const DateTime& dateTimeRef) {

	unsigned short weekDayRef = dateTimeRef.WeekDay();
	unsigned short daysUp = 6 - weekDayRef;

	DateTime lastDay = DateTime::DateTimeAdd(dateTimeRef, daysUp, 0, 0, 0, 0);

	return TimePeriod(dateTimeRef, lastDay);

}

#pragma endregion Query_App_Window

#pragma region Reg_App_Window

void InitRegAppControls(HWND hWnd, void(*initWithGlobals)(HWND)) {
	
	if (initWithGlobals)
		initWithGlobals(hWnd);

	SendDlgItemMessageW(hWnd, IDC_RA_SELSPE_COMBO, CB_INSERTSTRING, 0, (LPARAM)L"TODOS");

	SendDlgItemMessageW(hWnd, IDC_RA_DESC_EDIT, EM_SETLIMITTEXT, 300, NULL);	//DESCRIPT EDIT CONTROL
	SendDlgItemMessageW(hWnd, IDC_RA_PHONENUM_EDIT, EM_SETLIMITTEXT, 10, NULL);	//PHONE NUM EDIT CONTROL

}

void GetAppRegisterInfo(HWND hWnd, Appointment& app) {

	//Get patient
	app.SetPatient(GetKeyFromCB(hWnd, IDC_RA_SELPATIENT_COMBO));

	//Get doctor
	app.SetDoctor(GetIDFromCB(hWnd, IDC_RA_SELDR_COMBO));

	//Get Data from edit controls
	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RA_DESC_EDIT), aux);	//App description
	app.SetDescription(aux);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RA_DESC_EDIT), aux);	//Phone number
	app.GetPhoneNumber().SetPhoneNumber(aux);

	//Get Date Time
	SYSTEMTIME date = { 0 }, time = { 0 };
	SendDlgItemMessageW(hWnd, IDC_RA_APPDATE_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&date);
	SendDlgItemMessageW(hWnd, IDC_RA_APPTIME_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&time);
	date.wHour = time.wHour;
	date.wMinute = time.wMinute;
	app.SetAppointmentDateTime(DateTime(date));

	//Get if it is a double appointment
	bool isDouble = (bool)SendDlgItemMessageW(hWnd, IDC_RA_DOUBLEAPP_CHECK, BM_GETCHECK, NULL, NULL);
	app.SetDouble(isDouble);

	//Get Medical office
	app.SetMedOffice(GetKeyFromCB(hWnd, IDC_RA_SELMO_COMBO));

}

ValidationError ValidateAppRegister(HWND hWnd) {

	ErrorCode errorCode = ErrorCode::EC_NO_ERROR;
	HWND errorSrc = NULL;

	{
		HWND comboBoxes[4];
		comboBoxes[0] = GetDlgItem(hWnd, IDC_RA_SELPATIENT_COMBO);
		comboBoxes[1] = GetDlgItem(hWnd, IDC_RA_SELMO_COMBO);
		comboBoxes[2] = GetDlgItem(hWnd, IDC_RA_SELDR_COMBO);
		comboBoxes[3] = GetDlgItem(hWnd, IDC_RA_SELSPE_COMBO);

		for (int i = 0; i < 4; i++) {
			if (SendMessageW(comboBoxes[i], CB_GETCURSEL, NULL, NULL) == CB_ERR) {
				errorCode = ErrorCode::EC_NO_SEL;
				errorSrc = comboBoxes[i];
				break;
			}
		}
	}

	{
		HWND descriptionEdit, phoneNumEdit;
		descriptionEdit = GetDlgItem(hWnd, IDC_RA_DESC_EDIT);
		phoneNumEdit = GetDlgItem(hWnd, IDC_RA_PHONENUM_EDIT);
		
		if (GetWindowTextLengthW(descriptionEdit) == 0) {
			errorCode = ErrorCode::EC_EMPTY;
			errorSrc = descriptionEdit;
		}

		if (GetWindowTextLengthW(phoneNumEdit) == 0) {
			errorCode = ErrorCode::EC_EMPTY;
			errorSrc = phoneNumEdit;
		}
		else if (GetWindowTextLengthW(phoneNumEdit) < 10) {
			errorCode = ErrorCode::EC_INVALID_SIZE;
			errorSrc = phoneNumEdit;
		}

	}

	SYSTEMTIME appDate, appTime;
	HWND dateDTP, timeDTP;
	dateDTP = GetDlgItem(hWnd, IDC_RA_APPDATE_DTP);
	timeDTP = GetDlgItem(hWnd, IDC_RA_APPTIME_DTP);

	SendMessageW(dateDTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&appDate);
	SendMessageW(timeDTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&appTime);

	appDate.wHour = appTime.wHour;
	appDate.wMinute = appTime.wMinute;

	DateTime dateTime(appDate);
	if (dateTime < DateTime::RightNow()) {
		errorCode = ErrorCode::EC_INVALID_DATE;
		errorSrc = dateDTP;
	}

	ValidationError validationError = { errorCode, errorSrc };

	return validationError;

}

bool ValidDrSchAppTime(const Appointment& app, BinarySearchTree<Doctor>& drBST) {

	DateTime appDT = app.GetDateTime(), endDT = app.GetDateTime();
	Doctor drInfo = app.GetDoctorInfo(drBST);
	Schedule drSch = drInfo.GetSchedule();
	Schedule auxSch;
	
	if (app.IsDouble())
		endDT.AddTime(0, 0, 0, 0, 40);
	else
		endDT.AddTime(0, 0, 0, 0, 20);

	auxSch.Reserve(appDT, endDT);
	auxSch.SetWorkDay(appDT.WeekDay(), true);

	if (!drSch.ConflictWith(auxSch, SchValidate::SCH_ALL)) {
		//THE DOCTOR SCHEDULE AND THE APP TIME DO NOT MATCH
		MessageBoxW(NULL, L"Error", L"El doctor seleccionado no está disponible en esa fecha y hora",
			MB_ICONERROR | MB_OK);
		return false;
	}

	return true;

}

void ReserveApp(Appointment& app, List<Appointment>& appList, List<MedOffice>& moList) {

	//Get the info of the selected medical office
	MedOffice& selMO = app.GetMedOffice(moList);
	
	DateTime beginTime = app.GetDateTime(), endTime = app.GetDateTime();
	bool error = false;

	if (app.IsDouble()) {

		endTime.AddTime(0, 0, 0, 0, 40);	//Add the 40 minutes for the two appointments

		if (selMO.GetSchedule().IsAvailable(beginTime, endTime))
			selMO.GetSchedule().Reserve(beginTime, endTime);
		else
			error = true;
	}
	else {

		endTime.AddTime(0, 0, 0, 0, 20);	//Add the 20 minutes for the appointment

		if (selMO.GetSchedule().IsAvailable(beginTime, endTime))
			selMO.GetSchedule().Reserve(beginTime, endTime);
		else
			error = true;

	}

	if (error) {
		MessageBoxW(NULL, L"Ya hay una cita a esa hora", L"Error", MB_ICONERROR | MB_OK);
	}
	else {
		appList.Push(app);
		std::wstring msg = L"Se ha reservado la cita con código: " + std::to_wstring(app.Key());
		MessageBoxW(NULL, msg.c_str(), L"Reserva", MB_ICONEXCLAMATION | MB_OK);
	}

}

void ClearAppRegister(HWND hWnd) {

	//deselect combo boxes
	SendDlgItemMessageW(hWnd, IDC_RA_SELPATIENT_COMBO, CB_SETCURSEL, (WPARAM)-1, NULL);
	SendDlgItemMessageW(hWnd, IDC_RA_SELSPE_COMBO, CB_SETCURSEL, (WPARAM)-1, NULL);
	SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_SETCURSEL, (WPARAM)-1, NULL);
	SendDlgItemMessageW(hWnd, IDC_RA_SELMO_COMBO, CB_SETCURSEL, (WPARAM)-1, NULL);

	//clear edit controls
	SetDlgItemTextW(hWnd, IDC_RA_DESC_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_RA_PHONENUM_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_NAME_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_NAME_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_GENDER_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_H_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_W_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_BLOOD_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_PATIENT_BIRTHD_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_DR_NAME_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_DR_PROID_EDIT, L"");

	//restart date time pickers
	SYSTEMTIME now;
	GetLocalTime(&now);
	SendDlgItemMessageW(hWnd, IDC_RA_APPDATE_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);
	SendDlgItemMessageW(hWnd, IDC_RA_APPTIME_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);

	//restart check box
	SendDlgItemMessageW(hWnd, IDC_RA_DOUBLEAPP_CHECK, BM_SETCHECK, BST_UNCHECKED, NULL);

	//clear picture control
	SetPictureControlImg(L"", 170, 170, hWnd, IDC_DR_IMG);

}

Patient& FindPatientWithKey(List<Patient>& patList, unsigned int key) {

	return patList.Search<unsigned int>(key, [](const Patient& pat, const unsigned int& s_key) {
		return pat.Key() == s_key;
		});

}

void SelectPatient(HWND hWnd, int comboBoxID, List<Patient>& patList) {

	unsigned int key = GetKeyFromCB(hWnd, comboBoxID);

	if (key != 0) {
		Patient temp = patList.Search<unsigned int>(key, [](const Patient& pat, const unsigned int& key)->bool {
			return pat.Key() == key;
			});

		//Show Patient info
		SetDlgItemTextW(hWnd, IDC_PATIENT_NAME_EDIT, temp.GetName(Names::FULL_NAME).c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_GENDER_EDIT, temp.GetGenderString().c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_AGE_EDIT, std::to_wstring(temp.Age()).c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_H_EDIT, std::to_wstring(temp.Height()).c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_W_EDIT, std::to_wstring(temp.Weight()).c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_BLOOD_EDIT, temp.GetBloodTypeString().c_str());
		SetDlgItemTextW(hWnd, IDC_PATIENT_BIRTHD_EDIT, temp.Birthdate().DateString().c_str());

	}

}

void SelectDoctor(HWND hWnd, int comboBoxID, BinarySearchTree<Doctor>& drList) {

	unsigned long id = GetIDFromCB(hWnd, IDC_RA_SELDR_COMBO);
	Doctor temp(id);

	if (drList.BinarySearch(temp, temp)) {
		SetDlgItemTextW(hWnd, IDC_DR_NAME_EDIT, temp.GetName(Names::FULL_NAME).c_str());
		SetDlgItemTextW(hWnd, IDC_DR_PROID_EDIT, temp.ProfessionalID().c_str());
		SetPictureControlImg(temp.GetImagePath().c_str(), 170, 170, hWnd, IDC_DR_IMG);
	}

}

void FilterDoctorsWithSpeciality(HWND hWnd, BinarySearchTree<Doctor>& drBST) {

	if (SendDlgItemMessageW(hWnd, IDC_RA_SELSPE_COMBO, CB_GETCURSEL, NULL, NULL) != 0) {
		//Get the speciality key
		unsigned int key = GetKeyFromCB(hWnd, IDC_RA_SELSPE_COMBO);
		//Get the doctors with that speciality
		std::vector<Doctor> buffer;
		DoctorsPerSpeciality(key, drBST, &buffer);

		//Restart the combo box
		SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_RESETCONTENT, NULL, NULL);
		//Add the doctors with that speciality
		for (Doctor& doc : buffer) {
			std::wstring aux = doc.ProfessionalID() + L" "
				+ doc.GetName(Names::FIRST_NAME) + L" "
				+ doc.GetName(Names::FIRST_LASTNAME);
			SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		}
	}
	else {
		UpdateDoctorList(hWnd, drBST, IDC_RA_SELDR_COMBO, true);
	}

}

#pragma endregion Reg_App_Window

#pragma region Register_Doctor_Window

void InitRegMedControls(HWND hWnd, void(*initWithGlobals)(HWND)) {

	if (initWithGlobals)
		initWithGlobals(hWnd);

	//USE CONTROLS SUBCLASSES
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);

	//EDIT CONTROLS LIMIT TEXT
	SendDlgItemMessageW(hWnd, IDC_RM_DR_FNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);		//FIRST NAME
	SendDlgItemMessageW(hWnd, IDC_RM_DR_SNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);		//SECOND NAME
	SendDlgItemMessageW(hWnd, IDC_RM_DR_FLNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);		//FIRST LASTNAME
	SendDlgItemMessageW(hWnd, IDC_RM_DR_SLNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);		//SECOND LASTNAME
	SendDlgItemMessageW(hWnd, IDC_RM_PHONENUM_EDIT, EM_SETLIMITTEXT, 10, NULL);		//PHONE NUM
	SendDlgItemMessageW(hWnd, IDC_RM_PROID_EDIT, EM_SETLIMITTEXT, 20, NULL);		//PROFESIONAL ID

	//FILL COMBO BOXES
	SendDlgItemMessageW(hWnd, IDC_RM_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Masculino");	//GENDER
	SendDlgItemMessageW(hWnd, IDC_RM_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Femenino");	//	|
	SendDlgItemMessageW(hWnd, IDC_RM_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Otro");		// _|

}

void GetDoctorRegisterInfo(HWND hWnd, Doctor& doctor) {

	{	//Get edit control and static text info
		std::wstring fname, sname, flname, slname;
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT), fname);	//First name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT), sname);	//Second name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT), flname);	//First last name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT), slname);	//Second last name
		doctor.SetName(fname, sname, flname, slname);

		std::wstring aux;
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT), aux);		//Phone number
		doctor.GetPhoneNumber().SetPhoneNumber(aux);
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_PROID_EDIT), aux);			//Profesional id
		doctor.SetProfessionalID(std::stoul(aux));
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_IMGPATH), aux);			//Img file path
		if (aux != L"") {
			aux = CopyImageFile(aux, DocumentsDirectory() + MAIN_FOLDER + IMG_FOLDER);
			doctor.SetImagePath(aux);
		}
	}

	{	//Get date time pickers info
		SYSTEMTIME bdate, from, to;
		SendDlgItemMessageW(hWnd, IDC_RM_BIRTHD_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);		//Birth date
		doctor.SetBirthdate(Date(bdate));
		SendDlgItemMessageW(hWnd, IDC_RM_SCHE_FROM_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&from);	//Schedule
		SendDlgItemMessageW(hWnd, IDC_RM_SCHE_TO_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&to);
		doctor.GetSchedule().Reserve(DateTime(from), DateTime(to));
	}

	{	//Get check boxes state
		HWND checkBoxes[5];
		checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
		checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
		checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
		checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
		checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday

		for (int i = 0; i < 5; i++) {
			bool isWorkDay = (bool)SendMessageW(checkBoxes[i], BM_GETCHECK, NULL, NULL);
			if (isWorkDay)
				doctor.GetSchedule().SetWorkDay((unsigned int)(i + 1), isWorkDay);

		}
	}

	//Get gender
	HWND genderCB = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);	//Gender
	Gender gender = (Gender)SendMessageW(genderCB, CB_GETCURSEL, NULL, NULL);
	doctor.SetGender(gender);

	//Get medical office
	doctor.SetMedOfficeNumber(GetKeyFromCB(hWnd, IDC_RM_MO_COMBO));

	//Get speciality
	doctor.SetSpeciality(GetKeyFromCB(hWnd, IDC_RM_SPE_COMBO));

}

ValidationError ValidateDoctorRegister(HWND hWnd) {

	ValidationError validationError = { ErrorCode::EC_NO_ERROR, NULL };

	{	//Validate edit Controls
		HWND editControls[6];
		editControls[0] = GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT);	//First name
		editControls[1] = GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT);	//Second name
		editControls[2] = GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT);	//First last name
		editControls[3] = GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT);	//Second last name
		editControls[4] = GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT);	//Phone number
		editControls[5] = GetDlgItem(hWnd, IDC_RM_PROID_EDIT);		//Profesional ID

		for (int i = 0; i < 6; i++) {
			if (GetWindowTextLengthW(editControls[i]) == 0) {
				validationError.errorCode = ErrorCode::EC_EMPTY;
				validationError.errorSrc = editControls[i];
				break;
			}
		}

		//Validate phone number length
		if (GetWindowTextLengthW(editControls[4]) < 10) {
			validationError.errorCode = ErrorCode::EC_INVALID_SIZE;
			validationError.errorSrc = editControls[4];
		}
	}

	{	//Validate combo boxes
		HWND genderCB = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);	//Gender
		HWND medOffCB = GetDlgItem(hWnd, IDC_RM_MO_COMBO);		//Medical office

		if (SendMessageW(genderCB, CB_GETCURSEL, NULL, NULL) == CB_ERR) {
			validationError.errorCode = ErrorCode::EC_NO_SEL;
			validationError.errorSrc = genderCB;
		}
		else if (SendMessageW(medOffCB, CB_GETCURSEL, NULL, NULL) == CB_ERR) {
			validationError.errorCode = ErrorCode::EC_NO_SEL;
			validationError.errorSrc = medOffCB;
		}
	}

	{	//Validate birthdate
		SYSTEMTIME bdate;
		HWND bdateDTP = GetDlgItem(hWnd, IDC_RM_BIRTHD_DTP);
		SendMessageW(bdateDTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);
		DateTime bdateDT(bdate);
		if (bdateDT > DateTime::RightNow()) {
			validationError.errorCode = ErrorCode::EC_INVALID_DATE;
			validationError.errorSrc = bdateDTP;
		}
	}

	{	//validate check boxes (at least one checked)
		HWND checkBoxes[5];
		checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
		checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
		checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
		checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
		checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday

		bool allUnchecked = true;

		for (int i = 0; i < 5; i++) {
			if (SendMessageW(checkBoxes[i], BM_GETCHECK, NULL, NULL) == BST_CHECKED) {
				allUnchecked = false;
				break;
			}
		}

		if (allUnchecked) {
			validationError.errorCode = ErrorCode::EC_NO_SEL;
			validationError.errorSrc = checkBoxes[0];
		}

	}

	return validationError;

}

bool ValidateDoctorPerMO(HWND hWnd, BinarySearchTree<Doctor>& drBST, Doctor& toValidate) {
	
	std::vector<Doctor> doctorsInMO;

	//Get the selected medical office number
	unsigned int moKey = GetKeyFromCB(hWnd, IDC_RM_MO_COMBO);
	
	//Get all doctors in that medical office
	drBST.ExecuteInorder([&](Doctor& dr) {
		if (dr.GetMedOfficeNum() == moKey)
			doctorsInMO.push_back(dr);
		});

	//Check if there are conflicts with the schedules of those doctors and the new one
	bool conflict = false;
	for (auto& doctor : doctorsInMO) {
		if (doctor.GetSchedule().ConflictWith(toValidate.GetSchedule(),SchValidate::SCH_ALL)) {
			conflict = true;
			break;
		}
	}

	if (conflict) {
		MessageBoxW(hWnd, L"Ya hay un doctor en el mismo horario y consultorio", L"Error",
			MB_ICONERROR | MB_OK);
	}
	
	return !conflict;

}

void ClearDoctorRegister(HWND hWnd) {

	{	//Clear edit controls
		HWND editControls[6];
		editControls[0] = GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT);	//First name
		editControls[1] = GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT);	//Second name
		editControls[2] = GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT);	//First last name
		editControls[3] = GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT);	//Second last name
		editControls[4] = GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT);	//Phone number
		editControls[5] = GetDlgItem(hWnd, IDC_RM_PROID_EDIT);		//Profesional ID

		for (int i = 0; i < 6; i++) {
			SetWindowTextW(editControls[i], L"");
		}
	}

	{	//Reset combo boxes
		HWND genderCB = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);	//Gender
		HWND medOffCB = GetDlgItem(hWnd, IDC_RM_MO_COMBO);		//Medical office

		SendMessageW(genderCB, CB_SETCURSEL, -1, NULL);
		SendMessageW(medOffCB, CB_SETCURSEL, -1, NULL);
	}

	{	//Reset date time pickers
		HWND dateTimePickers[3];
		dateTimePickers[0] = GetDlgItem(hWnd, IDC_RM_BIRTHD_DTP);		//Birthdate
		dateTimePickers[1] = GetDlgItem(hWnd, IDC_RM_SCHE_FROM_DTP);	//Schedule from
		dateTimePickers[2] = GetDlgItem(hWnd, IDC_RM_SCHE_TO_DTP);		//Schedule to

		SYSTEMTIME now;
		GetLocalTime(&now);

		for (int i = 0; i < 3; i++) {
			SendMessageW(dateTimePickers[i], DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);
		}
	}

	{	//Reset check boxes
		HWND checkBoxes[5];
		checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
		checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
		checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
		checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
		checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday

		for (int i = 0; i < 5; i++) {
			SendMessageW(checkBoxes[i], BM_SETCHECK, BST_UNCHECKED, NULL);
		}
	}

	//Reset doctor image picture box
	SetDlgItemTextW(hWnd, IDC_RM_IMGPATH, L"");
	SetPictureControlImg(L"", 0, 0, hWnd, IDC_DR_IMG);

}

void LoadDoctorImg(HWND hWnd, int width, int height) {

	std::wstring buffer;

	if (GetImageFilename(hWnd, buffer)) {
		SetDlgItemTextW(hWnd, IDC_RM_IMGPATH, buffer.c_str());
		SetPictureControlImg(buffer.c_str(), width, height, hWnd, IDC_DR_IMG);
	}

}

bool GetImageFilename(HWND parent, std::wstring& buffer) {

	wchar_t szFile[MAX_PATH];

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = parent;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Image Files\0*.JPG;*.BMP;*.PNG\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	bool result = GetOpenFileNameW(&ofn);

	if (result) {
		buffer = szFile;
	}

	return result;

}

void ViewSelectedDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST, DLGPROC viewerProc) {

	unsigned long id = GetIDFromLB(hWnd, IDC_DR_LIST);

	Doctor key(id);
	Doctor* drRef = nullptr;

	if (drBST.BinarySearch(&drRef, key)) {
		CreateDialogParamW(NULL, MAKEINTRESOURCEW(IDD_VIEW_MED), hWnd, viewerProc, (LPARAM)drRef);
	}

}

void DeleteSelectedDoctor(HWND hWnd, BinarySearchTree<Doctor>& drBST) {

	unsigned long id = GetIDFromLB(hWnd, IDC_DR_LIST);

	Doctor key(id);

	drBST.Delete(key);

	//TODO: UPDATE LIST

}

void SaveDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST) {

	//Fill the open file name structure

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	wchar_t filepath[MAX_PATH] = L"DOCTOR_REPORT";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"Text Files (*.txt)\0*.TXT\0";
	ofn.lpstrFile = filepath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"TXT";
	ofn.lpstrInitialDir = DocumentsDirectory().c_str();
	ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN;

	if (GetSaveFileNameW(&ofn)) {
		GenDoctorReport(hWnd, drBST, filepath);
	}

}

void GenDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST, const std::wstring& filePath) {

	std::wofstream file;
	//Create doctor list
	List<Doctor> drList;
	drBST.ExecutePostorder([&](Doctor& doc) {
		drList.Push(doc);
		});
	
	if (!drList.IsEmpty()) {
		//Quicksort the list
		drList.QuickSort();
		//Write the data in the file
		file.open(filePath, ios::out | ios::trunc);

		if (file.is_open()) {
			drList.ForEach([&](Doctor& dr) {
				file << dr.ProfessionalID() << L" "
					<< dr.GetName(Names::FULL_NAME) << L" "
					<< dr.GetPhoneNumber().PhoneNumberString(false) << std::endl;
				});

			file.close();
			MessageBoxW(hWnd, L"El reporte se ha guardado con éxito.",
				L"Información de Archivo", MB_ICONEXCLAMATION | MB_OK);
		}
		else {
			MessageBoxW(hWnd, L"No se ha podido guardar el reporte.",
				L"Error al guardar", MB_ICONERROR | MB_OK);
		}
	}
	else {
		MessageBoxW(hWnd, L"No hay información para guardar.",
			L"Error al guardar", MB_ICONERROR | MB_OK);
	}

}

#pragma endregion Register_Doctor_Window

#pragma region Reg_Patient_Window

void InitRegPatientControls(HWND hWnd, void(*initWithGlobals)(HWND)) {

	if (initWithGlobals)
		initWithGlobals(hWnd);

	//USE CONTROLS SUBCLASSES
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RP_FNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RP_SNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RP_FLNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RP_SLNAME_EDIT), AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL, NULL);
	SetWindowSubclass(GetDlgItem(hWnd, IDC_RP_W_EDIT), FloatOnlyEditCtrlWndProc, ID_FLOAT_EDIT_CONTROL, NULL);

	//EDIT CONTROLS LIMIT TEXT
	SendDlgItemMessageW(hWnd, IDC_RP_FNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);	//FIRST NAME
	SendDlgItemMessageW(hWnd, IDC_RP_SNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);	//SECOND NAME
	SendDlgItemMessageW(hWnd, IDC_RP_FLNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);	//FIRST LASTNAME
	SendDlgItemMessageW(hWnd, IDC_RP_SLNAME_EDIT, EM_SETLIMITTEXT, 30, NULL);	//SECOND LASTNAME
	SendDlgItemMessageW(hWnd, IDC_RP_PHONENUM_EDIT, EM_SETLIMITTEXT, 10, NULL);	//PHONE NUM
	SendDlgItemMessageW(hWnd, IDC_RP_REFER_EDIT, EM_SETLIMITTEXT, 200, NULL);	//REFERENCE
	SendDlgItemMessageW(hWnd, IDC_RP_H_EDIT, EM_SETLIMITTEXT, 5, NULL);			//HEIGHT
	SendDlgItemMessageW(hWnd, IDC_RP_W_EDIT, EM_SETLIMITTEXT, 5, NULL);			//WEIGHT

	//FILL COMBO BOXES
	SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Masculino");	//GENDER
	SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Femenino");	//	|
	SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_ADDSTRING, NULL, (LPARAM)L"Otro");		// _|

	{	//BLOOD TYPES
		wchar_t bloodTypes[9][11] = { L"Indefinido", L"A+", L"A-", L"B+", L"A-",
			L"O+", L"O-", L"AB+", L"AB-" };

		for (int i = 0; i < 9; i++) {
			SendDlgItemMessageW(hWnd, IDC_RP_BLOOD_COMBO, CB_ADDSTRING, NULL, (LPARAM)bloodTypes[i]);
		}

	}

}

void ViewSelectedPatient(HWND hWnd, List<Patient>& patList, DLGPROC viewerProc) {

	unsigned int key = GetKeyFromLB(hWnd, IDC_PATIENT_LIST);

	Patient& patRef = patList.Search<unsigned int>(key, [](const Patient& pat, const unsigned int& _key) {
		return pat.Key() == _key;
		});

	CreateDialogParamW(NULL, MAKEINTRESOURCEW(IDD_VIEW_PATIENT), hWnd, viewerProc, (LPARAM)&patRef);

}

void DeleteSelectedPatient(HWND hWnd, List<Patient>& patList, List<Appointment>& appList) {

	unsigned int key = GetKeyFromLB(hWnd, IDC_PATIENT_LIST);
	int deleted = -1, i = 0;

	//VALIDATE APPOINTMENTS WITH THAT PATIENT
	if (AppointmentsPerPatient(key, appList) == 0) {

		//Find the element with that key
		patList.ForEach([&](Patient& pat) {
			if (pat.Key() == key)
				deleted = i;	//if found: save the current index
			i++;
			});

		if (deleted > -1)
			Patient tmp = patList.Delete((unsigned int)deleted);
	}
	else {
		MessageBoxW(NULL, L"El paciente ya tiene una cita registrada", L"No se ha podido eliminar",
			MB_ICONERROR | MB_OK);
	}

}

void SavePatientReport(HWND hWnd, List<Patient>& patList) {

	//Fill the open file name structure

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

	wchar_t filepath[MAX_PATH] = L"PATIENT_REPORT";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"Text Files (*.txt)\0*.TXT\0";
	ofn.lpstrFile = filepath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"TXT";
	ofn.lpstrInitialDir = DocumentsDirectory().c_str();
	ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN;

	if (GetSaveFileNameW(&ofn)) {
		GenPatientReport(hWnd, patList, filepath);
	}

}

void GenPatientReport(HWND hWnd, List<Patient>& patList, const std::wstring& filepath) {

	Patient* patArray = patList.GetArray();
	size_t size = patList.Size();

	if (patArray) {
		std::wofstream file;
		file.open(filepath, ios::out | ios::trunc);

		if (file.is_open()) {

			Heap<Patient>::HeapSort(patArray, size, HeapType::MIN_H);

			for (size_t i = 0; i < size; i++) {
				file << patArray[i].GetName(Names::LASTNAME_FIRST) << std::endl;
			}

			file.close();
			MessageBoxW(hWnd, L"El reporte se ha guardado con éxito.",
				L"Información de Archivo", MB_ICONEXCLAMATION | MB_OK);
		}
		else {
			MessageBoxW(hWnd, L"No se ha podido guardar el reporte.",
				L"Error al guardar", MB_ICONERROR | MB_OK);
		}

		delete[] patArray;
	}
	else {
		MessageBoxW(hWnd, L"No hay información para guardar.",
			L"Error al guardar", MB_ICONERROR | MB_OK);
	}

}

void GetPatientRegisterInfo(HWND hWnd, Patient& patient) {

	//Get edit controls data
	{	//Gettig name
	std::wstring fname, sname, flname, slname;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_FNAME_EDIT), fname);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_SNAME_EDIT), sname);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_FLNAME_EDIT), flname);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_SLNAME_EDIT), slname);
	patient.SetName(fname, sname, flname, slname);
	}
	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_PHONENUM_EDIT), aux);	//Phone number
	patient.GetPhoneNumber().SetPhoneNumber(aux);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_H_EDIT), aux);			//Height
	patient.SetHeight((unsigned int)_wtoi(aux.c_str()));
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_W_EDIT), aux);			//Weight
	patient.SetWeigth((float)_wtof(aux.c_str()));
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_REFER_EDIT), aux);		//Reference
	patient.SetReference(aux);

	//Get blood type
	int selection = (int)SendDlgItemMessageW(hWnd, IDC_RP_BLOOD_COMBO, CB_GETCURSEL, NULL, NULL);
	patient.SetBloodType((BloodType)selection);

	//Get Gender
	selection = (int)SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_GETCURSEL, NULL, NULL);
	patient.SetGender((Gender)selection);

	//Get Birthdate
	SYSTEMTIME bdate;
	SendDlgItemMessageW(hWnd, IDC_RP_BIRTHD_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);
	patient.SetBirthdate(Date(bdate));

	//Get First Doctor
	patient.SetDoctor(GetIDFromCB(hWnd, IDC_RP_FIRSTDR_COMBO));

}

ValidationError ValidatePatientRegister(HWND hWnd) {

	ValidationError validationError;
	validationError.errorCode = ErrorCode::EC_NO_ERROR;
	validationError.errorSrc = NULL;

	{	//Validate edit controls
		HWND editControls[7];
		editControls[0]= GetDlgItem(hWnd, IDC_RP_FNAME_EDIT);		//First name edit
		editControls[1]= GetDlgItem(hWnd, IDC_RP_SNAME_EDIT);		//Second name edit
		editControls[2] = GetDlgItem(hWnd, IDC_RP_FLNAME_EDIT);		//First last name edit
		editControls[3] = GetDlgItem(hWnd, IDC_RP_SLNAME_EDIT);		//Second last name edit
		editControls[4] = GetDlgItem(hWnd, IDC_RP_PHONENUM_EDIT);	//Phone number edit
		editControls[5] = GetDlgItem(hWnd, IDC_RP_H_EDIT);			//Height edit
		editControls[6] = GetDlgItem(hWnd, IDC_RP_W_EDIT);			//Weight edit

		for (int i = 0; i < 7; i++) {
			if (GetWindowTextLengthW(editControls[i]) == 0) {
				validationError.errorCode = ErrorCode::EC_EMPTY;
				validationError.errorSrc = editControls[i];
				break;
			}
		}
		//Validate phone number length
		if (GetWindowTextLengthW(editControls[4]) < 10) {
			validationError.errorCode = ErrorCode::EC_INVALID_SIZE;
			validationError.errorSrc = editControls[4];
		}
		
	}

	{	//validate combo boxes
		HWND comboBoxes[2];
		comboBoxes[0] = GetDlgItem(hWnd, IDC_RP_GENDER_COMBO);
		comboBoxes[1] = GetDlgItem(hWnd, IDC_RP_BLOOD_COMBO);

		for (int i = 0; i < 2; i++) {
			if (SendMessageW(comboBoxes[i], CB_GETCURSEL, NULL, NULL) == CB_ERR) {
				validationError.errorCode = ErrorCode::EC_NO_SEL;
				validationError.errorSrc = comboBoxes[i];
				break;
			}
		}

	}

	//validate birthdate
	HWND bdateDTP;
	bdateDTP = GetDlgItem(hWnd, IDC_RP_BIRTHD_DTP);
	SYSTEMTIME stBdate;
	SendMessageW(bdateDTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&stBdate);
	DateTime dtBdate(stBdate);
	if (dtBdate > DateTime::RightNow()) {
		validationError.errorCode = ErrorCode::EC_INVALID_DATE;
		validationError.errorSrc = bdateDTP;
	}

	return validationError;

}

unsigned int AppointmentsPerPatient(unsigned int patKey, List<Appointment>& appList, std::vector<Appointment>* buffer) {

	unsigned int count = 0;

	appList.ForEach([&](Appointment& app) {
		if (app.GetPatient() == patKey) {
			count++;
			if (buffer)
				buffer->push_back(app);
		}
		});

	return count;

}

void ClearPatientRegister(HWND hWnd) {

	//Clear edit controls
	SetDlgItemTextW(hWnd, IDC_RP_FNAME_EDIT, L"");		//first name
	SetDlgItemTextW(hWnd, IDC_RP_SNAME_EDIT, L"");		//Second name
	SetDlgItemTextW(hWnd, IDC_RP_FLNAME_EDIT, L"");		//First last name
	SetDlgItemTextW(hWnd, IDC_RP_SLNAME_EDIT, L"");		//Second last name
	SetDlgItemTextW(hWnd, IDC_RP_PHONENUM_EDIT, L"");	//Phone number
	SetDlgItemTextW(hWnd, IDC_RP_H_EDIT, L"");			//Height
	SetDlgItemTextW(hWnd, IDC_RP_W_EDIT, L"");			//Weight
	SetDlgItemTextW(hWnd, IDC_RP_REFER_EDIT, L"");		//Reference

	//Restart combo boxes
	SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_SETCURSEL, -1, NULL);		//Gender
	SendDlgItemMessageW(hWnd, IDC_RP_BLOOD_COMBO, CB_SETCURSEL, -1, NULL);		//Blood type
	SendDlgItemMessageW(hWnd, IDC_RP_FIRSTDR_COMBO, CB_SETCURSEL, -1, NULL);	//First doctor

	//Restart date time pickers
	SYSTEMTIME now;
	GetLocalTime(&now);
	SendDlgItemMessageW(hWnd, IDC_RP_BIRTHD_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);	//Birthdate


}

#pragma endregion Reg_Patient_Window

#pragma region Reg_Spe_Window

void InitRegSpeControls(HWND hWnd, void(*initWithGlobals)(HWND)) {

	if (initWithGlobals)
		initWithGlobals(hWnd);

	SendDlgItemMessageW(hWnd, IDC_RS_SPENAME_EDIT, EM_SETLIMITTEXT, 20, NULL);	//SPECIALITY
	SendDlgItemMessageW(hWnd, IDC_RS_SPEDESC_EDIT, EM_SETLIMITTEXT, 200, NULL);	//SPE DESCRIPTION

}

void GetSpeRegisterInfo(HWND hWnd, Speciality& speciality) {

	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RS_SPENAME_EDIT), aux);
	speciality.SetName(aux);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RS_SPEDESC_EDIT), aux);
	speciality.SetDescription(aux);

}

ValidationError ValidateSpeRegister(HWND hWnd) {

	ValidationError validationError = { ErrorCode::EC_NO_ERROR, NULL };

	HWND speNameEdit, speDescEdit;
	speNameEdit = GetDlgItem(hWnd, IDC_RS_SPENAME_EDIT);
	speDescEdit = GetDlgItem(hWnd, IDC_RS_SPEDESC_EDIT);

	if (GetWindowTextLengthW(speNameEdit) == 0) {
		validationError.errorCode = ErrorCode::EC_EMPTY;
		validationError.errorSrc = speNameEdit;
	}
	else if (GetWindowTextLengthW(speDescEdit) == 0) {
		validationError.errorCode = ErrorCode::EC_EMPTY;
		validationError.errorSrc = speDescEdit;
	}
	
	return validationError;

}

void ClearSpeRegister(HWND hWnd) {

	//clear edit controls
	SetDlgItemTextW(hWnd, IDC_RS_SPENAME_EDIT, L"");
	SetDlgItemTextW(hWnd, IDC_RS_SPEDESC_EDIT, L"");

}

void ViewSelectedSpeciality(HWND hWnd, List<Speciality>& speList, DLGPROC viewerProc) {

	unsigned int key = GetKeyFromLB(hWnd, IDC_SPE_LIST);
	Speciality& speRef = speList.Search<unsigned int>(key, [](const Speciality& spe, const unsigned int& _key) {
		return spe.Key() == _key;
		});

	CreateDialogParamW(NULL, MAKEINTRESOURCEW(IDD_VIEW_SPE), hWnd, viewerProc, (LPARAM)&speRef);

}

void DeleteSelectedSpeciality(HWND hWnd, List<Speciality>& speList, BinarySearchTree<Doctor>& drBST) {

	unsigned int key = GetKeyFromLB(hWnd, IDC_SPE_LIST);
	int deleted = -1, i = 0;

	if (DoctorsPerSpeciality(key, drBST) == 0) {

		//Find the element with that key
		speList.ForEach([&](Speciality& spe) {
			if (spe.Key() == key)
				deleted = i;	//if found: save the current index
			i++;
			});

		if (deleted > -1)
			Speciality tmp = speList.Delete((unsigned int)deleted);

	}
	else {
		//Speciality cannot be deleted because there are doctors registered with that speciality
		MessageBoxW(hWnd, L"Uno o más doctores registrados con esta especialidad.", L"No se ha podido eliminar",
			MB_ICONERROR | MB_OK);
	}

}

unsigned int DoctorsPerSpeciality(unsigned int key, BinarySearchTree<Doctor>& drBST, std::vector<Doctor>* buffer) {

	unsigned int count = 0;

	//Count the doctors with that speciality
	drBST.ExecutePreorder([&](Doctor& dr) {
		if (dr.GetSpeciality() == key) {
			count++;
			if (buffer)	//If the buffer has been specified
				buffer->push_back(dr);
		}
		});

	return count;

}

void ShowSpecialityDoctors(HWND hWnd, BinarySearchTree<Doctor>& drBST) {

	std::vector<Doctor> buffer;
	unsigned int speKey = GetKeyFromLB(hWnd, IDC_SPE_LIST);
	if (speKey != 0) {
		DoctorsPerSpeciality(speKey, drBST, &buffer);
		for (Doctor& doc : buffer) {
			std::wstring aux = doc.ProfessionalID() + L" "
				+ doc.GetName(Names::FIRST_NAME) + L" "
				+ doc.GetName(Names::FIRST_LASTNAME);
			SendDlgItemMessageW(hWnd, IDC_DR_LIST, LB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		}
	}

}

#pragma endregion Reg_Spe_Window

#pragma region File_Management

void CreateAppDirectory() {

	std::wstring mainAppDirectory = DocumentsDirectory() + MAIN_FOLDER;

	//Create Directory
	int res = _wmkdir(mainAppDirectory.c_str());
	res = _wmkdir((mainAppDirectory + DATA_FOLDER).c_str());
	res = _wmkdir((mainAppDirectory + IMG_FOLDER).c_str());

}

void LoadFiles(List<Appointment>& appList,
	List<Patient>& patList,
	List<Speciality>& speList,
	List<MedOffice>& medOffList,
	BinarySearchTree<Doctor>& drBST) {

	std::wstring dataDirectory = DocumentsDirectory() + MAIN_FOLDER + DATA_FOLDER;

	appList.ReadFromFile(dataDirectory + L"\\appoint.data");
	patList.ReadFromFile(dataDirectory + L"\\patient.data");
	speList.ReadFromFile(dataDirectory + L"\\spe.data");
	medOffList.ReadFromFile(dataDirectory + L"\\medoff.data");
	drBST.ReadFromFile(dataDirectory + L"\\doc.data");

}

void SaveFiles(List<Appointment>& appList,
	List<Patient>& patList,
	List<Speciality>& speList,
	List<MedOffice>& medOffList,
	BinarySearchTree<Doctor>& drBST) {

	std::wstring mainAppDirectory = DocumentsDirectory() + MAIN_FOLDER + DATA_FOLDER;

	appList.WriteToFile(mainAppDirectory + L"\\appoint.data");
	patList.WriteToFile(mainAppDirectory + L"\\patient.data");
	speList.WriteToFile(mainAppDirectory + L"\\spe.data");
	medOffList.WriteToFile(mainAppDirectory + L"\\medoff.data");
	drBST.WriteToFile(mainAppDirectory + L"\\doc.data");

}

#pragma endregion File_Management

#pragma region View_Appointment_Window

void ShowAppointmentInfo(HWND hWnd, Appointment& app, Patient& patient, Doctor& doctor, Speciality& spe) {

	//Write app info
	SetDlgItemTextW(hWnd, IDC_DESC_EDIT, app.GetDescription().c_str());
	SetDlgItemTextW(hWnd, IDC_DATETIME_EDIT, app.GetDateTime().DateTimeWstring().c_str());
	SetDlgItemTextW(hWnd, IDC_MEDOFFNUM_EDIT, std::to_wstring(app.GetMedOfficeKey()).c_str());
	SetDlgItemTextW(hWnd, IDC_PHONENUM_EDIT, app.GetPhoneNumber().PhoneNumberString(true).c_str());

	//Write patient info
	SetDlgItemTextW(hWnd, IDC_PATIENT_NAME_EDIT, patient.GetName(Names::FULL_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_GENDER_EDIT, patient.GetGenderString().c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_AGE_EDIT, std::to_wstring(patient.Age()).c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_H_EDIT, std::to_wstring(patient.Height()).c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_W_EDIT, std::to_wstring(patient.Weight()).c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_BLOOD_EDIT, patient.GetBloodTypeString().c_str());
	SetDlgItemTextW(hWnd, IDC_PATIENT_BIRTHD_EDIT, patient.Birthdate().DateString().c_str());

	//Write doctor info
	SetDlgItemTextW(hWnd, IDC_DR_NAME_EDIT, doctor.GetName(Names::FULL_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_DR_PROID_EDIT, doctor.ProfessionalID().c_str());
	SetPictureControlImg(doctor.GetImagePath().c_str(), 300, 300, hWnd, IDC_DR_IMG);

	//Write speciality info
	SetDlgItemTextW(hWnd, IDC_DR_SPE_EDIT, spe.Name().c_str());

	//Mark checkboxes
	SendDlgItemMessageW(hWnd, IDC_RA_DOUBLEAPP_CHECK, BM_SETCHECK, (WPARAM)app.IsDouble(), NULL);
	SendDlgItemMessageW(hWnd, IDC_ATT_CHECK, BM_SETCHECK, (WPARAM)app.IsAttended(), NULL);
	SendDlgItemMessageW(hWnd, IDC_CANCEL_CHECK, BM_SETCHECK, (WPARAM)app.IsCanceled(), NULL);

}

#pragma endregion View_Appointment_Window

#pragma region View_Patient_Window

void ShowPatientInfo(HWND hWnd, Patient& pat) {

	//Write patient info
	SetDlgItemTextW(hWnd, IDC_RP_FNAME_EDIT, pat.GetName(Names::FIRST_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_SNAME_EDIT, pat.GetName(Names::SECOND_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_FLNAME_EDIT, pat.GetName(Names::FIRST_LASTNAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_SLNAME_EDIT, pat.GetName(Names::SECOND_LASTNAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_PHONENUM_EDIT, pat.GetPhoneNumber().PhoneNumberString(false).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_H_EDIT, std::to_wstring(pat.Height()).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_W_EDIT, std::to_wstring(pat.Weight()).c_str());
	SetDlgItemTextW(hWnd, IDC_RP_REFER_EDIT, pat.Reference().c_str());

	//Set combo boxes
	SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_SETCURSEL, (WPARAM)pat.GetGender(), NULL);
	SendDlgItemMessageW(hWnd, IDC_RP_BLOOD_COMBO, CB_SETCURSEL, (WPARAM)pat.GetBloodType(), NULL);

	//Set Birthdate
	Date bd = pat.Birthdate();
	SendDlgItemMessageW(hWnd, IDC_RP_BIRTHD_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&bd.GetSystemTimeStruct());

}

void EditPatient(HWND hWnd, Patient& patient) {

	//Get edit controls data
	{	//Gettig name
		std::wstring fname, sname, flname, slname;
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_FNAME_EDIT), fname);
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_SNAME_EDIT), sname);
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_FLNAME_EDIT), flname);
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_SLNAME_EDIT), slname);
		patient.SetName(fname, sname, flname, slname);
	}
	std::wstring aux;
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_PHONENUM_EDIT), aux);	//Phone number
	patient.GetPhoneNumber().SetPhoneNumber(aux);
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_H_EDIT), aux);			//Height
	patient.SetHeight((unsigned int)_wtoi(aux.c_str()));
	GetWindowTextWstring(GetDlgItem(hWnd, IDC_RP_W_EDIT), aux);			//Weight
	patient.SetWeigth((float)_wtof(aux.c_str()));

	//Get blood type
	int selection = (int)SendDlgItemMessageW(hWnd, IDC_RP_BLOOD_COMBO, CB_GETCURSEL, NULL, NULL);
	patient.SetBloodType((BloodType)selection);

	//Get Gender
	selection = (int)SendDlgItemMessageW(hWnd, IDC_RP_GENDER_COMBO, CB_GETCURSEL, NULL, NULL);
	patient.SetGender((Gender)selection);

	//Get Birthdate
	SYSTEMTIME bdate;
	SendDlgItemMessageW(hWnd, IDC_RP_BIRTHD_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);
	patient.SetBirthdate(Date(bdate));

}

void EnablePatientEdit(HWND hWnd, bool enable) {

	HWND controls[13];
	controls[0] = GetDlgItem(hWnd, IDC_RP_FNAME_EDIT);
	controls[1] = GetDlgItem(hWnd, IDC_RP_SNAME_EDIT);
	controls[2] = GetDlgItem(hWnd, IDC_RP_FLNAME_EDIT);
	controls[3] = GetDlgItem(hWnd, IDC_RP_SLNAME_EDIT);
	controls[4] = GetDlgItem(hWnd, IDC_RP_PHONENUM_EDIT);
	controls[5] = GetDlgItem(hWnd, IDC_RP_BIRTHD_DTP);
	controls[6] = GetDlgItem(hWnd, IDC_RP_H_EDIT);
	controls[7] = GetDlgItem(hWnd, IDC_RP_W_EDIT);
	controls[8] = GetDlgItem(hWnd, IDC_RP_GENDER_COMBO);
	controls[9] = GetDlgItem(hWnd, IDC_RP_BLOOD_COMBO);
	controls[10] = GetDlgItem(hWnd, IDC_RP_REFER_EDIT);
	controls[11] = GetDlgItem(hWnd, IDC_SAVE_EDIT_PAT_CMD);
	controls[12] = GetDlgItem(hWnd, IDC_CANCEL_EDIT_PAT_CMD);

	for (int i = 0; i < 13; i++)
		EnableWindow(controls[i], enable);

	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PATIENT_CMD), !enable);

}

#pragma endregion View_Patient_Window

#pragma region View_Doctor_Window

void ShowDoctorInfo(HWND hWnd, Doctor& dr) {

	//edit controls
	SetDlgItemTextW(hWnd, IDC_RM_DR_FNAME_EDIT, dr.GetName(Names::FIRST_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RM_DR_SNAME_EDIT, dr.GetName(Names::SECOND_NAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RM_DR_FLNAME_EDIT, dr.GetName(Names::FIRST_LASTNAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RM_DR_SLNAME_EDIT, dr.GetName(Names::SECOND_LASTNAME).c_str());
	SetDlgItemTextW(hWnd, IDC_RM_PHONENUM_EDIT, dr.GetPhoneNumber().PhoneNumberString(false).c_str());
	SetDlgItemTextW(hWnd, IDC_RM_PROID_EDIT, dr.ProfessionalID().c_str());
	SetDlgItemTextW(hWnd, IDC_MO_EDIT, std::to_wstring(dr.GetMedOfficeNum()).c_str());

	//combo boxes
	SendDlgItemMessageW(hWnd, IDC_RM_GENDER_COMBO, CB_SETCURSEL, (WPARAM)dr.GetGender(), NULL);
	
	//date time pickers
	TimePeriod mainSchedule;
	if (!dr.GetSchedule().GetReservedTime().empty())
		mainSchedule = dr.GetSchedule().GetReservedTime()[0];

	SendDlgItemMessageW(hWnd, IDC_RM_BIRTHD_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&dr.Birthdate().GetSystemTimeStruct());
	SendDlgItemMessageW(hWnd, IDC_RM_SCHE_FROM_DTP, DTM_SETSYSTEMTIME, GDT_VALID,(LPARAM)&mainSchedule.Begin().GetSystemTimeStruct());
	SendDlgItemMessageW(hWnd, IDC_RM_SCHE_TO_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&mainSchedule.End().GetSystemTimeStruct());

	//checkers
	HWND checkBoxes[5];
	checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
	checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
	checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
	checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
	checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday
	const bool* workDays = dr.GetSchedule().GetWorkDays();

	for (int i = 0; i < 5; i++) {
		SendMessageW(checkBoxes[i], BM_SETCHECK, (WPARAM)workDays[i + 1], NULL);
	}

	//image
	SetPictureControlImg(dr.GetImagePath().c_str(), 170, 170, hWnd, IDC_DR_IMG);

}

void EditDoctor(HWND hWnd, Doctor& doctor) {

	{	//Get edit control and static text info
		std::wstring fname, sname, flname, slname;
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT), fname);	//First name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT), sname);	//Second name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT), flname);	//First last name
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT), slname);	//Second last name
		doctor.SetName(fname, sname, flname, slname);

		std::wstring aux;
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT), aux);		//Phone number
		doctor.GetPhoneNumber().SetPhoneNumber(aux);
		GetWindowTextWstring(GetDlgItem(hWnd, IDC_RM_IMGPATH), aux);			//Img file path
		doctor.SetImagePath(aux);
	}

	{	//Get date time pickers info
		SYSTEMTIME bdate, from, to;
		SendDlgItemMessageW(hWnd, IDC_RM_BIRTHD_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);		//Birth date
		doctor.SetBirthdate(Date(bdate));
		SendDlgItemMessageW(hWnd, IDC_RM_SCHE_FROM_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&from);	//Schedule
		SendDlgItemMessageW(hWnd, IDC_RM_SCHE_TO_DTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&to);
		doctor.GetSchedule().Free();
		doctor.GetSchedule().Reserve(DateTime(from), DateTime(to));
	}

	{	//Get check boxes state
		HWND checkBoxes[5];
		checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
		checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
		checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
		checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
		checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday

		for (int i = 0; i < 5; i++) {
			bool isWorkDay = (bool)SendMessageW(checkBoxes[i], BM_GETCHECK, NULL, NULL);
			if (isWorkDay)
				doctor.GetSchedule().SetWorkDay((unsigned int)(i + 1), isWorkDay);

		}
	}

	//Get gender
	HWND genderCB = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);	//Gender
	Gender gender = (Gender)SendMessageW(genderCB, CB_GETCURSEL, NULL, NULL);
	doctor.SetGender(gender);

}

ValidationError ValidateDoctorEdition(HWND hWnd) {
	
	ValidationError validationError = { ErrorCode::EC_NO_ERROR, NULL };

	{	//Validate edit Controls
		HWND editControls[5];
		editControls[0] = GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT);	//First name
		editControls[1] = GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT);	//Second name
		editControls[2] = GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT);	//First last name
		editControls[3] = GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT);	//Second last name
		editControls[4] = GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT);	//Phone number

		for (int i = 0; i < 5; i++) {
			if (GetWindowTextLengthW(editControls[i]) == 0) {
				validationError.errorCode = ErrorCode::EC_EMPTY;
				validationError.errorSrc = editControls[i];
				break;
			}
		}

		//Validate phone number length
		if (GetWindowTextLengthW(editControls[4]) < 10) {
			validationError.errorCode = ErrorCode::EC_INVALID_SIZE;
			validationError.errorSrc = editControls[4];
		}
	}

	{	//Validate combo boxes
		HWND genderCB = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);	//Gender
		if (SendMessageW(genderCB, CB_GETCURSEL, NULL, NULL) == CB_ERR) {
			validationError.errorCode = ErrorCode::EC_NO_SEL;
			validationError.errorSrc = genderCB;
		}
	}

	{	//Validate birthdate
		SYSTEMTIME bdate;
		HWND bdateDTP = GetDlgItem(hWnd, IDC_RM_BIRTHD_DTP);
		SendMessageW(bdateDTP, DTM_GETSYSTEMTIME, NULL, (LPARAM)&bdate);
		DateTime bdateDT(bdate);
		if (bdateDT > DateTime::RightNow()) {
			validationError.errorCode = ErrorCode::EC_INVALID_DATE;
			validationError.errorSrc = bdateDTP;
		}
	}

	{	//validate check boxes (at least one checked)
		HWND checkBoxes[5];
		checkBoxes[0] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);	//Monday
		checkBoxes[1] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);	//Tuesday
		checkBoxes[2] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);	//Wednesday
		checkBoxes[3] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);	//Thursday
		checkBoxes[4] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);	//Friday

		bool allUnchecked = true;

		for (int i = 0; i < 5; i++) {
			if (SendMessageW(checkBoxes[i], BM_GETCHECK, NULL, NULL) == BST_CHECKED) {
				allUnchecked = false;
				break;
			}
		}

		if (allUnchecked) {
			validationError.errorCode = ErrorCode::EC_NO_SEL;
			validationError.errorSrc = checkBoxes[0];
		}

	}

	return validationError;

}

void EnableDoctorEdit(HWND hWnd, bool enable) {

	HWND controls[17];
	controls[0] = GetDlgItem(hWnd, IDC_RM_DR_FNAME_EDIT);
	controls[1] = GetDlgItem(hWnd, IDC_RM_DR_SNAME_EDIT);
	controls[2] = GetDlgItem(hWnd, IDC_RM_DR_FLNAME_EDIT);
	controls[3] = GetDlgItem(hWnd, IDC_RM_DR_SLNAME_EDIT);
	controls[4] = GetDlgItem(hWnd, IDC_RM_PHONENUM_EDIT);
	controls[5] = GetDlgItem(hWnd, IDC_RM_BIRTHD_DTP);
	controls[6] = GetDlgItem(hWnd, IDC_RM_SCHE_FROM_DTP);
	controls[7] = GetDlgItem(hWnd, IDC_RM_GENDER_COMBO);
	controls[8] = GetDlgItem(hWnd, IDC_RM_SCHE_TO_DTP);
	controls[9] = GetDlgItem(hWnd, IDC_VM_LOADIMG_CMD);
	controls[10] = GetDlgItem(hWnd, IDC_RM_MON_CHECK);
	controls[11] = GetDlgItem(hWnd, IDC_RM_TUE_CHECK);
	controls[12] = GetDlgItem(hWnd, IDC_RM_WED_CHECK);
	controls[13] = GetDlgItem(hWnd, IDC_RM_THU_CHECK);
	controls[14] = GetDlgItem(hWnd, IDC_RM_FRI_CHECK);
	controls[15] = GetDlgItem(hWnd, IDC_SAVE_EDIT_DR_CMD);
	controls[16] = GetDlgItem(hWnd, IDC_CANCEL_EDIT_DR_CMD);

	for (int i = 0; i < 17; i++)
		EnableWindow(controls[i], enable);

	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_DR_CMD), !enable);

}

#pragma endregion View_Doctor_Window

#pragma region View_Speciality_Window

void ShowSpecialityInfo(HWND hWnd, Speciality& spe) {

	SetDlgItemTextW(hWnd, IDC_RS_SPENAME_EDIT, spe.Name().c_str());
	SetDlgItemTextW(hWnd, IDC_RS_SPEDESC_EDIT, spe.Description().c_str());

}

void EnableSpecialityEdit(HWND hWnd, bool enable) {

	HWND controls[4];
	controls[0] = GetDlgItem(hWnd, IDC_RS_SPENAME_EDIT);
	controls[1] = GetDlgItem(hWnd, IDC_RS_SPEDESC_EDIT);
	controls[2] = GetDlgItem(hWnd, IDC_SAVE_EDIT_SPE_CMD);
	controls[3] = GetDlgItem(hWnd, IDC_CANCEL_EDIT_SPE_CMD);

	for (int i = 0; i < 4; i++)
		EnableWindow(controls[i], enable);

	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_SPE_CMD), !enable);
}

#pragma endregion View_Speciality_Window

#pragma region Control_Subclasses

LRESULT CALLBACK AlphaOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData) {

	switch (msg) {

	case WM_CHAR:

		//Validate to only accept alphabetic characters and some escape characters
		if (!iswalpha(wParam)) {
			if (wParam != '\b' && wParam != ' ' && wParam != '\t') {
				return 0;
			}
		}

		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, AlphaOnlyEditCtrlWndProc, ID_ALPHA_EDIT_CONTROL);
		break;
	}

	return DefSubclassProc(hWnd, msg, wParam, lParam);

}

LRESULT CALLBACK FloatOnlyEditCtrlWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubClass, DWORD_PTR dwRefData) {

	std::wstring buffer;
	bool isPoint = false;

	static int count = 0;

	switch (msg) {

	case WM_CHAR:

		GetWindowTextWstring(hWnd, buffer);

		if (buffer.find_first_of(L'.') != std::wstring::npos)	//Detect if the user enters a point
			isPoint = true;

		//Reset the value of count when there is no point
		if (!isPoint) {
			count = 0;
		}

		//Decrease the count when the user erases a post point digit
		if (count > 0 && wParam == '\b') {
			count--;
		}

		if (count < 2) {	//Allow only two digits after the point
			//Validate only digits, point and backspace
			if (!iswdigit(wParam)) {
				if (wParam != '.' && wParam != '\b') {
					return 0;
				}
			}
			else {
				//If the string already has a point
				if (isPoint && wParam == '.') {
					return 0;
				}
				else if (isPoint && wParam != '\b') {
					count++;	//It counts the total digits after the point
				}
			}
		}
		else {
			return 0;
		}

		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, FloatOnlyEditCtrlWndProc, ID_FLOAT_EDIT_CONTROL);
		break;
	}

	return DefSubclassProc(hWnd, msg, wParam, lParam);

}

#pragma endregion Control_Subclasses

#pragma endregion Window_Functionality