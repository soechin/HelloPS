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
	virtual LRESULT OnUpdateAction(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnUpdateEnabled(WPARAM wParam, LPARAM lParam);
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
	// timer/worker
	static void __stdcall TimerFunc1(LPVOID lpParam, BOOLEAN bTimer);
	static void __stdcall TimerFunc2(LPVOID lpParam, BOOLEAN bTimer);
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
	// timers
	HANDLE m_timer1;
	HANDLE m_timer2;
	// states
	LARGE_INTEGER x_freq;
	LARGE_INTEGER x_tick;
	int x_action;
	bool x_enabled;
	bool x_idle;
	bool x_lbutton;
	double x_fpow;
	double x_fsin;
	double x_fcos;
	double x_remain;
	int x_dx;
	int x_dy;
	// weapon data
	double x_speed;
	double x_recoil;
	double x_factor;
	double x_angleMin;
	double x_angleMax;
	double x_burst;
	double x_delay;
	// sight data
	double x_zoom;
	// settings
	double x_sens;
	double x_dura1;
	double x_dura2;
	double x_vert;
	double x_horz;
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
