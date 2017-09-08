#pragma once

#include "sqlite.hpp"
#include <map>
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
	virtual void ReadSetting(std::string key, std::string& value);
	virtual void WriteSetting(std::string key, std::string value);
	afx_msg void OnBnClickedManageBtn();
	afx_msg void OnListWeapons1();
	afx_msg void OnListWeapons2();
	afx_msg void OnListSights1();
	afx_msg void OnListSights2();
	afx_msg void OnChangeWeapons1();
	afx_msg void OnChangeWeapons2();
	afx_msg void OnChangeSights1();
	afx_msg void OnChangeSights2();
	afx_msg void OnListDuration();
	afx_msg void OnListSensitivity();
	afx_msg void OnChangeDuration();
	afx_msg void OnChangeSensitivity();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELLOPS_DIALOG };
#endif
	HICON m_icon;
	// database
	soechin::sqlite m_sqlite;
	std::map<std::string, double> m_sights;
	std::string m_weapon1;
	std::string m_weapon2;
	std::string m_sight1;
	std::string m_sight2;
	std::string m_duration1;
	std::string m_duration2;
	std::string m_sensitivity1;
	std::string m_sensitivity2;
	std::string m_sensitivity3;
	// user interface
	CButton m_manageBtn;
	CComboBox m_weaponLst1;
	CComboBox m_weaponLst2;
	CComboBox m_sightLst1;
	CComboBox m_sightLst2;
	CEdit m_durationEdt1;
	CEdit m_durationEdt2;
	CEdit m_sensitivityEdt1;
	CEdit m_sensitivityEdt2;
	CEdit m_sensitivityEdt3;
};
