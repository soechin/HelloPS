#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
END_MESSAGE_MAP()

CHelloPSDlg::CHelloPSDlg() : CDialogEx(IDD_HELLOPS_DIALOG)
{
}

void CHelloPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CHelloPSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_icon, TRUE);
	SetIcon(m_icon, FALSE);

	return TRUE;
}
