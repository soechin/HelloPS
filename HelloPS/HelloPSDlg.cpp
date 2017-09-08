#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"
#include "ManageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
	ON_WM_DESTROY()
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

	OnChangeWeapons1();
	OnChangeWeapons2();
	OnChangeSights1();
	OnChangeSights2();
	OnChangeDuration();
	OnChangeSensitivity();

	return TRUE;
}

void CHelloPSDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

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
}

void CHelloPSDlg::OnChangeWeapons2()
{
	CString text;

	m_weaponLst2.GetWindowText(text);
	m_weapon2 = (LPSTR)ATL::CT2A(text, CP_UTF8);
}

void CHelloPSDlg::OnChangeSights1()
{
	CString text;

	m_sightLst1.GetWindowText(text);
	m_sight1 = (LPSTR)ATL::CT2A(text, CP_UTF8);
}

void CHelloPSDlg::OnChangeSights2()
{
	CString text;

	m_sightLst2.GetWindowText(text);
	m_sight2 = (LPSTR)ATL::CT2A(text, CP_UTF8);
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
