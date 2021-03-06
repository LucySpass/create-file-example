// beg-liba.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "beg-liba.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

struct header {
	int id;
	int size;
};

CString sms;
HANDLE hFile;
HANDLE hMap;
LPBYTE hByte;
HANDLE conf_Dll = CreateEvent(NULL, FALSE, FALSE, "conf_Dll");

extern "C" {
	_declspec(dllexport) void _stdcall form_send(int _id, int _size, char *_sms) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		header h;
		h.id = _id;
		h.size = _size + 1;
		sms = CString(_sms);

		hFile = CreateFile("beg-file.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, 0);
		hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(h) + h.size, "Map");
		hByte = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(h) + h.size);

		memcpy(hByte, &h, sizeof(h));
		memcpy(hByte + sizeof(h), sms.GetString(), h.size);

		CloseHandle(hFile);
		CloseHandle(hMap);

		SetEvent(conf_Dll);
	}
	_declspec(dllexport) void console_get(int &_id, char*&_sms) {
		WaitForSingleObject(conf_Dll, INFINITE);
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		header h;

		hFile = CreateFile("beg-file.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, 0);
		hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(h), "Map");
		hByte = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(h));

		memcpy(&h, hByte, sizeof(h));

		CloseHandle(hMap);
		CloseHandle(hFile);

		hFile = CreateFile("beg-file.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, 0);
		hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(h) + h.size, "Map");
		hByte = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(h) + h.size);

		_sms = new char[h.size];
		memcpy(_sms, hByte + sizeof(h), h.size);
		_id = h.id;

		CloseHandle(hFile);
		CloseHandle(hMap);
	}
	_declspec(dllexport) void delete_message (char*_sms) {
		delete[] _sms;
	}
}

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CbeglibaApp

BEGIN_MESSAGE_MAP(CbeglibaApp, CWinApp)
END_MESSAGE_MAP()


// CbeglibaApp construction

CbeglibaApp::CbeglibaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CbeglibaApp object

CbeglibaApp theApp;


// CbeglibaApp initialization

BOOL CbeglibaApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
