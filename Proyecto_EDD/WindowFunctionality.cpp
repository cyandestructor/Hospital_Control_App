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

std::wstring DocumentsDirectory() {

	wchar_t* buffer = nullptr;
	std::wstring temp;

	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &buffer) == S_OK)
		temp = buffer;

	CoTaskMemFree(buffer);
	return temp;

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

}

#pragma endregion Query_App_Window

#pragma region Reg_App_Window

void InitRegAppControls(HWND hWnd, void(*initWithGlobals)(HWND)) {
	
	if (initWithGlobals)
		initWithGlobals(hWnd);

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

	DateTime date(appDate), time(appTime);
	if (date < DateTime::RightNow()) {
		errorCode = ErrorCode::EC_INVALID_DATE;
		errorSrc = dateDTP;
	}
	else if (time < DateTime::RightNow()) {
		errorCode = ErrorCode::EC_INVALID_DATE;
		errorSrc = timeDTP;
	}

	ValidationError validationError = { errorCode, errorSrc };

	return validationError;

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

	//restart date time pickers
	SYSTEMTIME now;
	GetLocalTime(&now);
	SendDlgItemMessageW(hWnd, IDC_RA_APPDATE_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);
	SendDlgItemMessageW(hWnd, IDC_RA_APPTIME_DTP, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&now);

	//restart check box
	SendDlgItemMessageW(hWnd, IDC_RA_DOUBLEAPP_CHECK, BM_SETCHECK, BST_UNCHECKED, NULL);

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

	}

}

void SelectDoctor(HWND hWnd, int comboBoxID, BinarySearchTree<Doctor>& drList) {

	unsigned long id = GetIDFromCB(hWnd, IDC_RA_SELDR_COMBO);
	Doctor temp(id);

	if (drList.BinarySearch(temp, temp)) {
		SetDlgItemTextW(hWnd, IDC_DR_NAME_EDIT, temp.GetName(Names::FULL_NAME).c_str());
		SetDlgItemTextW(hWnd, IDC_DR_PROID_EDIT, temp.ProfessionalID().c_str());
		SetPictureControlImg(temp.GetImagePath().c_str(), 300, 300, hWnd, IDC_DR_IMG);
	}

}

void FilterDoctorsWithSpeciality(HWND hWnd, BinarySearchTree<Doctor>& drBST) {

	List<Doctor> tempList;

	//Get the speciality key
	unsigned int key = GetKeyFromCB(hWnd, IDC_RA_SELSPE_COMBO);
	//Get the doctors with that speciality
	drBST.ExecutePreorder([&](Doctor& dr) {
		if (dr.GetSpeciality() == key)
			tempList.Push(dr);	//Save in the list
		});
	//Restart the combo box
	SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_RESETCONTENT, NULL, NULL);
	//Add the doctors with that speciality
	tempList.ForEach([&](Doctor& doc) {
		std::wstring aux = doc.ProfessionalID() + L" "
			+ doc.GetName(Names::FIRST_NAME) + L" "
			+ doc.GetName(Names::FIRST_LASTNAME);
		SendDlgItemMessageW(hWnd, IDC_RA_SELDR_COMBO, CB_ADDSTRING, NULL, (LPARAM)aux.c_str());
		});

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
		doctor.SetImagePath(aux);
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

void GenDoctorReport(HWND hWnd, BinarySearchTree<Doctor>& drBST, const std::wstring& filePath) {

	std::ofstream file;
	//Create doctor list
	List<Doctor> drList;
	drBST.ExecutePostorder([&](Doctor& doc) {
		drList.Push(doc);
		});
	//Quicksort the list
	drList.QuickSort();
	//Write the data in the file
	file.open(filePath, ios::out | ios::trunc);

	if (file.is_open()) {
		drList.ForEach([&](Doctor& dr) {
			file << dr.ProfessionalID().c_str() << L" "
				<< dr.GetName(Names::FULL_NAME).c_str() << L" "
				<< dr.GetPhoneNumber().PhoneNumberString(false).c_str() << std::endl;
			});

		file.close();
		MessageBoxW(hWnd, L"Información de Archivo",
			L"El reporte se ha guardado con éxito.", MB_ICONEXCLAMATION | MB_OK);
	}
	else {
		MessageBoxW(hWnd, L"Error al guardar",
			L"No se ha podido guardar el reporte.", MB_ICONERROR | MB_OK);
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

void GenPatientReport(HWND hWnd, List<Patient>& patList, const std::wstring& filepath) {

	Patient* patArray = patList.GetArray();
	size_t size = patList.Size();

	if (patArray) {
		ofstream file;
		file.open(filepath, ios::out | ios::trunc);

		if (file.is_open()) {

			Heap<Patient>::HeapSort(patArray, size, HeapType::MIN_H);

			for (size_t i = 0; i < size; i++) {
				file << patArray[i].GetName(Names::LASTNAME_FIRST).c_str() << std::endl;
			}

			file.close();
		}

		delete patArray;
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

#pragma endregion Reg_Spe_Window

#pragma region File_Management

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

	std::wstring mainAppDirectory = DocumentsDirectory() + MAIN_FOLDER;

	//Create Directory
	int res = _wmkdir(mainAppDirectory.c_str());
	mainAppDirectory += DATA_FOLDER;
	res = _wmkdir(mainAppDirectory.c_str());

	appList.WriteToFile(mainAppDirectory + L"\\appoint.data");
	patList.WriteToFile(mainAppDirectory + L"\\patient.data");
	speList.WriteToFile(mainAppDirectory + L"\\spe.data");
	medOffList.WriteToFile(mainAppDirectory + L"\\medoff.data");
	drBST.WriteToFile(mainAppDirectory + L"\\doc.data");

}

#pragma endregion File_Management

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