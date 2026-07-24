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
        MessageBoxW(NULL,
                     L"UAC \uAD8C\uD55C\uC774 \uAC70\uBD80\uB418\uC5C8\uC2B5\uB2C8\uB2E4. "
                     L"\uD504\uB85C\uADF8\uB7A8\uC744 \uC885\uB8CC\uD569\uB2C8\uB2E4.",
                     L"\uC624\uB958", MB_OK | MB_ICONERROR);
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
        LookupPrivilegeValueW(NULL, L"SeShutdownPrivilege",
                              &tp.Privileges[0].Luid);
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
            L"\uCEF4\uD4E8\uD130\uB97C \uC9C0\uAE08 \uC7AC\uBD80\uD305\uD558\uC2DC\uACA0\uC2B5\uB2C8\uAE4C?",
            L"\uC7AC\uBD80\uD305 \uD655\uC778",
            MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
        return 0;

    if (!IsElevated()) {
        RelaunchAsAdmin();
        return 0;
    }

    DoReboot();
    return 0;
}
