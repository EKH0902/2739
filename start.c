#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <shlobj.h>
#include <stdio.h>

/*
 * start.c
 *
 * 실행 흐름 (Windows 전용):
 *   1) 관리자 권한이 아니면 UAC 승격 요청.
 *   2) "정말 실행하시겠습니까?" 1차 경고.
 *   3) "정말 실행하시겠습니까?" 2차 경고.
 *   4) reboot.exe 를 제외한 모든 프로그램을 시작프로그램에 등록.
 *   5) main.exe 실행.
 */

/* ── UAC ── */

static BOOL is_elevated(void) {
    BOOL elevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elev;
        DWORD size = sizeof(elev);
        if (GetTokenInformation(token, TokenElevation, &elev,
                                sizeof(elev), &size)) {
            elevated = elev.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return elevated;
}

static BOOL relaunch_as_admin(void) {
    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0)
        return FALSE;

    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.nShow  = SW_NORMAL;

    if (!ShellExecuteExW(&sei)) {
        MessageBoxW(NULL,
                    L"관리자 권한을 얻지 못했습니다. 프로그램을 종료합니다.",
                    L"권한 거부", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

/* ── 경고 ── */

static BOOL confirm(const wchar_t *title) {
    int r = MessageBoxW(NULL, L"정말 실행하시겠습니까?", title,
                        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
    return r == IDYES;
}

/* ── 시작프로그램 등록 (레지스트리, reboot 제외) ── */

static void get_exe_dir(wchar_t *dir, DWORD len) {
    GetModuleFileNameW(NULL, dir, len);
    wchar_t *sep = wcsrchr(dir, L'\\');
    if (sep) *(sep + 1) = L'\0';
}

static void register_startup(void) {
    HKEY hkey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0, KEY_SET_VALUE, &hkey) != ERROR_SUCCESS)
        return;

    wchar_t dir[MAX_PATH];
    get_exe_dir(dir, MAX_PATH);

    const wchar_t *exes[] = {
        L"start.exe", L"main.exe", L"effect.exe", L"effect2.exe", NULL
    };

    for (int i = 0; exes[i]; i++) {
        wchar_t path[MAX_PATH];
        swprintf(path, MAX_PATH, L"%s%s", dir, exes[i]);
        RegSetValueExW(hkey, exes[i], 0, REG_SZ,
                       (const BYTE *)path,
                       (DWORD)((wcslen(path) + 1) * sizeof(wchar_t)));
    }

    /* control.ps1 */
    wchar_t ps[MAX_PATH * 2];
    swprintf(ps, MAX_PATH * 2,
             L"powershell -WindowStyle Hidden -File \"%scontrol.ps1\"", dir);
    RegSetValueExW(hkey, L"control.ps1", 0, REG_SZ,
                   (const BYTE *)ps,
                   (DWORD)((wcslen(ps) + 1) * sizeof(wchar_t)));

    RegCloseKey(hkey);
}

/* ── main.exe 실행 ── */

static void run_main(void) {
    wchar_t dir[MAX_PATH];
    get_exe_dir(dir, MAX_PATH);

    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%smain.exe", dir);

    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi = {0};
    if (CreateProcessW(path, NULL, NULL, NULL, FALSE,
                       0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

/* ── main ── */

int main(void) {
    if (!is_elevated()) {
        if (!relaunch_as_admin())
            return 1;
        return 0;
    }

    if (!confirm(L"경고 (1/2)"))
        return 1;
    if (!confirm(L"경고 (2/2)"))
        return 1;

    register_startup();
    run_main();
    return 0;
}
