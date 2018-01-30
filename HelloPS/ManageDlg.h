#pragma once

#include "sqlite.hpp"

class CManageDlg : public CDialogEx {
    DECLARE_DYNAMIC(CManageDlg)
    DECLARE_MESSAGE_MAP()
  public:
    CManageDlg(soechin::sqlite *sqlite);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void UIFromDatabase();
    virtual void ReadSetting(std::string key, std::string &value);
    virtual void WriteSetting(std::string key, std::string value);
    virtual void ReloadWeaponLst1();
    virtual void ReloadWeaponLst2();
    afx_msg void OnBnClickedPrimaryRad();
    afx_msg void OnBnClickedSecondaryRad();
    afx_msg void OnBnClickedInsertBtn();
    afx_msg void OnBnClickedRemoveBtn();
    afx_msg void OnBnClickedImportBtn();
    afx_msg void OnCbnSelchangeFactionLst();
    afx_msg void OnCbnSelchangeCategoryLst();
    virtual bool DownloadWeaponData(CString &jsonFile);
  private:
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MANAGE_DIALOG };
#endif
    // database
    soechin::sqlite *m_sqlite;
    // user interface
    CButton m_primaryRad;
    CButton m_secondaryRad;
    CButton m_insertBtn;
    CButton m_removeBtn;
    CButton m_importBtn;
    CComboBox m_factionLst;
    CComboBox m_categoryLst;
    CListBox m_weaponLst1;
    CListBox m_weaponLst2;
    CProgressCtrl m_progressBar;
};
