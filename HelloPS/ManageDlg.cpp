#include "stdafx.h"
#include "HelloPS.h"
#include "ManageDlg.h"
// JSON for Modern C++
#include "json.hpp"
#include <fstream>

IMPLEMENT_DYNAMIC(CManageDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CManageDlg, CDialogEx)
	ON_CBN_DROPDOWN(IDC_FACTION_LST, &CManageDlg::OnUpdateFactionLst)
	ON_CBN_DROPDOWN(IDC_CATEGORY_LST, &CManageDlg::OnUpdateCategoryLst)
	ON_BN_CLICKED(IDC_PRIMARY_RAD, &CManageDlg::OnUpdateWeaponLst1)
	ON_BN_CLICKED(IDC_SECONDARY_RAD, &CManageDlg::OnUpdateWeaponLst1)
	ON_CBN_SELCHANGE(IDC_FACTION_LST, &CManageDlg::OnUpdateWeaponLst2)
	ON_CBN_SELCHANGE(IDC_CATEGORY_LST, &CManageDlg::OnUpdateWeaponLst2)
	ON_BN_CLICKED(IDC_INSERT_BTN, &CManageDlg::OnBnClickedInsertBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &CManageDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDC_IMPORT_BTN, &CManageDlg::OnBnClickedImportBtn)
END_MESSAGE_MAP()

CManageDlg::CManageDlg(soechin::sqlite* sqlite) : CDialogEx(IDD_MANAGE_DIALOG)
{
	m_sqlite = sqlite;
}

void CManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRIMARY_RAD, m_primaryRad);
	DDX_Control(pDX, IDC_SECONDARY_RAD, m_secondaryRad);
	DDX_Control(pDX, IDC_FACTION_LST, m_factionLst);
	DDX_Control(pDX, IDC_CATEGORY_LST, m_categoryLst);
	DDX_Control(pDX, IDC_WEAPON_LST_1, m_weaponLst1);
	DDX_Control(pDX, IDC_WEAPON_LST_2, m_weaponLst2);
	DDX_Control(pDX, IDC_INSERT_BTN, m_insertBtn);
	DDX_Control(pDX, IDC_REMOVE_BTN, m_removeBtn);
	DDX_Control(pDX, IDC_IMPORT_BTN, m_importBtn);
}

BOOL CManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

void CManageDlg::OnUpdateWeaponLst1()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string selText, name;
	int selIndex;

	if ((selIndex = m_weaponLst1.GetCurSel()) >= 0)
	{
		m_weaponLst1.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	m_weaponLst1.ResetContent();
	selIndex = -1;

	if (m_primaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "SELECT Name FROM Weapons1 "
			"ORDER BY Name");
	}
	else if (m_secondaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "SELECT Name FROM Weapons2 "
			"ORDER BY Name");
	}
	else
	{
		return;
	}

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

void CManageDlg::OnUpdateWeaponLst2()
{
	CString text;
	std::string selText, name, faction, category;
	soechin::sqlite_stmt stmt;
	int selIndex;

	m_factionLst.GetWindowText(text);
	faction = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_categoryLst.GetWindowText(text);
	category = (LPSTR)ATL::CT2A(text, CP_UTF8);

	if ((selIndex = m_weaponLst2.GetCurSel()) >= 0)
	{
		m_weaponLst2.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	m_weaponLst2.ResetContent();
	selIndex = -1;

	stmt.prepare(m_sqlite, "SELECT Name FROM WeaponsDB WHERE "
		"Faction = @faction AND Category = @category ORDER BY Name;");
	stmt.bind("@faction", faction);
	stmt.bind("@category", category);

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

void CManageDlg::OnUpdateFactionLst()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string selText, faction;
	int selIndex;

	m_factionLst.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_factionLst.ResetContent();
	selIndex = -1;

	stmt.prepare(m_sqlite, "SELECT DISTINCT Faction FROM WeaponsDB "
		"ORDER BY Faction;");

	while (stmt.step())
	{
		stmt.column("Faction", faction);

		if (selIndex < 0 && selText == faction)
		{
			selIndex = m_factionLst.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(faction.c_str(), CP_UTF8);
		m_factionLst.AddString(text);
	}

	stmt.finalize();

	if (selIndex >= 0)
	{
		m_factionLst.SetCurSel(selIndex);
	}
}

void CManageDlg::OnUpdateCategoryLst()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string selText, category;
	int selIndex;

	m_categoryLst.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_categoryLst.ResetContent();
	selIndex = -1;

	stmt.prepare(m_sqlite, "SELECT DISTINCT Category FROM WeaponsDB "
		"ORDER BY Category;");

	while (stmt.step())
	{
		stmt.column("Category", category);

		if (selIndex < 0 && selText == category)
		{
			selIndex = m_categoryLst.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(category.c_str(), CP_UTF8);
		m_categoryLst.AddString(text);
	}

	stmt.finalize();

	if (selIndex >= 0)
	{
		m_categoryLst.SetCurSel(selIndex);
	}
}

void CManageDlg::OnBnClickedInsertBtn()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string name;
	int selIndex;

	if ((selIndex = m_weaponLst2.GetCurSel()) >= 0)
	{
		m_weaponLst2.GetText(selIndex, text);
		name = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	if (name.empty())
	{
		return;
	}

	if (m_primaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO Weapons1 "
			"(Name) VALUES (@name);");
	}
	else if (m_secondaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO Weapons2 "
			"(Name) VALUES (@name);");
	}
	else
	{
		return;
	}

	stmt.bind("@name", name);
	stmt.step();
	stmt.finalize();

	// refresh
	OnUpdateWeaponLst1();
}

void CManageDlg::OnBnClickedRemoveBtn()
{
	CString text;
	soechin::sqlite_stmt stmt;
	std::string name;
	int selIndex;

	if ((selIndex = m_weaponLst1.GetCurSel()) >= 0)
	{
		m_weaponLst1.GetText(selIndex, text);
		name = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	if (name.empty())
	{
		return;
	}

	if (m_primaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "DELETE FROM Weapons1 WHERE "
			"Name = @name;");
	}
	else if (m_secondaryRad.GetCheck() == BST_CHECKED)
	{
		stmt.prepare(m_sqlite, "DELETE FROM Weapons2 WHERE "
			"Name = @name;");
	}
	else
	{
		return;
	}

	stmt.bind("@name", name);
	stmt.step();
	stmt.finalize();

	// refresh
	OnUpdateWeaponLst1();
}

void CManageDlg::OnBnClickedImportBtn()
{
	CFileDialog ofd(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		TEXT("JSON|*.json||"));
	std::ifstream ifs;
	std::string name, category, faction;
	std::vector<std::string> categories, factions, modes;
	nlohmann::json json, item, mode, modej;
	soechin::sqlite_stmt stmt;
	double recoil, factor, angleMin, angleMax;
	int found, burst, delay, speed;

	if (ofd.DoModal() != IDOK)
	{
		return;
	}

	// open, read, and close
	ifs.open(ofd.GetPathName());

	if (ifs.is_open())
	{
		json << ifs;
		ifs.close();
	}

	// DO NOT sort factions and modes
	categories.push_back("Assault Rifle");
	categories.push_back("Battle Rifle");
	categories.push_back("Carbine");
	categories.push_back("LMG");
	categories.push_back("Pistol");
	categories.push_back("SMG");
	categories.push_back("Scout Rifle");
	categories.push_back("Sniper Rifle");

	factions.push_back("NS");
	factions.push_back("VS");
	factions.push_back("NC");
	factions.push_back("TR");

	modes.push_back("Auto");
	modes.push_back("3x Burst");
	modes.push_back("2x Burst");
	modes.push_back("Semi-Auto");

	for (int i = 0; i < (int)json["item_list"].size(); i++)
	{
		item = json["item_list"][i];

		// name, category
		if (!item["name"]["en"].is_string() ||
			!item["item_category"]["name"]["en"].is_string())
		{
			continue;
		}

		name = item["name"]["en"].get<std::string>();
		category = item["item_category"]["name"]["en"].get<std::string>();

		// category filter
		found = (int)categories.size();

		for (int j = 0; j < (int)categories.size(); j++)
		{
			if (categories[j] == category)
			{
				found = j;
				break;
			}
		}

		if (found >= (int)categories.size())
		{
			continue;
		}

		// faction
		if (item["faction_id"] == "1") faction = factions[1];
		else if (item["faction_id"] == "2") faction = factions[2];
		else if (item["faction_id"] == "3") faction = factions[3];
		else faction = factions[0];

		// fire mode filter
		found = (int)modes.size();

		for (int j = 0; j < (int)item["fire_mode"].size(); j++)
		{
			modej = item["fire_mode"][j];

			if (modej["type"] != "primary")
			{
				continue;
			}

			for (int k = 0; k < found; k++)
			{
				if (modej["description"]["en"] == modes[k])
				{
					mode = modej["fire_mode_2"];
					found = k;
					break;
				}
			}
		}

		if (found >= (int)modes.size())
		{
			continue;
		}

		// burst count
		burst = std::stoi(mode["fire_burst_count"].get<std::string>());

		// first shot delay
		delay = std::stoi(mode["fire_delay_ms"].get<std::string>());

		// auto, semi-auto
		if (burst <= 1)
		{
			speed = std::stoi(mode["fire_refire_ms"].get<std::string>());

			if (std::stoi(mode["automatic"].get<std::string>()) != 0)
			{
				burst = 0;
			}
		}
		// 2x burst, 3x burst
		else
		{
			speed = std::stoi(mode["fire_auto_fire_ms"].get<std::string>());
		}

		// recoil, first shot multiplier, angle
		recoil = std::stod(mode["recoil_magnitude_min"].get<std::string>());
		factor = std::stod(mode["recoil_first_shot_modifier"].get<std::string>());
		angleMin = std::stod(mode["recoil_angle_min"].get<std::string>());
		angleMax = std::stod(mode["recoil_angle_max"].get<std::string>());

		// insert into database
		stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO WeaponsDB "
			"(Name, Faction, Category, Speed, Recoil, Factor, "
			"AngleMin, AngleMax, Burst, Delay) VALUES "
			"(@name, @faction, @category, @speed, @recoil, @factor, "
			"@angleMin, @angleMax, @burst, @delay);");
		stmt.bind("@name", name);
		stmt.bind("@faction", faction);
		stmt.bind("@category", category);
		stmt.bind("@speed", (int)((double)(60 * 1000) / speed + 0.5)); // ms -> rpm
		stmt.bind("@recoil", recoil);
		stmt.bind("@factor", factor);
		stmt.bind("@angleMin", angleMin);
		stmt.bind("@angleMax", angleMax);
		stmt.bind("@burst", burst);
		stmt.bind("@delay", (double)delay / 1000); // ms -> sec
		stmt.step();
		stmt.finalize();
	}

	// refresh
	OnUpdateWeaponLst2();
}
