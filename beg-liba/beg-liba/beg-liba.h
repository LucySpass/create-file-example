// beg-liba.h : main header file for the beg-liba DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CbeglibaApp
// See beg-liba.cpp for the implementation of this class
//

class CbeglibaApp : public CWinApp
{
public:
	CbeglibaApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
