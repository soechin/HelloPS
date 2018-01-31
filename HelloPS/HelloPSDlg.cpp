#include "stdafx.h"
#include "HelloPS.h"
#include "HelloPSDlg.h"
#include "ManageDlg.h"
#include <mmsystem.h>
#include <cmath>
#include <map>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_ACTION (WM_APP + 1)
#define WM_UPDATE_ENABLED (WM_APP + 2)

BEGIN_MESSAGE_MAP(CHelloPSDlg, CDialogEx)
    ON_WM_DESTROY()
    ON_MESSAGE_VOID(WM_UPDATE_ACTION, CHelloPSDlg::OnUpdateAction)
    ON_MESSAGE_VOID(WM_UPDATE_ENABLED, CHelloPSDlg::OnUpdateEnabled)
    ON_BN_CLICKED(IDC_MANAGE_BTN, &CHelloPSDlg::OnBnClickedManageBtn)
    ON_CBN_SELCHANGE(IDC_WEAPON_LST_1, &CHelloPSDlg::OnCbnSelchangeWeaponLst1)
    ON_CBN_SELCHANGE(IDC_WEAPON_LST_2, &CHelloPSDlg::OnCbnSelchangeWeaponLst2)
    ON_CBN_SELCHANGE(IDC_SIGHT_LST_1, &CHelloPSDlg::OnCbnSelchangeSightLst1)
    ON_CBN_SELCHANGE(IDC_SIGHT_LST_2, &CHelloPSDlg::OnCbnSelchangeSightLst2)
    ON_EN_KILLFOCUS(IDC_DURATION_EDT_1, &CHelloPSDlg::OnEnKillfocusDurationEdt1)
    ON_EN_KILLFOCUS(IDC_DURATION_EDT_2, &CHelloPSDlg::OnEnKillfocusDurationEdt2)
    ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_1, &CHelloPSDlg::OnEnKillfocusSensitivityEdt1)
    ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_2, &CHelloPSDlg::OnEnKillfocusSensitivityEdt2)
    ON_EN_KILLFOCUS(IDC_SENSITIVITY_EDT_3, &CHelloPSDlg::OnEnKillfocusSensitivityEdt3)
    ON_EN_KILLFOCUS(IDC_DELAY_EDT_1, &CHelloPSDlg::OnEnKillfocusDelayEdt1)
    ON_EN_KILLFOCUS(IDC_DELAY_EDT_2, &CHelloPSDlg::OnEnKillfocusDelayEdt2)
    ON_EN_KILLFOCUS(IDC_OSD_EDT_1, &CHelloPSDlg::OnEnKillfocusOsdEdt1)
    ON_EN_KILLFOCUS(IDC_OSD_EDT_2, &CHelloPSDlg::OnEnKillfocusOsdEdt2)
END_MESSAGE_MAP()

CHelloPSDlg::CHelloPSDlg() : CDialogEx(IDD_HELLOPS_DIALOG) {
}

void CHelloPSDlg::DoDataExchange(CDataExchange *pDX) {
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
    DDX_Control(pDX, IDC_DELAY_EDT_1, m_delayEdt1);
    DDX_Control(pDX, IDC_DELAY_EDT_2, m_delayEdt2);
    DDX_Control(pDX, IDC_OSD_EDT_1, m_osdEdt1);
    DDX_Control(pDX, IDC_OSD_EDT_2, m_osdEdt2);
}

BOOL CHelloPSDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    DWORD cs, ws, wsex;

    m_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    SetIcon(m_icon, TRUE);
    SetIcon(m_icon, FALSE);

    // open database
    OpenDatabase();

    // user interface
    UIFromDatabase();

    // create timers
#ifdef OBSOLETE_TIMER
    m_timer1 = timeSetEvent(100, 1, TimerFunc1, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
    m_timer2 = timeSetEvent(1, 1, TimerFunc2, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
#else
    CreateTimerQueueTimer(&m_timer1, NULL, TimerFunc1, this, 0, 100, WT_EXECUTEDEFAULT);
    CreateTimerQueueTimer(&m_timer2, NULL, TimerFunc2, this, 0, 1, WT_EXECUTEDEFAULT);
#endif

    // states
    QueryPerformanceFrequency(&m_freq);
    QueryPerformanceCounter(&m_tick);
    m_action = 0;
    m_enabled = false;
    m_idle = false;
    m_lbutton = false;
    m_fpow = 0;
    m_fsin = 0;
    m_fcos = 0;
    m_limit = 0;
    m_remain = 0;
    m_dx = 0;
    m_dy = 0;

    // Q-button
    QueryPerformanceCounter(&m_qtick);
    m_qdown = false;

    // osd
    cs = CS_HREDRAW | CS_VREDRAW;
    ws = WS_POPUP;
    wsex = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT;

    m_osdClass = AfxRegisterWndClass(cs, NULL, NULL, NULL);
    m_osdWnd = new CWnd();
    m_osdBg = RGB(0, 0, 0);
    m_osdFg = RGB(255, 0, 0);

    m_osdWnd->CreateEx(wsex, m_osdClass, NULL, ws, 0, 0, 1, 1, NULL, NULL, NULL);
    m_osdWnd->SetLayeredWindowAttributes(m_osdBg, 0, LWA_COLORKEY);

    // weapon data
    m_speed = 0;
    m_recoil = 0;
    m_factor = 0;
    m_angleMin = 0;
    m_angleMax = 0;
    m_burst = 0;
    m_delay = 0; // first shot delay
    m_velocity = 0;
    m_gravity = 0;

    // sight data
    m_zoom = 1.0;

    // settings
    m_sensitivity = 0.3;
    m_duration1 = 0;
    m_duration2 = 1;
    m_delay1 = 0; // input delay
    m_delay2 = 0; // recovery delay
    m_vertical = 1.0;
    m_horizonal = 0.5;

    return TRUE;
}

void CHelloPSDlg::OnDestroy() {
    CDialogEx::OnDestroy();

    // delete timers
#ifdef OBSOLETE_TIMER
    timeKillEvent(m_timer1);
    timeKillEvent(m_timer2);
#else
    DeleteTimerQueueTimer(NULL, m_timer1, INVALID_HANDLE_VALUE);
    DeleteTimerQueueTimer(NULL, m_timer2, INVALID_HANDLE_VALUE);
#endif

    // osd
    m_osdWnd->DestroyWindow();
    delete m_osdWnd;

    // close database
    CloseDatabase();
}

void CHelloPSDlg::OpenDatabase() {
    CString text;
    std::string path, sql;
    std::map<std::string, double> sights;
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
        "AngleMax REAL, Burst REAL, Delay REAL, Velocity REAL, "
        "PRIMARY KEY(Name));");

    // create table "Weapons1", "Weapons2"
    m_sqlite.exec("CREATE TABLE IF NOT EXISTS Weapons1 "
        "(Name TEXT NOT NULL UNIQUE, PRIMARY KEY(Name));");
    m_sqlite.exec("CREATE TABLE IF NOT EXISTS Weapons2 "
        "(Name TEXT NOT NULL UNIQUE, PRIMARY KEY(Name));");

    // create table "Sights" with default values
    m_sqlite.exec("CREATE TABLE IF NOT EXISTS Sights "
        "(Name TEXT NOT NULL UNIQUE, Zoom REAL, PRIMARY KEY(Name));");

    sights["1.0x Hip"] = 1.0;
    sights["1.35x Aim"] = 1.35;
    sights["2.0x Aim"] = 2.0;
    sights["3.4x Scope"] = 3.4;
    sights["4.0x Scope"] = 4.0;
    sights["6.0x Scope"] = 6.0;
    sights["8.0x Scope"] = 8.0;
    sights["10.0x Scope"] = 10.0;
    sights["12.0x Scope"] = 12.0;

    stmt.prepare(&m_sqlite, "INSERT OR IGNORE INTO Sights "
        "(Name, Zoom) VALUES (@name, @zoom);");

    for (std::map<std::string, double>::iterator it = sights.begin();
        it != sights.end(); it++) {
        stmt.reset();
        stmt.bind("@name", it->first);
        stmt.bind("@zoom", it->second);
        stmt.step();
    }

    stmt.finalize();

    // create table "Settings"
    m_sqlite.exec("CREATE TABLE IF NOT EXISTS Settings "
        "(Key TEXT NOT NULL UNIQUE, Value TEXT, PRIMARY KEY(Key));");
}

void CHelloPSDlg::CloseDatabase() {
    m_sqlite.exec("VACUUM;");
    m_sqlite.close();
}

void CHelloPSDlg::UIFromDatabase() {
    CString text;
    std::string name, weapon, sight, duration, sensitivity, delay, osd;
    soechin::sqlite_stmt stmt;
    int index;

    // primary weapon
    ReadSetting("Weapon1", weapon);

    m_weaponLst1.ResetContent();
    m_weaponLst1.AddString(TEXT("Disabled"));
    index = -1;

    stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons1 "
        "ORDER BY Name");

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
        m_sightLst1.EnableWindow(TRUE);
    } else {
        m_sightLst1.EnableWindow(FALSE);
    }

    // primary sight
    ReadSetting("Sight: " + weapon, sight);

    m_sightLst1.ResetContent();
    index = -1;

    stmt.prepare(&m_sqlite, "SELECT Name FROM Sights "
        "ORDER BY Name");

    while (stmt.step()) {
        stmt.column("Name", name);

        if (index < 0 && name == sight) {
            index = m_sightLst1.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_sightLst1.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_sightLst1.SetCurSel(index);
    }

    // secondary weapon
    ReadSetting("Weapon2", weapon);

    m_weaponLst2.ResetContent();
    m_weaponLst2.AddString(TEXT("Disabled"));
    index = -1;

    stmt.prepare(&m_sqlite, "SELECT Name FROM Weapons2 "
        "ORDER BY Name");

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
        m_sightLst2.EnableWindow(TRUE);
    } else {
        m_sightLst2.EnableWindow(FALSE);
    }

    // secondary sight
    ReadSetting("Sight: " + weapon, sight);

    m_sightLst2.ResetContent();
    index = -1;

    stmt.prepare(&m_sqlite, "SELECT Name FROM Sights "
        "ORDER BY Name");

    while (stmt.step()) {
        stmt.column("Name", name);

        if (index < 0 && name == sight) {
            index = m_sightLst2.GetCount();
        }

        text = (LPTSTR)ATL::CA2T(name.c_str(), CP_UTF8);
        m_sightLst2.AddString(text);
    }

    stmt.finalize();

    if (index >= 0) {
        m_sightLst2.SetCurSel(index);
    }

    // duration
    ReadSetting("Duration1", duration);
    text = (LPTSTR)ATL::CA2T(duration.c_str(), CP_UTF8);
    m_durationEdt1.SetWindowText(text);

    ReadSetting("Duration2", duration);
    text = (LPTSTR)ATL::CA2T(duration.c_str(), CP_UTF8);
    m_durationEdt2.SetWindowText(text);

    // sensitivity
    ReadSetting("Sensitivity1", sensitivity);
    text = (LPTSTR)ATL::CA2T(sensitivity.c_str(), CP_UTF8);
    m_sensitivityEdt1.SetWindowText(text);

    ReadSetting("Sensitivity2", sensitivity);
    text = (LPTSTR)ATL::CA2T(sensitivity.c_str(), CP_UTF8);
    m_sensitivityEdt2.SetWindowText(text);

    ReadSetting("Sensitivity3", sensitivity);
    text = (LPTSTR)ATL::CA2T(sensitivity.c_str(), CP_UTF8);
    m_sensitivityEdt3.SetWindowText(text);

    // delay
    ReadSetting("Delay1", delay);
    text = (LPTSTR)ATL::CA2T(delay.c_str(), CP_UTF8);
    m_delayEdt1.SetWindowText(text);

    ReadSetting("Delay2", delay);
    text = (LPTSTR)ATL::CA2T(delay.c_str(), CP_UTF8);
    m_delayEdt2.SetWindowText(text);

    // osd
    ReadSetting("Osd1", osd);
    text = (LPTSTR)ATL::CA2T(osd.c_str(), CP_UTF8);
    m_osdEdt1.SetWindowText(text);

    ReadSetting("Osd2", osd);
    text = (LPTSTR)ATL::CA2T(osd.c_str(), CP_UTF8);
    m_osdEdt2.SetWindowText(text);
}

void CHelloPSDlg::ReadSetting(std::string key, std::string &value) {
    soechin::sqlite_stmt stmt;

    value.clear();

    if (key.empty()) {
        return;
    }

    stmt.prepare(&m_sqlite, "SELECT Value FROM Settings "
        "WHERE Key = @key;");
    stmt.bind("@key", key);

    if (stmt.step()) {
        stmt.column("Value", value);
    }

    stmt.finalize();
}

void CHelloPSDlg::WriteSetting(std::string key, std::string value) {
    soechin::sqlite_stmt stmt;

    if (key.empty()) {
        return;
    }

    stmt.prepare(&m_sqlite, "INSERT OR REPLACE INTO Settings "
        "(Key, Value) VALUES (@key, @value);");
    stmt.bind("@key", key);
    stmt.bind("@value", value);
    stmt.step();
    stmt.finalize();
}

void CHelloPSDlg::OnUpdateAction() {
    CSingleLock locker(&m_mutex, TRUE);
    CString text;
    std::string weapon, sight, category;
    soechin::sqlite_stmt stmt;

    if (m_action == 1) {
        m_weaponLst1.GetWindowText(text);
        weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

        m_sightLst1.GetWindowText(text);
        sight = (LPSTR)ATL::CT2A(text, CP_UTF8);
    } else if (m_action == 2) {
        m_weaponLst2.GetWindowText(text);
        weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

        m_sightLst2.GetWindowText(text);
        sight = (LPSTR)ATL::CT2A(text, CP_UTF8);
    }

    if (weapon.empty() || sight.empty()) {
        m_action = 0;
    }

    // weapon data
    stmt.prepare(&m_sqlite, "SELECT Category, Speed, Recoil, Factor, "
        "AngleMin, AngleMax, Burst, Delay, Velocity FROM WeaponsDB WHERE "
        "Name = @name;");
    stmt.bind("@name", weapon);

    if (stmt.step()) {
        stmt.column("Category", category);
        stmt.column("Speed", m_speed);
        stmt.column("Recoil", m_recoil);
        stmt.column("Factor", m_factor);
        stmt.column("AngleMin", m_angleMin);
        stmt.column("AngleMax", m_angleMax);
        stmt.column("Burst", m_burst);
        stmt.column("Delay", m_delay);
        stmt.column("Velocity", m_velocity);

        if (category == "Sniper Rifle") {
            m_recoil = 0;
            m_gravity = 7.5;
        } else {
            m_gravity = 0;
        }
    }

    stmt.finalize();

    // sight data
    stmt.prepare(&m_sqlite, "SELECT Zoom FROM Sights WHERE "
        "Name = @name;");
    stmt.bind("@name", sight);

    if (stmt.step()) {
        stmt.column("Zoom", m_zoom);
    }

    stmt.finalize();

    // duration
    m_durationEdt1.GetWindowText(text);
    m_duration1 = _ttof(text);

    m_durationEdt2.GetWindowText(text);
    m_duration2 = _ttof(text);

    // sensitivity
    if (m_zoom <= 1) {
        m_sensitivityEdt1.GetWindowText(text);
        m_sensitivity = _ttof(text);
    } else if (m_zoom <= 2) {
        m_sensitivityEdt2.GetWindowText(text);
        m_sensitivity = _ttof(text);
    } else {
        m_sensitivityEdt3.GetWindowText(text);
        m_sensitivity = _ttof(text);
    }

    // delay
    m_delayEdt1.GetWindowText(text);
    m_delay1 = _ttof(text);

    m_delayEdt2.GetWindowText(text);
    m_delay2 = _ttof(text);

    // osd
    m_osdEdt1.GetWindowText(text);
    m_osd1 = _ttof(text) / 2;
    m_osdEdt2.GetWindowText(text);
    m_osd2 = _ttoi(text);

    // draw osd
    DrawOSD();
}

void CHelloPSDlg::OnUpdateEnabled() {
    CSingleLock locker(&m_mutex, TRUE);

    m_durationEdt1.EnableWindow(!m_enabled);
    m_durationEdt2.EnableWindow(!m_enabled);
    m_sensitivityEdt1.EnableWindow(!m_enabled);
    m_sensitivityEdt2.EnableWindow(!m_enabled);
    m_sensitivityEdt3.EnableWindow(!m_enabled);
    m_delayEdt1.EnableWindow(!m_enabled);
    m_delayEdt2.EnableWindow(!m_enabled);
    m_osdEdt1.EnableWindow(!m_enabled);
    m_osdEdt2.EnableWindow(!m_enabled);

    OnUpdateAction();
}

void CHelloPSDlg::OnBnClickedManageBtn() {
    CManageDlg dlg(&m_sqlite);

    m_sqlite.exec("BEGIN;");

    if (dlg.DoModal() != IDOK) {
        m_sqlite.exec("ROLLBACK;");
        return;
    }

    m_sqlite.exec("COMMIT;");
    UIFromDatabase();
}

void CHelloPSDlg::OnCbnSelchangeWeaponLst1() {
    CString text;
    std::string weapon;

    m_weaponLst1.GetWindowText(text);
    weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

    WriteSetting("Weapon1", weapon);
    UIFromDatabase();
    OnUpdateAction();
}

void CHelloPSDlg::OnCbnSelchangeWeaponLst2() {
    CString text;
    std::string weapon;

    m_weaponLst2.GetWindowText(text);
    weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

    WriteSetting("Weapon2", weapon);
    UIFromDatabase();
    OnUpdateAction();
}

void CHelloPSDlg::OnCbnSelchangeSightLst1() {
    CString text;
    std::string weapon, sight;

    m_weaponLst1.GetWindowText(text);
    weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

    m_sightLst1.GetWindowText(text);
    sight = (LPSTR)ATL::CT2A(text, CP_UTF8);

    if (!weapon.empty()) {
        WriteSetting("Sight: " + weapon, sight);
    }

    OnUpdateAction();
}

void CHelloPSDlg::OnCbnSelchangeSightLst2() {
    CString text;
    std::string weapon, sight;

    m_weaponLst2.GetWindowText(text);
    weapon = (LPSTR)ATL::CT2A(text, CP_UTF8);

    m_sightLst2.GetWindowText(text);
    sight = (LPSTR)ATL::CT2A(text, CP_UTF8);

    if (!weapon.empty()) {
        WriteSetting("Sight: " + weapon, sight);
    }

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusDurationEdt1() {
    CString text;
    std::string duration;

    m_durationEdt1.GetWindowText(text);
    duration = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Duration1", duration);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusDurationEdt2() {
    CString text;
    std::string duration;

    m_durationEdt2.GetWindowText(text);
    duration = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Duration2", duration);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusSensitivityEdt1() {
    CString text;
    std::string sensitivity;

    m_sensitivityEdt1.GetWindowText(text);
    sensitivity = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Sensitivity1", sensitivity);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusSensitivityEdt2() {
    CString text;
    std::string sensitivity;

    m_sensitivityEdt2.GetWindowText(text);
    sensitivity = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Sensitivity2", sensitivity);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusSensitivityEdt3() {
    CString text;
    std::string sensitivity;

    m_sensitivityEdt3.GetWindowText(text);
    sensitivity = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Sensitivity3", sensitivity);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusDelayEdt1() {
    CString text;
    std::string delay;

    m_delayEdt1.GetWindowText(text);
    delay = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Delay1", delay);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusDelayEdt2() {
    CString text;
    std::string delay;

    m_delayEdt2.GetWindowText(text);
    delay = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Delay2", delay);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusOsdEdt1() {
    CString text;
    std::string osd;

    m_osdEdt1.GetWindowText(text);
    osd = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Osd1", osd);

    OnUpdateAction();
}

void CHelloPSDlg::OnEnKillfocusOsdEdt2() {
    CString text;
    std::string osd;

    m_osdEdt2.GetWindowText(text);
    osd = (LPSTR)ATL::CT2A(text, CP_UTF8);
    WriteSetting("Osd2", osd);

    OnUpdateAction();
}

#ifdef OBSOLETE_TIMER
void __stdcall CHelloPSDlg::TimerFunc1(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    if (dwUser != NULL) {
        ((CHelloPSDlg *)dwUser)->TimerFunc1();
    }
}

void __stdcall CHelloPSDlg::TimerFunc2(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    if (dwUser != NULL) {
        ((CHelloPSDlg *)dwUser)->TimerFunc2();
    }
}

#else
void __stdcall CHelloPSDlg::TimerFunc1(LPVOID lpParam, BOOLEAN bTimer) {
    if (lpParam != NULL && bTimer) {
        ((CHelloPSDlg *)lpParam)->TimerFunc1();
    }
}

void __stdcall CHelloPSDlg::TimerFunc2(LPVOID lpParam, BOOLEAN bTimer) {
    if (lpParam != NULL && bTimer) {
        ((CHelloPSDlg *)lpParam)->TimerFunc2();
    }
}
#endif

void CHelloPSDlg::TimerFunc1() {
    if ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0) {
        if ((GetAsyncKeyState(VK_F1) & 0x8000) != 0) { // ALT-F1
            CSingleLock locker(&m_mutex, TRUE);

            if (!m_enabled) {
                m_enabled = true;
                PostMessage(WM_UPDATE_ENABLED);
            }
        } else if ((GetAsyncKeyState(VK_F2) & 0x8000) != 0) { // ALT-F2
            CSingleLock locker(&m_mutex, TRUE);

            if (m_enabled) {
                m_enabled = false;
                PostMessage(WM_UPDATE_ENABLED);
            }
        }
    } else if ((GetAsyncKeyState(0x31) & 0x8000) != 0) { // '1'
        CSingleLock locker(&m_mutex, TRUE);

        if (m_action != 1) {
            m_action = 1;
            PostMessage(WM_UPDATE_ACTION);
        }
    } else if ((GetAsyncKeyState(0x32) & 0x8000) != 0) { // '2'
        CSingleLock locker(&m_mutex, TRUE);

        if (m_action != 2) {
            m_action = 2;
            PostMessage(WM_UPDATE_ACTION);
        }
    } else if ((GetAsyncKeyState(0xc0) & 0x8000) != 0 || // '`'
        (GetAsyncKeyState(0x33) & 0x8000) != 0 || // '3'
        (GetAsyncKeyState(0x34) & 0x8000) != 0 || // '4'
        (GetAsyncKeyState(0x35) & 0x8000) != 0) { // '5'
        CSingleLock locker(&m_mutex, TRUE);

        if (m_action != 0) {
            m_action = 0;
            PostMessage(WM_UPDATE_ACTION);
        }
    }

    // Q-button
    if (m_qdown) {
        INPUT input;

        memset(&input, 0, sizeof(input));
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 'Q';
        input.ki.wScan = MapVirtualKey('Q', MAPVK_VK_TO_VSC);
        input.ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(1, &input, sizeof(input));
        m_qdown = false;
    } else if (m_enabled && !m_idle && m_lbutton) {
        INPUT input;
        LARGE_INTEGER tick;
        double time;

        QueryPerformanceCounter(&tick);
        time = (double)(tick.QuadPart - m_qtick.QuadPart) / m_freq.QuadPart;

        if (time >= 1.0/*seconds*/) {
            memset(&input, 0, sizeof(input));
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = 'Q';
            input.ki.wScan = MapVirtualKey('Q', MAPVK_VK_TO_VSC);
            input.ki.dwFlags = 0/*KEYEVENTF_KEYDOWN*/;

            SendInput(1, &input, sizeof(input));
            m_qtick = tick;
            m_qdown = true;
        }
    }
}

void CHelloPSDlg::TimerFunc2() {
    CSingleLock locker(&m_mutex, TRUE);
    CURSORINFO cursor;
    LARGE_INTEGER tick;
    bool lbutton;
    double ifps, ishot, time, num, intp, frac, dz;
    int dx, dy, ox, oy;

    // lbutton down
    lbutton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    // first shot
    if (lbutton && !m_lbutton) {
        // idle
        m_idle = false;

        // check cursor
        memset(&cursor, 0, sizeof(cursor));
        cursor.cbSize = sizeof(cursor);

        if (GetCursorInfo(&cursor)) {
            if ((cursor.flags & CURSOR_SHOWING) != 0) {
                m_idle = true;
            }
        }

        // time begin
        QueryPerformanceCounter(&m_tick);

        // S: mouse sensivisity
        // Z: zoom level
        // Z': zoom level > 1 ? 1.6 : 1
        // A1: min recoil angle
        // A2: max recoil angle
        // F = (11.7581 / (S + 0.3)) ^ 3 * Z * Z' / 2.54 / 360
        m_fpow = pow(11.7581 / (m_sensitivity + 0.3), 3) * m_zoom *
            (m_zoom > 1 ? 1.6 : 1) / 2.54 / 360;
        // Fsin = sin((A1 + A2) / 2 * PI / 180)
        m_fsin = sin(((m_angleMin + m_angleMax) / 2) * M_PI / 180);
        // Fcos = cos(((|A1| + |A2|) / 2) * PI / 180)
        m_fcos = cos(((abs(m_angleMin) + abs(m_angleMax)) / 2) * M_PI / 180);

        // reset burst
        m_limit = 0;
        m_remain = 0;

        // reset offsets
        m_dx = 0;
        m_dy = 0;
    }

    // backup lbutton state
    m_lbutton = lbutton;

    // abort
    if (m_idle || (m_action != 1 && m_action != 2) || !m_enabled) {
        return;
    }

    // lbutton down
    if (lbutton || m_remain > 0) {
        // time end
        QueryPerformanceCounter(&tick);
        time = (double)(tick.QuadPart - m_tick.QuadPart) / m_freq.QuadPart;

        // fps interval
        ifps = 1 / m_delay1;

        // shot interval
        ishot = 60 / m_speed;

        // delay
        time = std::max(time - m_delay - ifps, 0.0);

        // time limit(for burst mode)
        if (m_burst > 0) {
            m_limit = m_burst * ishot;

            // time limit
            if (time > m_limit) {
                time = m_limit;
            }

            // time remaining
            m_remain = m_limit - time;
        }

        // number of shots
        num = time / ishot;
        frac = modf(num, &intp);

        if (m_burst <= 0) {
            if (lbutton) {
                m_limit = frac > std::min(ifps / ishot * m_delay2, 1.0) ? ceil(num) : num;
            }

            if (num > m_limit) {
                num = m_limit;
                frac = modf(num, &intp);
            }

            m_remain = m_limit - num;
        }

        // new number
        frac = (frac - m_duration1) / (m_duration2 - m_duration1);
        num = intp + std::min(std::max(frac, 0.0), 1.0);

        // distance to recoil center
        dz = (m_factor * std::min(num, 1.0) + std::max(num - 1, 0.0)) * m_recoil * m_fpow;

        // hypotenuse to opposite and adjacent
        dx = (int)(dz * m_fsin * m_horizonal);
        dy = (int)(dz * m_fcos * m_vertical);

        // move mouse
        if (m_dx != dx || m_dy != dy) {
            ox = (m_dx < dx) ? 1 : (m_dx > dx) ? -1 : 0;
            oy = (m_dy < dy) ? 1 : (m_dy > dy) ? -1 : 0;
            if (ox != 0) ox *= std::min((int)sqrt(abs(m_dx - dx)), 3);
            if (oy != 0) oy *= std::min((int)sqrt(abs(m_dy - dy)), 3);
            MoveMouse(-ox, oy);
            m_dx += ox;
            m_dy += oy;
        }
    }
}

void CHelloPSDlg::MoveMouse(int dx, int dy) {
    INPUT input;

    memset(&input, 0, sizeof(input));
    input.type = INPUT_MOUSE;
    input.mi.dx = dx;
    input.mi.dy = dy;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;

    SendInput(1, &input, sizeof(input));
}

void CHelloPSDlg::DrawOSD() {
    CSingleLock locker(&m_mutex, TRUE);
    CString text;
    CDC *dc;
    CBrush *brush, *brush0;
    CPen *pen, *pen0;
    CFont *font, *font0;
    CRect rect;
    std::map<int, int> drops;
    int cx, cy, ch, tw, th;

    if (!m_enabled || (m_action != 1 && m_action != 2) || m_gravity <= 0) {
        if (m_osdWnd->IsWindowVisible()) {
            m_osdWnd->ShowWindow(SW_HIDE);
        }

        return;
    } else if (!m_osdWnd->IsWindowVisible()) {
        m_osdWnd->ShowWindow(SW_SHOW);
    }

    // half screen size
    cx = GetSystemMetrics(SM_CXSCREEN) / 2;
    cy = GetSystemMetrics(SM_CYSCREEN) / 2;

    // dc
    dc = m_osdWnd->GetDC();
    dc->SetTextColor(m_osdFg);
    dc->SetBkColor(m_osdBg);

    // brush
    brush = new CBrush();
    brush->CreateSolidBrush(m_osdBg);
    brush0 = dc->SelectObject(brush);

    // pen
    pen = new CPen();
    pen->CreatePen(PS_SOLID, 1, m_osdFg);
    pen0 = dc->SelectObject(pen);

    // font
    font = new CFont();
    font->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
    font0 = dc->SelectObject(font);

    // window height
    ch = 0;

    // text width/height
    tw = 0;
    th = 0;

    for (int x = 50; x <= 400; x += 50) {
        double t0 = m_osd2 / m_velocity;
        double t1 = x / m_velocity;
        double y0 = m_gravity * t0 * t0 / 2;
        double y1 = m_gravity * t1 * t1 / 2;
        double y2 = tan((m_osd1 / m_zoom) * (M_PI / 180)) * x;
        int d = (int)((y1 - y0) / y2 * cy);

        drops.insert(std::make_pair(x, d));

        if (d > ch) {
            ch = d;
        }

        if ((x % 100) == 0) {
            text.Format(TEXT("%d"), x / 100);
            rect.left = 0;
            rect.top = 0;
            dc->DrawText(text, &rect, DT_LEFT | DT_TOP | DT_CALCRECT);

            if (rect.right > tw) {
                tw = rect.right;
            }

            if (rect.bottom > th) {
                th = rect.bottom;
            }
        }
    }

    // resize window
    m_osdWnd->SetWindowPos(NULL, cx, cy, 16 + tw * 2, ch + th, SWP_NOZORDER | SWP_NOACTIVATE);

    // clear
    m_osdWnd->GetClientRect(&rect);
    dc->FillRect(&rect, brush);

    for (std::map<int, int>::iterator it = drops.begin(); it != drops.end(); it++) {
        if ((it->first % 100) == 0) {
            text.Format(TEXT("%d"), it->first / 100);
            rect.left = 12 + (((it->first % 200) == 0) ? tw : 0);
            rect.top = it->second - th / 2;
            rect.right = rect.left + tw;
            rect.bottom = rect.top + th;
            dc->DrawText(text, &rect, DT_LEFT | DT_TOP);

            dc->MoveTo(3, it->second);
            dc->LineTo(10, it->second);
        } else {
            dc->MoveTo(3, it->second);
            dc->LineTo(5, it->second);
        }
    }

    // delete pen
    dc->SelectObject(pen0);
    pen->DeleteObject();
    delete pen;

    // delete brush
    dc->SelectObject(brush0);
    brush->DeleteObject();
    delete brush;

    // release dc
    m_osdWnd->ReleaseDC(dc);
}
