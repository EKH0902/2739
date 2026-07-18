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

    wchar_t cmd[MAX_PATH];
    swprintf(cmd, MAX_PATH, L"\"%sreboot.exe\"", dir);
    _wsystem(cmd);

    return 0;
}
