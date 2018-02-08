#pragma once

#include "sqlite.hpp"

class CHelloPSDlg : public CDialogEx {
    DECLARE_MESSAGE_MAP()
  public:
    CHelloPSDlg();
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    virtual void OpenDatabase();
    virtual void CloseDatabase();
    virtual void UIFromDatabase();
    virtual void ReadSetting(std::string key, std::string &value);
    virtual void WriteSetting(std::string key, std::string value);
    virtual void ReadSetting(std::string weapon, std::string key, std::string &value);
    virtual void WriteSetting(std::string weapon, std::string key, std::string value);
    virtual void OnUpdateAction();
    virtual void OnUpdateEnabled();
    afx_msg void OnBnClickedManageBtn();
    afx_msg void OnCbnSelchangeWeaponLst1();
    afx_msg void OnCbnSelchangeWeaponLst2();
    afx_msg void OnCbnSelchangeSightLst1();
    afx_msg void OnCbnSelchangeSightLst2();
    afx_msg void OnBnClickedPrimaryRad();
    afx_msg void OnBnClickedSecondaryRad();
    afx_msg void OnEnKillfocusGravityEdt();
    afx_msg void OnEnKillfocusZeroingEdt();
    afx_msg void OnEnKillfocusDurationBeginEdt();
    afx_msg void OnEnKillfocusDurationEndEdt();
    afx_msg void OnEnKillfocusSensitivityHipEdt();
    afx_msg void OnEnKillfocusSensitivityAimEdt();
    afx_msg void OnEnKillfocusSensitivityScopeEdt();
    afx_msg void OnEnKillfocusGraphicsFpsEdt();
    afx_msg void OnEnKillfocusGraphicsDelayEdt();
    afx_msg void OnEnKillfocusGraphicsFovEdt();
    // timer/worker
#ifdef OBSOLETE_TIMER
    static void __stdcall TimerFunc1(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
    static void __stdcall TimerFunc2(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
#else
    static void __stdcall TimerFunc1(LPVOID lpParam, BOOLEAN bTimer);
    static void __stdcall TimerFunc2(LPVOID lpParam, BOOLEAN bTimer);
#endif
    virtual void TimerFunc1();
    virtual void TimerFunc2();
    virtual void MoveMouse(int dx, int dy);
    virtual void DrawOSD();
  private:
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_HELLOPS_DIALOG };
#endif
    HICON m_icon;
    CCriticalSection m_mutex;
    // database
    soechin::sqlite m_sqlite;
    // timers
#ifdef OBSOLETE_TIMER
    UINT m_timer1;
    UINT m_timer2;
#else
    HANDLE m_timer1;
    HANDLE m_timer2;
#endif
    // states
    LARGE_INTEGER m_freq;
    LARGE_INTEGER m_tick;
    int m_action;
    bool m_enabled;
    bool m_idle;
    bool m_lbutton;
    double m_fpow;
    double m_fsin;
    double m_fcos;
    double m_limit;
    double m_remain;
    int m_dx;
    int m_dy;
    // Q-button
    LARGE_INTEGER m_qtick;
    bool m_qdown;
    // OSD
    CString m_osdClass;
    CWnd *m_osdWnd;
    COLORREF m_osdBg;
    COLORREF m_osdFg;
    COLORREF m_osdFg2;
    int m_osdDist;
    // weapon data
    std::string m_category;
    double m_speed;
    double m_recoil;
    double m_factor;
    double m_angleMin;
    double m_angleMax;
    double m_burst;
    double m_velocity;
    // sight data
    double m_zoom;
    // settings
    double m_gravity;
    double m_zeroing;
    double m_durationBegin;
    double m_durationEnd;
    double m_sensitivity;
    double m_graphicsFps;
    double m_graphicsDelay;
    double m_graphicsFov;
    double m_vertical;
    double m_horizonal;
    // user interface
    CButton m_manageBtn;
    CComboBox m_weaponLst1;
    CComboBox m_weaponLst2;
    CComboBox m_sightLst1;
    CComboBox m_sightLst2;
    CButton m_primaryRad;
    CButton m_secondaryRad;
    CEdit m_gravityEdt;
    CEdit m_zeroingEdt;
    CEdit m_durationBeginEdt;
    CEdit m_durationEndEdt;
    CEdit m_sensitivityHipEdt;
    CEdit m_sensitivityAimEdt;
    CEdit m_sensitivityScopeEdt;
    CEdit m_graphicsFpsEdt;
    CEdit m_graphicsDelayEdt;
    CEdit m_graphicsFovEdt;
};
