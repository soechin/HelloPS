#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_IMPORT_BTN, &CHelloPSDlg::OnBnClickedImportBtn)
END_MESSAGE_MAP()

CHelloPSDlg::CHelloPSDlg() : CDialogEx(IDD_HELLOPS_DIALOG)
{
}

void CHelloPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FACTION_LST_1, m_factionLst1);
	DDX_Control(pDX, IDC_CATEGORY_LST_1, m_categoryLst1);
	DDX_Control(pDX, IDC_WEAPON_LST_1, m_weaponLst1);
}

BOOL CHelloPSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_icon, TRUE);
	SetIcon(m_icon, FALSE);

	// setting file
	ReadSettingFile();

	return TRUE;
}

void CHelloPSDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// setting file
	WriteSettingFile();
}

void CHelloPSDlg::ReadSettingFile()
{
	std::ifstream ifs;
	nlohmann::json json;

	// path
	if (m_json.IsEmpty())
	{
		AfxGetModuleFileName(AfxGetInstanceHandle(), m_json);
		PathRenameExtension(m_json.GetBuffer(m_json.GetLength() + MAX_PATH), TEXT(".json"));
		m_json.ReleaseBuffer();
	}

	// open, read, and close
	ifs.open(m_json);

	if (ifs.is_open())
	{
		json << ifs;
		ifs.close();
	}

	// settings
	m_cats = json["Categories"];
	m_factions = json["Factions"];
	m_modes = json["Modes"];
	m_weapons = json["Weapons"];

	// default: category filters
	if (!m_cats.is_array())
	{
		m_cats =
		{
			"Assault Rifle", "Battle Rifle", "Carbine", "LMG", "Pistol",
			"SMG", "Scout Rifle", "Sniper Rifle"
		};
	}

	// default: factions
	if (!m_factions.is_array())
	{
		m_factions =
		{
			"NC", "NS", "TR", "VS"
		};
	}

	// default: fire mode filters
	if (!m_modes.is_array())
	{
		m_modes =
		{
			"Auto", "3x Burst", "2x Burst", "Semi-Auto"
		};
	}
}

void CHelloPSDlg::WriteSettingFile()
{
	std::ofstream ofs;
	nlohmann::json json;

	// settings
	json["Categories"] = m_cats;
	json["Factions"] = m_factions;
	json["Modes"] = m_modes;
	json["Weapons"] = m_weapons;

	// open, write, and close
	ofs.open(m_json);

	if (ofs.is_open())
	{
		ofs << std::setw(4) << json;
		ofs.close();
	}
}

void CHelloPSDlg::OnBnClickedImportBtn()
{
	CFileDialog ofd(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		TEXT("JavaScript Object Notation|*.json||"));
	std::ifstream ifs;
	std::string name, cat, faction;
	nlohmann::json json, weapons, mode, data;
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
		if (std::find(m_cats.begin(), m_cats.end(), cat) == m_cats.end())
		{
			continue;
		}

		// faction
		if (i["faction_id"] == "1") faction = "VS";
		else if (i["faction_id"] == "2") faction = "NC";
		else if (i["faction_id"] == "3") faction = "TR";
		else faction = "NS";

		// fire mode filter
		found = m_modes.end();

		for (nlohmann::json& m : i["fire_mode"])
		{
			it = std::find(m_modes.begin(), m_modes.end(), m["description"]["en"]);

			if (it < found && m["type"] == "primary")
			{
				mode = m["fire_mode_2"];
				found = it;
			}
		}

		if (found == m_modes.end())
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

		data["AngleMin"] = angleMin;
		data["AngleMax"] = angleMax;
		data["Burst"] = burst;
		data["Delay"] = delay;
		data["Factor"] = factor;
		data["Recoil"] = recoil;
		data["Speed"] = (int)((double)(60 * 1000) / speed + 0.5);

		weapons[faction][cat][name] = data;
	}

	m_weapons = weapons;
}
