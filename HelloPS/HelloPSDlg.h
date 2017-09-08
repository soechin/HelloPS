#pragma once

#include "sqlite.hpp"
#include "afxwin.h"

class CHelloPSDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()
public:
	CHelloPSDlg();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OpenDatabase();
	virtual void CloseDatabase();
	afx_msg void OnBnClickedManageBtn();
	afx_msg void OnUpdateWeaponLst1();
	afx_msg void OnUpdateWeaponLst2();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELLOPS_DIALOG };
#endif
	HICON m_icon;
	// database
	soechin::sqlite m_sqlite;
	// user interface
	CButton m_manageBtn;
	CComboBox m_weaponLst1;
	CComboBox m_weaponLst2;
};
