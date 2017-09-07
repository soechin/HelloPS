#include "stdafx.h"
#include "HelloPS.h"
#include "ManageDlg.h"
#include <fstream>

IMPLEMENT_DYNAMIC(CManageDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CManageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_PRIMARY_RAD, &CManageDlg::OnBnClickedPrimaryRad)
	ON_BN_CLICKED(IDC_SECONDARY_RAD, &CManageDlg::OnBnClickedSecondaryRad)
	ON_CBN_DROPDOWN(IDC_FACTION_LST, &CManageDlg::OnCbnDropdownFactionLst)
	ON_CBN_DROPDOWN(IDC_CATEGORY_LST, &CManageDlg::OnCbnDropdownCategoryLst)
	ON_CBN_SELCHANGE(IDC_FACTION_LST, &CManageDlg::OnCbnSelchangeFactionLst)
	ON_CBN_SELCHANGE(IDC_CATEGORY_LST, &CManageDlg::OnCbnSelchangeCategoryLst)
	ON_BN_CLICKED(IDC_INSERT_BTN, &CManageDlg::OnBnClickedInsertBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, &CManageDlg::OnBnClickedRemoveBtn)
	ON_BN_CLICKED(IDC_IMPORT_BTN, &CManageDlg::OnBnClickedImportBtn)
END_MESSAGE_MAP()

CManageDlg::CManageDlg() : CDialogEx(IDD_MANAGE_DIALOG)
{
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

void CManageDlg::GetJson(nlohmann::json& json)
{
	json = m_json;
}

void CManageDlg::SetJson(nlohmann::json json)
{
	m_json = json;
}

void CManageDlg::OnBnClickedPrimaryRad()
{
	OnBnClickedSecondaryRad();
}

void CManageDlg::OnBnClickedSecondaryRad()
{
	CString text;
	std::string selText, selRad;
	int selIndex;

	if (m_primaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons1";
	else if (m_secondaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons2";
	else return;

	if ((selIndex = m_weaponLst1.GetCurSel()) >= 0)
	{
		m_weaponLst1.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	m_weaponLst1.ResetContent();
	selIndex = -1;

	for (std::string name : m_json[selRad])
	{
		if (selIndex < 0 && selText == name)
		{
			selIndex = m_weaponLst1.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
		m_weaponLst1.AddString(text);
	}

	if (selIndex >= 0)
	{
		m_weaponLst1.SetCurSel(selIndex);
	}
}

void CManageDlg::OnCbnDropdownFactionLst()
{
	CString text;
	std::string selText;
	int selIndex;

	m_factionLst.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_factionLst.ResetContent();
	selIndex = -1;

	for (std::string faction : m_json["Factions"])
	{
		if (selIndex < 0 && selText == faction)
		{
			selIndex = m_factionLst.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(faction.c_str(), CP_UTF8);
		m_factionLst.AddString(text);
	}

	if (selIndex >= 0)
	{
		m_factionLst.SetCurSel(selIndex);
	}
}

void CManageDlg::OnCbnDropdownCategoryLst()
{
	CString text;
	std::string selText;
	int selIndex;

	m_categoryLst.GetWindowText(text);
	selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_categoryLst.ResetContent();
	selIndex = -1;

	for (std::string category : m_json["Categories"])
	{
		if (selIndex < 0 && selText == category)
		{
			selIndex = m_categoryLst.GetCount();
		}

		text = (LPTSTR)ATL::CA2T(category.c_str(), CP_UTF8);
		m_categoryLst.AddString(text);
	}

	if (selIndex >= 0)
	{
		m_categoryLst.SetCurSel(selIndex);
	}
}

void CManageDlg::OnCbnSelchangeFactionLst()
{
	OnCbnSelchangeCategoryLst();
}

void CManageDlg::OnCbnSelchangeCategoryLst()
{
	CString text;
	std::string selText, name, category, faction;
	nlohmann::json weapon;
	nlohmann::json::iterator it;
	int selIndex;

	m_categoryLst.GetWindowText(text);
	category = (LPSTR)ATL::CT2A(text, CP_UTF8);

	m_factionLst.GetWindowText(text);
	faction = (LPSTR)ATL::CT2A(text, CP_UTF8);

	if ((selIndex = m_weaponLst2.GetCurSel()) >= 0)
	{
		m_weaponLst2.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	m_weaponLst2.ResetContent();
	selIndex = -1;

	for (it = m_json["Weapons"].begin(); it != m_json["Weapons"].end(); it++)
	{
		name = it.key();
		weapon = it.value();

		if (weapon["Category"] == category &&
			weapon["Faction"] == faction)
		{
			if (selIndex < 0 && selText == name)
			{
				selIndex = m_weaponLst2.GetCount();
			}

			text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
			m_weaponLst2.AddString(text);
		}
	}

	if (selIndex >= 0)
	{
		m_weaponLst2.SetCurSel(selIndex);
	}
}

void CManageDlg::OnBnClickedInsertBtn()
{
	CString text;
	std::string selText, selRad;
	int selIndex;
	bool found;

	if (m_primaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons1";
	else if (m_secondaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons2";
	else return;

	if ((selIndex = m_weaponLst2.GetCurSel()) >= 0)
	{
		m_weaponLst2.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	if (!m_json["Weapons"][selText].is_null())
	{
		found = false;

		for (std::string name : m_json[selRad])
		{
			if (name == selText)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			m_json[selRad].push_back(selText);
		}
	}

	// refresh
	OnBnClickedSecondaryRad();
}

void CManageDlg::OnBnClickedRemoveBtn()
{
	CString text;
	std::string selText, selRad;
	nlohmann::json weapons;
	int selIndex;
	bool found;

	if (m_primaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons1";
	else if (m_secondaryRad.GetCheck() == BST_CHECKED) selRad = "Weapons2";
	else return;

	if ((selIndex = m_weaponLst1.GetCurSel()) >= 0)
	{
		m_weaponLst1.GetText(selIndex, text);
		selText = (LPSTR)ATL::CT2A(text, CP_UTF8);
	}

	for (std::string name : m_json[selRad])
	{
		if (name != selText)
		{
			weapons.push_back(name);
		}
	}

	m_json[selRad] = weapons;

	// refresh
	OnBnClickedSecondaryRad();
}

void CManageDlg::OnBnClickedImportBtn()
{
	CFileDialog ofd(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		TEXT("JSON|*.json||"));
	std::ifstream ifs;
	std::string name, cat, faction;
	nlohmann::json json, cats, factions, modes, mode, weapons, weapon;
	nlohmann::json::iterator it, found;
	double recoil, factor, angleMin, angleMax;
	int burst, delay, speed;

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

	cats = m_json["Categories"];
	factions = m_json["Factions"];
	modes = m_json["Modes"];

	for (nlohmann::json& i : json["item_list"])
	{
		// name, category
		if (!i["name"]["en"].is_string() ||
			!i["item_category"]["name"]["en"].is_string())
		{
			continue;
		}

		name = i["name"]["en"].get<std::string>();
		cat = i["item_category"]["name"]["en"].get<std::string>();

		// category filter
		if (std::find(cats.begin(), cats.end(), cat) == cats.end())
		{
			continue;
		}

		// faction
		if (i["faction_id"] == "1") faction = "VS";
		else if (i["faction_id"] == "2") faction = "NC";
		else if (i["faction_id"] == "3") faction = "TR";
		else faction = "NS";

		// fire mode filter
		found = modes.end();

		for (nlohmann::json& m : i["fire_mode"])
		{
			it = std::find(modes.begin(), modes.end(), m["description"]["en"]);

			if (it < found && m["type"] == "primary")
			{
				mode = m["fire_mode_2"];
				found = it;
			}
		}

		if (found == modes.end())
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

		weapon["Category"] = cat;
		weapon["Faction"] = faction;
		weapon["Burst"] = burst;
		weapon["Delay"] = delay;
		weapon["Speed"] = (int)((double)(60 * 1000) / speed + 0.5);
		weapon["Recoil"] = recoil;
		weapon["Factor"] = factor;
		weapon["AngleMin"] = angleMin;
		weapon["AngleMax"] = angleMax;

		weapons[name] = weapon;
	}

	m_json["Weapons"] = weapons;

	// refresh
	OnCbnSelchangeCategoryLst();
}
