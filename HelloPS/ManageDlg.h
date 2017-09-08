#pragma once

#include "sqlite.hpp"

class CManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CManageDlg)
	DECLARE_MESSAGE_MAP()
public:
	CManageDlg(soechin::sqlite* sqlite);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateWeaponLst1();
	afx_msg void OnUpdateWeaponLst2();
	afx_msg void OnUpdateFactionLst();
	afx_msg void OnUpdateCategoryLst();
	afx_msg void OnBnClickedInsertBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedImportBtn();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGE_DIALOG };
#endif
	// database
	soechin::sqlite* m_sqlite;
	// user interface
	CButton m_primaryRad;
	CButton m_secondaryRad;
	CComboBox m_factionLst;
	CComboBox m_categoryLst;
	CListBox m_weaponLst1;
	CListBox m_weaponLst2;
	CButton m_insertBtn;
	CButton m_removeBtn;
	CButton m_importBtn;
};
