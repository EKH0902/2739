#include <windows.h>
#include <shellapi.h>

static BOOL IsElevated(void)
{
    BOOL elevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elev;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &size))
            elevated = elev.TokenIsElevated;
        CloseHandle(token);
    }
    return elevated;
}

static void RelaunchAsAdmin(void)
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);

    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.nShow  = SW_SHOWNORMAL;
    sei.fMask  = SEE_MASK_NOCLOSEPROCESS;

    if (!ShellExecuteExW(&sei)) {
        MessageBoxW(NULL, L"UAC denied. Program will exit.",
                     L"Error", MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }
    if (sei.hProcess) {
        WaitForSingleObject(sei.hProcess, INFINITE);
        CloseHandle(sei.hProcess);
    }
    ExitProcess(0);
}

static void DoReboot(void)
{
    HANDLE token;
    TOKEN_PRIVILEGES tp;
    if (OpenProcessToken(GetCurrentProcess(),
                         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        LookupPrivilegeValueW(NULL, L"SeShutdownPrivilege", &tp.Privileges[0].Luid);
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(token, FALSE, &tp, 0, NULL, NULL);
        CloseHandle(token);
    }
    ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR cmdLine, int nShow)
{
    (void)hInst; (void)hPrev; (void)cmdLine; (void)nShow;

    if (MessageBoxW(NULL,
            L"Hold on. Do you want to start this program?",
            L"Warning", MB_YESNO | MB_ICONWARNING) != IDYES)
        return 0;

    if (MessageBoxW(NULL,
            L"\uc7a0\uae50. \ub2f9\uc2e0\uc740 \uc815\ub9d0\ub85c "
            L"\uc774 \ud504\ub85c\uadf8\ub7a8\uc744 "
            L"\uc2e4\ud589\ud558\uc2dc\uaca0\uc2b5\ub2c8\uae4c?",
            L"\uacbd\uace0", MB_YESNO | MB_ICONWARNING) != IDYES)
        return 0;

    if (MessageBoxW(NULL,
            L"\uc774 \ud504\ub85c\uadf8\ub7a8\uc73c\ub85c \uc778\ud55c "
            L"\uc7ac\ubd80\ud305\uc744 \uac10\ub2f9\ud560\uc218 "
            L"\uc788\uc2b5\ub2c8\uae4c?",
            L"\ucd5c\uc885 \uacbd\uace0", MB_YESNO | MB_ICONEXCLAMATION) != IDYES)
        return 0;

    if (!IsElevated()) {
        RelaunchAsAdmin();
        return 0;
    }

    MessageBoxW(NULL, L"Ok. Let's go.",
                L"Ready", MB_OK | MB_ICONINFORMATION);

    DoReboot();
    return 0;
}
