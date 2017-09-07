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
	ON_CBN_DROPDOWN(IDC_WEAPON_LST_1, &CHelloPSDlg::OnCbnDropdownWeaponLst1)
	ON_CBN_SELCHANGE(IDC_WEAPON_LST_1, &CHelloPSDlg::OnCbnSelchangeWeaponLst1)
END_MESSAGE_MAP()

CHelloPSDlg::CHelloPSDlg() : CDialogEx(IDD_HELLOPS_DIALOG)
{
}

void CHelloPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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

	// path
	if (m_path.IsEmpty())
	{
		AfxGetModuleFileName(AfxGetInstanceHandle(), m_path);
		PathRenameExtension(m_path.GetBuffer(m_path.GetLength() + MAX_PATH), TEXT(".json"));
		m_path.ReleaseBuffer();
	}

	// open, read, and close
	ifs.open(m_path);

	if (ifs.is_open())
	{
		m_json << ifs;
		ifs.close();
	}

	// default: category filters
	if (!m_json["Categories"].is_array())
	{
		m_json["Categories"] =
		{
			"Assault Rifle", "Battle Rifle", "Carbine", "LMG", "Pistol",
			"SMG", "Scout Rifle", "Sniper Rifle"
		};
	}

	// default: factions
	if (!m_json["Factions"].is_array())
	{
		m_json["Factions"] =
		{
			"NC", "NS", "TR", "VS"
		};
	}

	// default: fire mode filters
	if (!m_json["Modes"].is_array())
	{
		m_json["Modes"] =
		{
			"Auto", "3x Burst", "2x Burst", "Semi-Auto"
		};
	}

	//// default: empty weapons
	//if (!m_json["Weapons"].is_object())
	//{
	//	m_json["Weapons"] = nlohmann::json::object();
	//}

	//if (!m_json["Weapons1"].is_array())
	//{
	//	m_json["Weapons1"] = nlohmann::json::array();
	//}

	//if (!m_json["Weapons2"].is_array())
	//{
	//	m_json["Weapons2"] = nlohmann::json::array();
	//}
}

void CHelloPSDlg::WriteSettingFile()
{
	std::ofstream ofs;

	// open, write, and close
	ofs.open(m_path);

	if (ofs.is_open())
	{
		ofs << std::setw(4) << m_json;
		ofs.close();
	}
}

void CHelloPSDlg::OnBnClickedManageBtn()
{
	CManageDlg dlg;

	dlg.SetJson(m_json);
	
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetJson(m_json);
	}
}

void CHelloPSDlg::OnCbnDropdownWeaponLst1()
{
	//CString text;
	//std::string selText;
	//int selIndex;

	//m_weaponLst1.GetWindowText(text);
	//selText = (LPSTR)ATL::CT2A(text, CP_UTF8);

	//m_weaponLst1.ResetContent();
	//selIndex = -1;

	//for (std::string name : m_weapons1)
	//{
	//	if (selIndex < 0 && name == selText)
	//	{
	//		selIndex = m_weaponLst1.GetCount();
	//	}

	//	text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
	//	m_weaponLst1.AddString(text);
	//}

	//if (selIndex >= 0)
	//{
	//	m_weaponLst1.SetCurSel(selIndex);
	//}
}

void CHelloPSDlg::OnCbnSelchangeWeaponLst1()
{
}
