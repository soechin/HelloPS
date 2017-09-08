#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"
#include "ManageDlg.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MANAGE_BTN, &CHelloPSDlg::OnBnClickedManageBtn)
	ON_CBN_DROPDOWN(IDC_WEAPON_LST_1, &CHelloPSDlg::OnUpdateWeaponLst1)
	ON_CBN_DROPDOWN(IDC_WEAPON_LST_2, &CHelloPSDlg::OnUpdateWeaponLst2)
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
}

BOOL CHelloPSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_icon, TRUE);
	SetIcon(m_icon, FALSE);

	// open database
	OpenDatabase();

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
	soechin::sqlite_stmt stmt;

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
}

void CHelloPSDlg::CloseDatabase()
{
	m_sqlite.exec("VACUUM;");
	m_sqlite.close();
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
	OnUpdateWeaponLst1();
	OnUpdateWeaponLst2();
}

void CHelloPSDlg::OnUpdateWeaponLst1()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string selText, name;
	int selIndex;

	m_weaponLst1.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_weaponLst1.ResetContent();
	selIndex = -1;

	stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons1 "
		"ORDER BY Name");

	while (stmt.step())
	{
		stmt.column("Name", name);

		if (selIndex < 0 && selText == name)
		{
			selIndex = m_weaponLst1.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_weaponLst1.AddString(text);
	}

	stmt.finalize();

	if (selIndex >= 0)
	{
		m_weaponLst1.SetCurSel(selIndex);
	}
}

void CHelloPSDlg::OnUpdateWeaponLst2()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string selText, name;
	int selIndex;

	m_weaponLst2.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_weaponLst2.ResetContent();
	selIndex = -1;

	stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons2 "
		"ORDER BY Name");

	while (stmt.step())
	{
		stmt.column("Name", name);

		if (selIndex < 0 && selText == name)
		{
			selIndex = m_weaponLst2.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_weaponLst2.AddString(text);
	}

	stmt.finalize();

	if (selIndex >= 0)
	{
		m_weaponLst2.SetCurSel(selIndex);
	}
}
