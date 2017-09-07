#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CHelloPSApp : public CWinApp
{
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_mutex;
public:
	CHelloPSApp();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

extern CHelloPSApp theApp;
