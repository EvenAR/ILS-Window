// MiniPAR.h : main header file for the MiniPAR DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "MiniParPlugIn.h"

// CMiniPARApp
// See MiniPAR.cpp for the implementation of this class
//

class CMiniPARApp : public CWinApp
{
public:
	CMiniPARApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};