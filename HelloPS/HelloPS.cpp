#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloPSApp, CWinApp)
END_MESSAGE_MAP()

CHelloPSApp theApp;

CHelloPSApp::CHelloPSApp()
{
}

BOOL CHelloPSApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	m_mutex = CreateMutex(NULL, FALSE, TEXT("HelloPS"));
	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		CHelloPSDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

int CHelloPSApp::ExitInstance()
{
	if (m_mutex != NULL)
	{
		CloseHandle(m_mutex);
	}

	return CWinApp::ExitInstance();
}
