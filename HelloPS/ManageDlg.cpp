#include "stdafx.h"
#include "HelloPS.h"
#include "ManageDlg.h"
// JSON for Modern C++
#include "json.hpp"
#include <fstream>
#include <afxinet.h>

IMPLEMENT_DYNAMIC(CManageDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CManageDlg, CDialogEx)
    ON_BN_CLICKED(IDC_PRIMARY_RAD, &CManageDlg::OnBnClickedPrimaryRad)
    ON_BN_CLICKED(IDC_SECONDARY_RAD, &CManageDlg::OnBnClickedSecondaryRad)
    ON_BN_CLICKED(IDC_INSERT_BTN, &CManageDlg::OnBnClickedInsertBtn)
    ON_BN_CLICKED(IDC_REMOVE_BTN, &CManageDlg::OnBnClickedRemoveBtn)
    ON_BN_CLICKED(IDC_IMPORT_BTN, &CManageDlg::OnBnClickedImportBtn)
    ON_CBN_SELCHANGE(IDC_FACTION_LST, &CManageDlg::OnCbnSelchangeFactionLst)
    ON_CBN_SELCHANGE(IDC_CATEGORY_LST, &CManageDlg::OnCbnSelchangeCategoryLst)
END_MESSAGE_MAP()

std::string json_string(const nlohmann::json &node) {
    std::string str;
    int left, right;

    if (!node.is_string()) {
        return str;
    }

    str = node.get<std::string>();
    left = -1;
    right = str.length();

    for (int i = 0; i < (int)str.length(); i++) {
        if (isspace(str[i])) {
            left = i;
        } else {
            break;
        }
    }

    for (int i = (int)str.length() - 1; i > left; i--) {
        if (isspace(str[i])) {
            right = i;
        } else {
            break;
        }
    }

    return str.substr(left + 1, right - left - 1);
}

int json_integer(const nlohmann::json &node) {
    return atoi(json_string(node).c_str());
}

double json_double(const nlohmann::json &node) {
    return atof(json_string(node).c_str());
}

CManageDlg::CManageDlg(soechin::sqlite *sqlite) : CDialogEx(IDD_MANAGE_DIALOG) {
    m_sqlite = sqlite;
}

void CManageDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRIMARY_RAD, m_primaryRad);
    DDX_Control(pDX, IDC_SECONDARY_RAD, m_secondaryRad);
    DDX_Control(pDX, IDC_INSERT_BTN, m_insertBtn);
    DDX_Control(pDX, IDC_REMOVE_BTN, m_removeBtn);
    DDX_Control(pDX, IDC_IMPORT_BTN, m_importBtn);
    DDX_Control(pDX, IDC_FACTION_LST, m_factionLst);
    DDX_Control(pDX, IDC_CATEGORY_LST, m_categoryLst);
    DDX_Control(pDX, IDC_WEAPON_LST_1, m_weaponLst1);
    DDX_Control(pDX, IDC_WEAPON_LST_2, m_weaponLst2);
    DDX_Control(pDX, IDC_PROGRESS_LBL, m_progressLbl);
}

BOOL CManageDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // user interface
    UIFromDatabase();

    return TRUE;
}

void CManageDlg::UIFromDatabase() {
    CString text;
    std::string action, name, faction, category;
    soechin::sqlite_stmt stmt;
    int index;

    // action
    ReadSetting("Action", action);

    m_secondaryRad.SetCheck(BST_UNCHECKED);
    m_primaryRad.SetCheck(BST_UNCHECKED);

    if (action == "1") {
        m_primaryRad.SetCheck(BST_CHECKED);
    } else if (action == "2") {
        m_secondaryRad.SetCheck(BST_CHECKED);
    }

    // faction
    ReadSetting("Faction", faction);

    m_factionLst.ResetContent();
    index = -1;

    stmt.prepare(m_sqlite, "SELECT DISTINCT Faction FROM WeaponsDB "
        "ORDER BY Faction;");

    while (stmt.step()) {
        stmt.column("Faction", name);

        if (index < 0 && name == faction) {
            index = m_factionLst.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_factionLst.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_factionLst.SetCurSel(index);
    }

    // category
    ReadSetting("Category", category);

    m_categoryLst.ResetContent();
    index = -1;

    stmt.prepare(m_sqlite, "SELECT DISTINCT Category FROM WeaponsDB "
        "ORDER BY Category;");

    while (stmt.step()) {
        stmt.column("Category", name);

        if (index < 0 && name == category) {
            index = m_categoryLst.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_categoryLst.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_categoryLst.SetCurSel(index);
    }

    // reload
    ReloadWeaponLst1();
    ReloadWeaponLst2();
}

void CManageDlg::ReadSetting(std::string key, std::string &value) {
    soechin::sqlite_stmt stmt;

    value.clear();

    if (key.empty()) {
        return;
    }

    stmt.prepare(m_sqlite, "SELECT Value FROM Settings "
        "WHERE Key = @key;");
    stmt.bind("@key", key);

    if (stmt.step()) {
        stmt.column("Value", value);
    }

    stmt.finalize();
}

void CManageDlg::WriteSetting(std::string key, std::string value) {
    soechin::sqlite_stmt stmt;

    if (key.empty()) {
        return;
    }

    stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO Settings "
        "(Key, Value) VALUES (@key, @value);");
    stmt.bind("@key", key);
    stmt.bind("@value", value);
    stmt.step();
    stmt.finalize();
}

void CManageDlg::ReloadWeaponLst1() {
    CString text;
    std::string name, weapon;
    soechin::sqlite_stmt stmt;
    int index;

    if ((index = m_weaponLst1.GetCurSel()) >= 0) {
        m_weaponLst1.GetText(index, text);
        weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);
    }

    m_weaponLst1.ResetContent();
    index = -1;

    if (m_primaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "SELECT Name FROM Weapons1 "
            "ORDER BY Name");
    } else if (m_secondaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "SELECT Name FROM Weapons2 "
            "ORDER BY Name");
    } else {
        return;
    }

    while (stmt.step()) {
        stmt.column("Name", name);

        if (index < 0 && name == weapon) {
            index = m_weaponLst1.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_weaponLst1.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_weaponLst1.SetCurSel(index);
    }
}

void CManageDlg::ReloadWeaponLst2() {
    CString text;
    std::string name, weapon, faction, category;
    soechin::sqlite_stmt stmt;
    int index;

    m_factionLst.GetWindowText(text);
    faction = (LPSTR)ATL::CT2A(text, CP_UTF8);

    m_categoryLst.GetWindowText(text);
    category = (LPSTR)ATL::CT2A(text, CP_UTF8);

    if ((index = m_weaponLst2.GetCurSel()) >= 0) {
        m_weaponLst2.GetText(index, text);
        weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);
    }

    m_weaponLst2.ResetContent();
    index = -1;

    stmt.prepare(m_sqlite, "SELECT Name FROM WeaponsDB WHERE "
        "Faction = @faction AND Category = @category ORDER BY Name;");
    stmt.bind("@faction", faction);
    stmt.bind("@category", category);

    while (stmt.step()) {
        stmt.column("Name", name);

        if (index < 0 && name == weapon) {
            index = m_weaponLst2.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_weaponLst2.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_weaponLst2.SetCurSel(index);
    }
}

void CManageDlg::OnBnClickedPrimaryRad() {
    WriteSetting("Action", "1");
    ReloadWeaponLst1();
}

void CManageDlg::OnBnClickedSecondaryRad() {
    WriteSetting("Action", "2");
    ReloadWeaponLst1();
}

void CManageDlg::OnBnClickedInsertBtn() {
    CString text;
    soechin::sqlite_stmt stmt;
    std::string name;
    int selIndex;

    if ((selIndex = m_weaponLst2.GetCurSel()) >= 0) {
        m_weaponLst2.GetText(selIndex, text);
        name = (LPSTR)ATL::CT2A(text, CP_UTF8);
    }

    if (name.empty()) {
        return;
    }

    if (m_primaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO Weapons1 "
            "(Name) VALUES (@name);");
    } else if (m_secondaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO Weapons2 "
            "(Name) VALUES (@name);");
    } else {
        return;
    }

    stmt.bind("@name", name);
    stmt.step();
    stmt.finalize();

    ReloadWeaponLst1();
}

void CManageDlg::OnBnClickedRemoveBtn() {
    CString text;
    soechin::sqlite_stmt stmt;
    std::string name;
    int selIndex;

    if ((selIndex = m_weaponLst1.GetCurSel()) >= 0) {
        m_weaponLst1.GetText(selIndex, text);
        name = (LPSTR)ATL::CT2A(text, CP_UTF8);
    }

    if (name.empty()) {
        return;
    }

    if (m_primaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "DELETE FROM Weapons1 WHERE "
            "Name = @name;");
    } else if (m_secondaryRad.GetCheck() == BST_CHECKED) {
        stmt.prepare(m_sqlite, "DELETE FROM Weapons2 WHERE "
            "Name = @name;");
    } else {
        return;
    }

    stmt.bind("@name", name);
    stmt.step();
    stmt.finalize();

    ReloadWeaponLst1();
}

void CManageDlg::OnBnClickedImportBtn() {
    CString file, text;
    std::ifstream ifs;
    std::string name, category, faction;
    std::vector<std::string> categories, factions, modes;
    nlohmann::json json, item, mode, modej;
    soechin::sqlite_stmt stmt;
    double recoil, factor, angleMin, angleMax, velocity;
    int found, burst, speed;

    if (DownloadWeaponData(file)) {
        // open, read, and close
        ifs.open(file);

        if (ifs.is_open()) {
            json << ifs;
            ifs.close();
        }

        DeleteFile(file);
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
    categories.push_back("Lightning Primary Weapon");

    factions.push_back("NS");
    factions.push_back("VS");
    factions.push_back("NC");
    factions.push_back("TR");

    modes.push_back("Auto");
    modes.push_back("3x Burst");
    modes.push_back("2x Burst");
    modes.push_back("Semi-Auto");
    modes.push_back("Bolt Action");

    // prepare to insert
    stmt.prepare(m_sqlite, "INSERT OR REPLACE INTO WeaponsDB "
        "(Name, Faction, Category, Speed, Recoil, Factor, "
        "AngleMin, AngleMax, Burst, Velocity) VALUES "
        "(@name, @faction, @category, @speed, @recoil, @factor, "
        "@angleMin, @angleMax, @burst, @velocity);");

    for (int i = 0; i < (int)json["item_list"].size(); i++) {
        item = json["item_list"][i];

        // name, category
        name = json_string(item["name"]["en"]);
        category = json_string(item["item_category"]["name"]["en"]);

        // category filter
        found = (int)categories.size();

        for (int j = 0; j < (int)categories.size(); j++) {
            if (categories[j] == category) {
                found = j;
                break;
            }
        }

        if (found >= (int)categories.size()) {
            continue;
        }

        // faction
        if (item["faction_id"] == "1") faction = factions[1];
        else if (item["faction_id"] == "2") faction = factions[2];
        else if (item["faction_id"] == "3") faction = factions[3];
        else faction = factions[0];

        // fire mode filter
        found = (int)modes.size();

        for (int j = 0; j < (int)item["fire_mode"].size(); j++) {
            modej = item["fire_mode"][j];

            if (modej["type"] != "primary") {
                continue;
            }

            for (int k = 0; k < found; k++) {
                if (modej["description"]["en"] == modes[k]) {
                    mode = modej["fire_mode_2"];
                    found = k;
                    break;
                }
            }
        }

        if (found >= (int)modes.size()) {
            continue;
        }

        // burst count
        burst = json_integer(mode["fire_burst_count"]);

        // auto, semi-auto
        if (burst <= 1) {
            speed = json_integer(mode["fire_refire_ms"]);
            if (speed == 0) {
                speed = 1000;
            }

            if (json_integer(mode["automatic"]) != 0) {
                burst = 0;
            }
        }
        // 2x burst, 3x burst
        else {
            speed = json_integer(mode["fire_auto_fire_ms"]);
        }

        // recoil, first shot multiplier, angle
        recoil = json_double(mode["recoil_magnitude_min"]);
        factor = json_double(mode["recoil_first_shot_modifier"]);
        angleMin = json_double(mode["recoil_angle_min"]);
        angleMax = json_double(mode["recoil_angle_max"]);

        // velocity
        velocity = json_double(mode["projectile_speed_override"]);
        if (velocity == 0) {
            velocity = json_double(modej["muzzle_velocity"]);
        }
        if (velocity == 0) {
            velocity = json_double(modej["speed"]);
        }

        // insert into database
        stmt.reset();
        stmt.bind("@name", name);
        stmt.bind("@faction", faction);
        stmt.bind("@category", category);
        stmt.bind("@speed", (int)((double)(60 * 1000) / speed + 0.5)); // ms -> rpm
        stmt.bind("@recoil", recoil);
        stmt.bind("@factor", factor);
        stmt.bind("@angleMin", angleMin);
        stmt.bind("@angleMax", angleMax);
        stmt.bind("@burst", burst);
        stmt.bind("@velocity", velocity);
        stmt.step();

        text.Format(TEXT("%d"), i);
        m_progressLbl.SetWindowText(text);
    }

    m_progressLbl.SetWindowText(TEXT("DONE"));

    // cleanup
    stmt.finalize();

    UIFromDatabase();
}

void CManageDlg::OnCbnSelchangeFactionLst() {
    CString text;
    std::string faction;

    m_factionLst.GetWindowText(text);
    faction = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Faction", faction);

    ReloadWeaponLst2();
}

void CManageDlg::OnCbnSelchangeCategoryLst() {
    CString text;
    std::string category;

    m_categoryLst.GetWindowText(text);
    category = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Category", category);

    ReloadWeaponLst2();
}

bool CManageDlg::DownloadWeaponData(CString &jsonFile) {
    CAutoPtr<CInternetSession> session;
    CAutoPtr<CStdioFile> in, out;
    CString text;
    TCHAR tempDir[MAX_PATH], tempFile[MAX_PATH];
    BYTE buf[1024];
    UINT len;
    ULONGLONG  bytes;

    try {
        session.Attach(new CInternetSession());
        in.Attach(session->OpenURL(TEXT("https://www.planetstats.net/data/weapons.json")));
    } catch (CInternetException *e) {
        e->ReportError();
        return false;
    }

    GetTempPath(_countof(tempDir), tempDir);
    GetTempFileName(tempDir, NULL, 0, tempFile);
    out.Attach(new CStdioFile(tempFile, CFile::modeWrite));

    bytes = 0;

    while ((len = in->Read(buf, _countof(buf))) > 0) {
        out->Write(buf, len);
        bytes += len;

        text.Format(TEXT("%llu"), bytes);
        m_progressLbl.SetWindowText(text);
    }

    m_progressLbl.SetWindowText(TEXT("DONE"));

    in->Close();
    out->Close();

    jsonFile = tempFile;
    return true;
}
