#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>

int main(void) {
    printf("Hello, world!\n");

    wchar_t dir[MAX_PATH];
    GetModuleFileNameW(NULL, dir, MAX_PATH);
    wchar_t *sep = wcsrchr(dir, L'\\');
    if (sep) *(sep + 1) = L'\0';

    wchar_t path[MAX_PATH];
    swprintf(path, MAX_PATH, L"%sreboot.exe", dir);

    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi = {0};
    if (CreateProcessW(path, NULL, NULL, NULL, FALSE,
                       0, NULL, dir, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return 0;
}
