#pragma once

#include "json.hpp"

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
	afx_msg void OnBnClickedManageBtn();
	afx_msg void OnCbnDropdownWeaponLst1();
	afx_msg void OnCbnSelchangeWeaponLst1();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELLOPS_DIALOG };
#endif
	HICON m_icon;
	// json
	CString m_path;
	nlohmann::json m_json;
	// user interface
	CComboBox m_weaponLst1;
};
