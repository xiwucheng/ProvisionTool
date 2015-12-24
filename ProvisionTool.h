// ProvisionTool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CProvisionToolApp:
// See ProvisionTool.cpp for the implementation of this class
//

class CProvisionToolApp : public CWinApp
{
public:
	CProvisionToolApp();

// Overrides
	public:
	virtual BOOL InitInstance();
private:
	HANDLE 	m_hMutex;

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
};

extern CProvisionToolApp theApp;