#pragma once

#include "json.hpp"
#include <fstream>

class CHelloPSDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()
public:
	CHelloPSDlg();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void ReadSettingFile();
	virtual void WriteSettingFile();
	afx_msg void OnBnClickedImportBtn();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELLOPS_DIALOG };
#endif
	HICON m_icon;
	// setting file
	CString m_json;
	nlohmann::json m_cats;
	nlohmann::json m_factions;
	nlohmann::json m_modes;
	nlohmann::json m_weapons;
	// user interface
	CComboBox m_factionLst1;
	CComboBox m_categoryLst1;
	CComboBox m_weaponLst1;
};
