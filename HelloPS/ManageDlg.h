#pragma once

#include "json.hpp"

class CManageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CManageDlg)
	DECLARE_MESSAGE_MAP()
public:
	CManageDlg();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void GetJson(nlohmann::json& json);
	virtual void SetJson(nlohmann::json json);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPrimaryRad();
	afx_msg void OnBnClickedSecondaryRad();
	afx_msg void OnCbnDropdownFactionLst();
	afx_msg void OnCbnDropdownCategoryLst();
	afx_msg void OnCbnSelchangeFactionLst();
	afx_msg void OnCbnSelchangeCategoryLst();
	afx_msg void OnBnClickedInsertBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedImportBtn();
private:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGE_DIALOG };
#endif
	// json
	nlohmann::json m_json;
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
