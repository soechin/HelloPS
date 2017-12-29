#pragma once

#include "sqlite.hpp"

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
	virtual void UIFromDatabase();
	virtual void ReadSetting(std::string key, std::string& value);
	virtual void WriteSetting(std::string key, std::string value);
	virtual void OnUpdateAction();
	virtual void OnUpdateEnabled();
	afx_msg void OnBnClickedManageBtn();
	afx_msg void OnCbnSelchangeWeaponLst1();
	afx_msg void OnCbnSelchangeWeaponLst2();
	afx_msg void OnCbnSelchangeSightLst1();
	afx_msg void OnCbnSelchangeSightLst2();
	afx_msg void OnEnKillfocusDurationEdt1();
	afx_msg void OnEnKillfocusDurationEdt2();
	afx_msg void OnEnKillfocusSensitivityEdt1();
	afx_msg void OnEnKillfocusSensitivityEdt2();
	afx_msg void OnEnKillfocusSensitivityEdt3();
	afx_msg void OnEnKillfocusDelayEdt1();
	afx_msg void OnEnKillfocusDelayEdt2();
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
	// weapon data
	double m_speed;
	double m_recoil;
	double m_factor;
	double m_angleMin;
	double m_angleMax;
	double m_burst;
	double m_delay;
	// sight data
	double m_zoom;
	// settings
	double m_duration1;
	double m_duration2;
	double m_sensitivity;
	double m_delay1;
	double m_delay2;
	double m_vertical;
	double m_horizonal;
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
	CEdit m_delayEdt1;
	CEdit m_delayEdt2;
};
