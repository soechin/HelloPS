#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"
#include "ManageDlg.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_ACTION (WM_APP + 1)
#define WM_UPDATE_ENABLED (WM_APP + 2)

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UPDATE_ACTION, &CHelloPSDlg::OnUpdateAction)
	ON_MESSAGE(WM_UPDATE_ENABLED, &CHelloPSDlg::OnUpdateEnabled)
	ON_BN_CLICKED(IDC_MANAGE_BTN, &CHelloPSDlg::OnBnClickedManageBtn)
	ON_CBN_DROPDOWN(IDC_WEAPON_LST_1, &CHelloPSDlg::OnListWeapons1)
	ON_CBN_DROPDOWN(IDC_WEAPON_LST_2, &CHelloPSDlg::OnListWeapons2)
	ON_CBN_DROPDOWN(IDC_SIGHT_LST_1, &CHelloPSDlg::OnListSights1)
	ON_CBN_DROPDOWN(IDC_SIGHT_LST_2, &CHelloPSDlg::OnListSights2)
	ON_CBN_SELCHANGE(IDC_WEAPON_LST_1, &CHelloPSDlg::OnChangeWeapons1)
	ON_CBN_SELCHANGE(IDC_WEAPON_LST_2, &CHelloPSDlg::OnChangeWeapons2)
	ON_CBN_SELCHANGE(IDC_SIGHT_LST_1, &CHelloPSDlg::OnChangeSights1)
	ON_CBN_SELCHANGE(IDC_SIGHT_LST_2, &CHelloPSDlg::OnChangeSights2)
	ON_EN_KILLFOCUS(IDC_DURATION_EDT_1, &CHelloPSDlg::OnChangeDuration)
	ON_EN_KILLFOCUS(IDC_DURATION_EDT_2, &CHelloPSDlg::OnChangeDuration)
	ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_1, &CHelloPSDlg::OnChangeSensitivity)
	ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_2, &CHelloPSDlg::OnChangeSensitivity)
	ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_3, &CHelloPSDlg::OnChangeSensitivity)
	ON_EN_KILLFOCUS(IDC_FRAMERATE_EDT, &CHelloPSDlg::OnChangeDelay)
	ON_EN_KILLFOCUS(IDC_FRAMES_EDT, &CHelloPSDlg::OnChangeDelay)
END_MESSAGE_MAP()

CHelloPSDlg::CHelloPSDlg() : CDialogEx(IDD_HELLOPS_DIALOG)
{
}

void CHelloPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MANAGE_BTN, m_manageBtn);
	DDX_Control(pDX, IDC_WEAPON_LST_1, m_weaponLst1);
	DDX_Control(pDX, IDC_WEAPON_LST_2, m_weaponLst2);
	DDX_Control(pDX, IDC_SIGHT_LST_1, m_sightLst1);
	DDX_Control(pDX, IDC_SIGHT_LST_2, m_sightLst2);
	DDX_Control(pDX, IDC_DURATION_EDT_1, m_durationEdt1);
	DDX_Control(pDX, IDC_DURATION_EDT_2, m_durationEdt2);
	DDX_Control(pDX, IDC_SENSITIVITY_EDT_1, m_sensitivityEdt1);
	DDX_Control(pDX, IDC_SENSITIVITY_EDT_2, m_sensitivityEdt2);
	DDX_Control(pDX, IDC_SENSITIVITY_EDT_3, m_sensitivityEdt3);
	DDX_Control(pDX, IDC_FRAMERATE_EDT, m_framerateEdt);
	DDX_Control(pDX, IDC_FRAMES_EDT, m_framesEdt);
}

BOOL CHelloPSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_icon, TRUE);
	SetIcon(m_icon, FALSE);

	// open database
	OpenDatabase();

	// initialize sights
	m_sights.clear();
	m_sights["1.0x Hip"] = 1.0;
	m_sights["1.35x Aim"] = 1.35;
	m_sights["2.0x Aim"] = 2.0;
	m_sights["3.4x Scope"] = 3.4;
	m_sights["4.0x Scope"] = 4.0;
	m_sights["6.0x Scope"] = 6.0;
	m_sights["8.0x Scope"] = 8.0;

	// user interface
	OnListWeapons1();
	OnListWeapons2();
	OnListSights1();
	OnListSights2();
	OnListDuration();
	OnListSensitivity();
	OnListDelay();

	OnChangeWeapons1();
	OnChangeWeapons2();
	OnChangeSights1();
	OnChangeSights2();
	OnChangeDuration();
	OnChangeSensitivity();
	OnChangeDelay();

	// create timers
	CreateTimerQueueTimer(&m_timer1, NULL, TimerFunc1, this, 0, 100, WT_EXECUTEDEFAULT);
	CreateTimerQueueTimer(&m_timer2, NULL, TimerFunc2, this, 0, 1, WT_EXECUTEDEFAULT);

	// states
	QueryPerformanceFrequency(&x_freq);
	QueryPerformanceCounter(&x_tick);
	x_action = 0;
	x_enabled = false;
	x_idle = false;
	x_lbutton = false;
	x_fpow = 0;
	x_fsin = 0;
	x_fcos = 0;
	x_limit = 0;
	x_remain = 0;
	x_dx = 0;
	x_dy = 0;

	// weapon data
	x_speed = 0;
	x_recoil = 0;
	x_factor = 0;
	x_angleMin = 0;
	x_angleMax = 0;
	x_burst = 0;
	x_delay = 0;

	// sight data
	x_zoom = 1.0;

	// settings
	x_sens = 0.3;
	x_dura1 = 0;
	x_dura2 = 1;
	x_vert = 1;
	x_horz = 0;
	x_framerate = 100;
	x_frames = 2;

	return TRUE;
}

void CHelloPSDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// delete timers
	DeleteTimerQueueTimer(NULL, m_timer1, INVALID_HANDLE_VALUE);
	DeleteTimerQueueTimer(NULL, m_timer2, INVALID_HANDLE_VALUE);

	// close database
	CloseDatabase();
}

void CHelloPSDlg::OpenDatabase()
{
	CString text;
	std::string path, sql;

	AfxGetModuleFileName(AfxGetInstanceHandle(), text);
	PathRenameExtension(text.GetBuffer(text.GetLength() + MAX_PATH), TEXT(".db"));
	text.ReleaseBuffer();
	path = (LPSTR)ATL::CT2A(text, CP_UTF8);

	// open
	m_sqlite.open(path);

	// create table "WeaponsDB"
	m_sqlite.exec("CREATE TABLE IF NOT EXISTS WeaponsDB "
		"(Name TEXT NOT NULL UNIQUE, Faction TEXT, Category TEXT, "
		"Speed INTEGER, Recoil REAL, Factor REAL, AngleMin REAL, "
		"AngleMax REAL, Burst REAL, Delay REAL, PRIMARY KEY(Name));");

	// create table "Weapons1", "Weapons2"
	m_sqlite.exec("CREATE TABLE IF NOT EXISTS Weapons1 "
		"(Name TEXT NOT NULL UNIQUE, PRIMARY KEY(Name));");
	m_sqlite.exec("CREATE TABLE IF NOT EXISTS Weapons2 "
		"(Name TEXT NOT NULL UNIQUE, PRIMARY KEY(Name));");

	// create table "Sights"
	m_sqlite.exec("CREATE TABLE IF NOT EXISTS Sights "
		"(Name TEXT NOT NULL UNIQUE, Zoom REAL, PRIMARY KEY(Name));");

	// create table "Settings"
	m_sqlite.exec("CREATE TABLE IF NOT EXISTS Settings "
		"(Key TEXT NOT NULL UNIQUE, Value TEXT, PRIMARY KEY(Key));");

	// read settings
	ReadSetting("Weapon1", m_weapon1);
	ReadSetting("Weapon2", m_weapon2);
	ReadSetting("Sight1", m_sight1);
	ReadSetting("Sight2", m_sight2);
	ReadSetting("Duration1", m_duration1);
	ReadSetting("Duration2", m_duration2);
	ReadSetting("Sensitivity1", m_sensitivity1);
	ReadSetting("Sensitivity2", m_sensitivity2);
	ReadSetting("Sensitivity3", m_sensitivity3);
	ReadSetting("Framerate", m_framerate);
	ReadSetting("Frames", m_frames);
}

void CHelloPSDlg::CloseDatabase()
{
	// write settings
	WriteSetting("Weapon1", m_weapon1);
	WriteSetting("Weapon2", m_weapon2);
	WriteSetting("Sight1", m_sight1);
	WriteSetting("Sight2", m_sight2);
	WriteSetting("Duration1", m_duration1);
	WriteSetting("Duration2", m_duration2);
	WriteSetting("Sensitivity1", m_sensitivity1);
	WriteSetting("Sensitivity2", m_sensitivity2);
	WriteSetting("Sensitivity3", m_sensitivity3);
	WriteSetting("Framerate", m_framerate);
	WriteSetting("Frames", m_frames);

	m_sqlite.exec("VACUUM;");
	m_sqlite.close();
}

void CHelloPSDlg::ReadSetting(std::string key, std::string& value)
{
	soechin::sqlite_stmt stmt;

	value.clear();

	if (key.empty())
	{
		return;
	}

	stmt.prepare(&m_sqlite, "SELECT Value FROM Settings "
		"WHERE Key = @key;");
	stmt.bind("@key", key);

	if (stmt.step())
	{
		stmt.column("Value", value);
	}

	stmt.finalize();
}

void CHelloPSDlg::WriteSetting(std::string key, std::string value)
{
	soechin::sqlite_stmt stmt;

	if (key.empty())
	{
		return;
	}

	stmt.prepare(&m_sqlite, "INSERT OR REPLACE INTO Settings "
		"(Key, Value) VALUES (@key, @value);");
	stmt.bind("@key", key);
	stmt.bind("@value", value);
	stmt.step();
	stmt.finalize();
}

LRESULT CHelloPSDlg::OnUpdateAction(WPARAM wParam, LPARAM lParam)
{
	CSingleLock locker(&m_mutex, TRUE);
	std::string weapon, sight;
	soechin::sqlite_stmt stmt;

	if (x_action == 1)
	{
		weapon = m_weapon1;
		sight = m_sight1;
	}
	else if (x_action == 2)
	{
		weapon = m_weapon2;
		sight = m_sight2;
	}

	if (weapon.empty() || sight.empty())
	{
		return 0;
	}

	// weapon data
	stmt.prepare(&m_sqlite, "SELECT Speed, Recoil, Factor, "
		"AngleMin, AngleMax, Burst, Delay FROM WeaponsDB WHERE "
		"Name = @name;");
	stmt.bind("@name", weapon);

	if (stmt.step())
	{
		stmt.column("Speed", x_speed);
		stmt.column("Recoil", x_recoil);
		stmt.column("Factor", x_factor);
		stmt.column("AngleMin", x_angleMin);
		stmt.column("AngleMax", x_angleMax);
		stmt.column("Burst", x_burst);
		stmt.column("Delay", x_delay);
	}

	stmt.finalize();

	// sight data
	x_zoom = m_sights[sight];

	// settings
	try
	{
		if (x_zoom <= 1)
		{
			x_sens = std::stod(m_sensitivity1);
		}
		else if (x_zoom <= 2)
		{
			x_sens = std::stod(m_sensitivity2);
		}
		else
		{
			x_sens = std::stod(m_sensitivity3);
		}
	}
	catch (...)
	{
		x_sens = 0;
	}

	try
	{
		x_dura1 = std::stod(m_duration1);
		x_dura2 = std::stod(m_duration2);
	}
	catch (...)
	{
		x_dura1 = 0;
		x_dura2 = 1;
	}

	try
	{
		x_framerate = std::stod(m_framerate);
		x_frames = std::stod(m_frames);
	}
	catch (...)
	{
		x_framerate = 100;
		x_frames = 2;
	}

	return 0;
}

LRESULT CHelloPSDlg::OnUpdateEnabled(WPARAM wParam, LPARAM lParam)
{
	CSingleLock locker(&m_mutex, TRUE);

	m_durationEdt1.EnableWindow(!x_enabled);
	m_durationEdt2.EnableWindow(!x_enabled);
	m_sensitivityEdt1.EnableWindow(!x_enabled);
	m_sensitivityEdt2.EnableWindow(!x_enabled);
	m_sensitivityEdt3.EnableWindow(!x_enabled);
	m_framerateEdt.EnableWindow(!x_enabled);
	m_framesEdt.EnableWindow(!x_enabled);

	OnUpdateAction(wParam, lParam);
	return 0;
}

void CHelloPSDlg::OnBnClickedManageBtn()
{
	CManageDlg dlg(&m_sqlite);

	m_sqlite.exec("BEGIN;");

	if (dlg.DoModal() != IDOK)
	{
		m_sqlite.exec("ROLLBACK;");
		return;
	}

	m_sqlite.exec("COMMIT;");

	// refresh
	OnListWeapons1();
	OnListWeapons2();
}

void CHelloPSDlg::OnListWeapons1()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string name;
	int index;

	m_weaponLst1.ResetContent();
	index = -1;

	stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons1 "
		"ORDER BY Name");

	while (stmt.step())
	{
		stmt.column("Name", name);

		if (index < 0 && name == m_weapon1)
		{
			index = m_weaponLst1.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_weaponLst1.AddString(text);
	}

	stmt.finalize();

	if (index >= 0)
	{
		m_weaponLst1.SetCurSel(index);
	}
}

void CHelloPSDlg::OnListWeapons2()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string name;
	int index;

	m_weaponLst2.ResetContent();
	index = -1;

	stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons2 "
		"ORDER BY Name");

	while (stmt.step())
	{
		stmt.column("Name", name);

		if (index < 0 && name == m_weapon2)
		{
			index = m_weaponLst2.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_weaponLst2.AddString(text);
	}

	stmt.finalize();

	if (index >= 0)
	{
		m_weaponLst2.SetCurSel(index);
	}
}

void CHelloPSDlg::OnListSights1()
{
	CString text;
	std::string name;
	std::map<std::string, double>::iterator it;
	int index;

	m_sightLst1.ResetContent();
	index = -1;

	for (it = m_sights.begin(); it != m_sights.end(); it++)
	{
		name = it->first;

		if (index < 0 && name == m_sight1)
		{
			index = m_sightLst1.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_sightLst1.AddString(text);
	}

	if (index >= 0)
	{
		m_sightLst1.SetCurSel(index);
	}
}

void CHelloPSDlg::OnListSights2()
{
	CString text;
	std::string name;
	std::map<std::string, double>::iterator it;
	int index;

	m_sightLst2.ResetContent();
	index = -1;

	for (it = m_sights.begin(); it != m_sights.end(); it++)
	{
		name = it->first;

		if (index < 0 && name == m_sight2)
		{
			index = m_sightLst2.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_sightLst2.AddString(text);
	}

	if (index >= 0)
	{
		m_sightLst2.SetCurSel(index);
	}
}

void CHelloPSDlg::OnChangeWeapons1()
{
	CString text;

	m_weaponLst1.GetWindowText(text);
	m_weapon1 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	// update action
	PostMessage(WM_UPDATE_ACTION);
}

void CHelloPSDlg::OnChangeWeapons2()
{
	CString text;

	m_weaponLst2.GetWindowText(text);
	m_weapon2 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	// update action
	PostMessage(WM_UPDATE_ACTION);
}

void CHelloPSDlg::OnChangeSights1()
{
	CString text;

	m_sightLst1.GetWindowText(text);
	m_sight1 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	// update action
	PostMessage(WM_UPDATE_ACTION);
}

void CHelloPSDlg::OnChangeSights2()
{
	CString text;

	m_sightLst2.GetWindowText(text);
	m_sight2 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	// update action
	PostMessage(WM_UPDATE_ACTION);
}

void CHelloPSDlg::OnListDuration()
{
	CString text;

	text = (LPTSTR)ATL::CA2T(m_duration1.c_str(), CP_UTF8);
	m_durationEdt1.SetWindowText(text);

	text = (LPTSTR)ATL::CA2T(m_duration2.c_str(), CP_UTF8);
	m_durationEdt2.SetWindowText(text);
}

void CHelloPSDlg::OnListSensitivity()
{
	CString text;

	text = (LPTSTR)ATL::CA2T(m_sensitivity1.c_str(), CP_UTF8);
	m_sensitivityEdt1.SetWindowText(text);

	text = (LPTSTR)ATL::CA2T(m_sensitivity2.c_str(), CP_UTF8);
	m_sensitivityEdt2.SetWindowText(text);

	text = (LPTSTR)ATL::CA2T(m_sensitivity3.c_str(), CP_UTF8);
	m_sensitivityEdt3.SetWindowText(text);
}

void CHelloPSDlg::OnListDelay()
{
	CString text;

	text = (LPTSTR)ATL::CA2T(m_framerate.c_str(), CP_UTF8);
	m_framerateEdt.SetWindowText(text);

	text = (LPTSTR)ATL::CA2T(m_frames.c_str(), CP_UTF8);
	m_framesEdt.SetWindowText(text);
}

void CHelloPSDlg::OnChangeDuration()
{
	CString text;

	m_durationEdt1.GetWindowText(text);
	m_duration1 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_durationEdt2.GetWindowText(text);
	m_duration2 = (LPSTR)ATL::CT2A(text, CP_UTF8);
}

void CHelloPSDlg::OnChangeSensitivity()
{
	CString text;

	m_sensitivityEdt1.GetWindowText(text);
	m_sensitivity1 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_sensitivityEdt2.GetWindowText(text);
	m_sensitivity2 = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_sensitivityEdt3.GetWindowText(text);
	m_sensitivity3 = (LPSTR)ATL::CT2A(text, CP_UTF8);
}

void CHelloPSDlg::OnChangeDelay()
{
	CString text;

	m_framerateEdt.GetWindowText(text);
	m_framerate = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_framesEdt.GetWindowText(text);
	m_frames = (LPSTR)ATL::CT2A(text, CP_UTF8);
}

void __stdcall CHelloPSDlg::TimerFunc1(LPVOID lpParam, BOOLEAN bTimer)
{
	if (lpParam != NULL && bTimer)
	{
		((CHelloPSDlg*)lpParam)->TimerFunc1();
	}
}

void __stdcall CHelloPSDlg::TimerFunc2(LPVOID lpParam, BOOLEAN bTimer)
{
	if (lpParam != NULL && bTimer)
	{
		((CHelloPSDlg*)lpParam)->TimerFunc2();
	}
}

void CHelloPSDlg::TimerFunc1()
{
	if ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0)
	{
		if ((GetAsyncKeyState(VK_F1) & 0x8000) != 0) // ALT-F1
		{
			CSingleLock locker(&m_mutex, TRUE);

			if (!x_enabled)
			{
				x_enabled = true;
				PostMessage(WM_UPDATE_ENABLED);
			}
		}
		else if ((GetAsyncKeyState(VK_F2) & 0x8000) != 0) // ALT-F2
		{
			CSingleLock locker(&m_mutex, TRUE);

			if (x_enabled)
			{
				x_enabled = false;
				PostMessage(WM_UPDATE_ENABLED);
			}
		}
	}
	else if ((GetAsyncKeyState(0xc0) & 0x8000) != 0) // '`'
	{
		CSingleLock locker(&m_mutex, TRUE);

		if (x_action != 0)
		{
			x_action = 0;
			PostMessage(WM_UPDATE_ACTION);
		}
	}
	else if ((GetAsyncKeyState(0x31) & 0x8000) != 0) // '1'
	{
		CSingleLock locker(&m_mutex, TRUE);

		if (x_action != 1)
		{
			x_action = 1;
			PostMessage(WM_UPDATE_ACTION);
		}
	}
	else if ((GetAsyncKeyState(0x32) & 0x8000) != 0) // '2'
	{
		CSingleLock locker(&m_mutex, TRUE);

		if (x_action != 2)
		{
			x_action = 2;
			PostMessage(WM_UPDATE_ACTION);
		}
	}
}

void CHelloPSDlg::TimerFunc2()
{
	CSingleLock locker(&m_mutex, TRUE);
	CURSORINFO cursor;
	LARGE_INTEGER tick;
	bool lbutton;
	double ifps, ishot, time, num, intp, frac, dz;
	int dx, dy, ox, oy;

	// check action
	if ((x_action != 1 && x_action != 2) || !x_enabled)
	{
		return;
	}

	// lbutton down
	lbutton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	// first shot
	if (lbutton && !x_lbutton)
	{
		// idle
		x_idle = false;

		// check lshift
		if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0)
		{
			x_idle = true;
		}

		// check cursor
		memset(&cursor, 0, sizeof(cursor));
		cursor.cbSize = sizeof(cursor);

		if (GetCursorInfo(&cursor))
		{
			if ((cursor.flags & CURSOR_SHOWING) != 0)
			{
#ifndef _DEBUG
				x_idle = true;
#endif
			}
		}

		// time begin
		QueryPerformanceCounter(&x_tick);

		// S: mouse sensivisity
		// Z: zoom level
		// Z': zoom level > 1 ? 1.6 : 1
		// A1: min recoil angle
		// A2: max recoil angle
		// F = (11.7581 / (S + 0.3)) ^ 3 * Z * Z' / 2.54 / 360
		x_fpow = pow(11.7581 / (x_sens + 0.3), 3) * x_zoom * (x_zoom > 1 ? 1.6 : 1) / 2.54 / 360;
		// Fsin = sin((A1 + A2) / 2 * PI / 180)
		x_fsin = sin(((x_angleMin + x_angleMax) / 2) * M_PI / 180);
		// Fcos = cos(((|A1| + |A2|) / 2) * PI / 180)
		x_fcos = cos(((abs(x_angleMin) + abs(x_angleMax)) / 2) * M_PI / 180);

		// reset burst
		x_limit = 0;
		x_remain = 0;

		// reset offsets
		x_dx = 0;
		x_dy = 0;
	}

	// backup lbutton state
	x_lbutton = lbutton;

	// abort
	if (x_idle)
	{
		return;
	}

	// lbutton down
	if (lbutton || x_remain > 0)
	{
		// time end
		QueryPerformanceCounter(&tick);
		time = (double)(tick.QuadPart - x_tick.QuadPart) / x_freq.QuadPart;

		// fps interval
		ifps = 1 / x_framerate;

		// shot interval
		ishot = 60 / x_speed;

		// delay
		time = std::max(time - x_delay - ifps, 0.0);

		// time limit(for burst mode)
		if (x_burst > 0)
		{
			x_limit = x_burst * ishot;

			// time limit
			if (time > x_limit)
			{
				time = x_limit;
			}

			// time remaining
			x_remain = x_limit - time;
		}

		// number of shots
		num = time / ishot;
		frac = std::modf(num, &intp);

		if (x_burst <= 0)
		{
			if (lbutton)
			{
				x_limit = frac > std::min(ifps / ishot * x_frames, 1.0) ? ceil(num) : num;
			}

			if (num > x_limit)
			{
				num = x_limit;
				frac = std::modf(num, &intp);
			}

			x_remain = x_limit - num;
		}

		// new number
		frac = (frac - x_dura1) / (x_dura2 - x_dura1);
		num = intp + std::min(std::max(frac, 0.0), 1.0);

		// distance to recoil center
		dz = (x_factor * std::min(num, 1.0) + std::max(num - 1, 0.0)) * x_recoil * x_fpow;

		// hypotenuse to opposite and adjacent
		dx = (int)(dz * x_fsin * x_horz);
		dy = (int)(dz * x_fcos * x_vert);

		// move mouse
		if (x_dx != dx || x_dy != dy)
		{
			ox = (x_dx < dx) ? 1 : (x_dx > dx) ? -1 : 0;
			oy = (x_dy < dy) ? 1 : (x_dy > dy) ? -1 : 0;
			if (ox != 0) ox *= std::min((int)sqrt(abs(x_dx - dx)), 3);
			if (oy != 0) oy *= std::min((int)sqrt(abs(x_dy - dy)), 3);
			MoveMouse(-ox, oy);
			x_dx += ox;
			x_dy += oy;
		}
	}
}

void CHelloPSDlg::MoveMouse(int dx, int dy)
{
	INPUT input;

	memset(&input, 0, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.dx = dx;
	input.mi.dy = dy;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(1, &input, sizeof(input));
}
