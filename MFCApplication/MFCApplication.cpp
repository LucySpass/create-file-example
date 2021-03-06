// MFCApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MFCApplication.h"
#include "../beg-liba/beg-liba/beg-liba.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;
typedef void(*sndmsg)(int &_id, char *&sms);
typedef void(*dltmsg)(char *_sms);

using namespace std;

HANDLE hConfirm = CreateEvent(NULL, FALSE, FALSE, "EventConfirm");
HANDLE hLocalThread = CreateEvent(NULL, FALSE, FALSE, "EventLocalThread");
HANDLE hStopLastThread = CreateEvent(NULL, TRUE, FALSE, "EventStopIt");
HANDLE hSendTxt = CreateEvent(NULL, TRUE, FALSE, "EventSendTxt");

HANDLE hEvents[4];
HANDLE mMutex;

vector<HANDLE> ThreadsContainer;

int sendThreadId, temp;
char *txt;

DWORD WINAPI ThreadStart(LPVOID lpParameter) 
{
	HANDLE hEvent[2];
	hEvent[0] = hStopLastThread;
	hEvent[1] = hSendTxt;
	const int index = ThreadsContainer.size() - 1;
	WaitForSingleObject(mMutex, INFINITE);
	cout << "Starting " << index << " thread..." << endl;
	ReleaseMutex(mMutex);

	SetEvent(hConfirm);

	while (1) {
		DWORD evNum = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
		switch (evNum) {
			case 0: // thread stop
			{
				if (index == (ThreadsContainer.size() - 1)) {
	
					ResetEvent(hStopLastThread);
					CloseHandle(lpParameter);

					WaitForSingleObject(mMutex, INFINITE);
					cout << "Thread " << index << " stopped!" << endl;
					ReleaseMutex(mMutex);

					SetEvent(hLocalThread);

					return 0;
				}
				else if (ThreadsContainer.size() < 1) {
					cout << "Container is empty!" << endl;
				}
				else {
					WaitForSingleObject(mMutex, INFINITE);
					cout << "Not the last one! Checked index: " << index << endl;
					ReleaseMutex(mMutex);
				}
				SetEvent(hConfirm);
				break;
			}
			case 1: // message send
			{
				if (sendThreadId == -1 || index == sendThreadId) {
					
					ResetEvent(hSendTxt);
					CString name;
					name.Format("%d.txt", index);
					ofstream file(name, ios::binary);
					file << txt << endl;
					file.close();
					if (temp == ThreadsContainer.size()-1) {
						SetEvent(hLocalThread);
					}
					else temp++;

					SetEvent(hConfirm);
				}
				break;
			}
		}		
	}

	return 0;
}

void start()
{
	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, "EventQuit");
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "EventStartThread");
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "EventStop");
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "EventSend");

	HINSTANCE liba = LoadLibrary("beg-liba.dll");
	if (liba == NULL) { cout << "Failed to load library liba.dll" << endl; return; }
	sndmsg console_get = (sndmsg)GetProcAddress(liba, "console_get");
	dltmsg delete_message = (dltmsg)GetProcAddress(liba, "delete_message");
	
	mMutex = CreateMutex(NULL, FALSE, NULL);

	while (1)
	{
		DWORD dwResult = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE);
		switch (dwResult)
		{
			case 0: // QUIT EVENT
			{
				std::cout << "App quit" << endl;
				CloseHandle(hEvents[0]);
				CloseHandle(hEvents[1]);
				CloseHandle(hEvents[2]);				
				CloseHandle(hLocalThread);
				CloseHandle(hEvents[3]);
				CloseHandle(hSendTxt);
				ThreadsContainer.clear();

				SetEvent(hConfirm);
				CloseHandle(hConfirm);

				return;
			}
			case 1: // START THREAD EVENT
			{ 
				HANDLE hThread;				
				ThreadsContainer.push_back(hThread);
				hThread = CreateThread(NULL, 0, ThreadStart, (LPVOID)hThread, 0, NULL);
	
				break;
			}
			case 2: // STOP EVENT	
			{ 	
				SetEvent(hStopLastThread);
				WaitForSingleObject(hLocalThread, INFINITE);

				ThreadsContainer.erase(ThreadsContainer.end() - 1);
				SetEvent(hConfirm);			

				break;
			}
			case 3: // SEND EVENT
			{ 
				int configId = 0;
				temp = 0;
				console_get(configId, txt);
				
				if (configId == 0) {
					cout << "From main thread: " << txt << endl;
				}
				else if (configId == 1) {
					sendThreadId = -1;					
					cout << "Message sent to all threads: " << txt << endl;
					SetEvent(hSendTxt);
				}
				else {
					sendThreadId = configId - 2;
					SetEvent(hSendTxt);
				}
				SetEvent(hConfirm);

				WaitForSingleObject(hLocalThread, INFINITE);

				delete_message(txt);
				cout << "Message deleted!" << endl;

				break;
			}
		}
	}
}

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			start();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
