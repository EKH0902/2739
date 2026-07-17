#include <windows.h>

static int enable_shutdown_privilege(void) {
    HANDLE token;
    TOKEN_PRIVILEGES tp;

    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return 0;
    }

    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid)) {
        CloseHandle(token);
        return 0;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(token, FALSE, &tp, 0, NULL, NULL);
    int ok = (GetLastError() == ERROR_SUCCESS);
    CloseHandle(token);
    return ok;
}

int main(void) {
    if (!enable_shutdown_privilege()) {
        return 1;
    }

    if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
                       SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
                           SHTDN_REASON_FLAG_PLANNED)) {
        return 1;
    }

    return 0;
}
