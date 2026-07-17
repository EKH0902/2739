#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <stdio.h>

/*
 * unify.c
 *
 * 저장소의 모든 프로그램(main / effect / effect2 / start / control)을 하나로
 * 통합한 단일 실행 파일. reboot 는 제외한다.
 *
 * 실행 흐름 (Windows 전용):
 *   1) 관리자 권한이 아니면, 요청 권한 목록을 안내한 뒤 UAC 승격을 요청하며
 *      자기 자신을 관리자 권한으로 다시 실행한다.
 *   2) 승격된 상태에서 요청 권한을 토큰에 활성화(가능한 것)한다.
 *   3) "정말 실행하시겠습니까?" 1차 경고.
 *   4) "정말 실행하시겠습니까?" 2차 경고.
 *   5) 모든 구성 프로그램의 동작을 순차 실행한다.
 */

/* UAC 안내에 표시할 요청 권한 목록 */
static const wchar_t *REQUESTED_PRIVILEGES =
    L"이 프로그램은 다음의 높은 등급 권한을 요청합니다:\n\n"
    L"  \x2022 디스크 로우 레벨 쓰기 권한 (SeRawDiskReadWrite)\n"
    L"  \x2022 레지스트리 및 커널 하이브 수정 권한\n"
    L"  \x2022 시스템 중요 프로세스 격상 권한 (SeDebugPrivilege)\n"
    L"  \x2022 보안 계정 관리자(SAM) 및 사용자 계정 통제 권한\n\n"
    L"계속하려면 관리자 권한(UAC)을 허용해야 합니다.";

/* 각 구성 프로그램의 동작 (reboot 제외) */
static void prog_main(void)    { printf("[main] Hello, world!\n"); }
static void prog_effect(void)  { printf("[effect] effect\n"); }
static void prog_effect2(void) { printf("[effect2] effect2\n"); }
static void prog_start(void)   { printf("[start] start\n"); }
static void prog_control(void) { printf("[control] control\n"); }

static void run_all(void) {
    prog_main();
    prog_effect();
    prog_effect2();
    prog_start();
    prog_control();
}

/* 현재 프로세스가 관리자(승격) 권한인지 확인 */
static BOOL is_elevated(void) {
    BOOL elevated = FALSE;
    HANDLE token = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(elevation);
        if (GetTokenInformation(token, TokenElevation, &elevation,
                                sizeof(elevation), &size)) {
            elevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return elevated;
}

/* UAC 안내 후, 관리자 권한으로 자기 자신을 다시 실행 */
static BOOL relaunch_as_admin(void) {
    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0) {
        return FALSE;
    }

    MessageBoxW(NULL, REQUESTED_PRIVILEGES, L"권한 요청",
                MB_OK | MB_ICONWARNING);

    SHELLEXECUTEINFOW sei = {0};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas"; /* UAC 승격 트리거 */
    sei.lpFile = path;
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteExW(&sei)) {
        MessageBoxW(NULL, L"관리자 권한을 얻지 못했습니다. 프로그램을 종료합니다.",
                    L"권한 거부", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

/* 승격 후 토큰에 이름 있는 권한을 활성화 (실제 존재하는 권한만 적용됨) */
static void enable_privilege(const wchar_t *name) {
    HANDLE token = NULL;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return;
    }

    TOKEN_PRIVILEGES tp;
    if (LookupPrivilegeValueW(NULL, name, &tp.Privileges[0].Luid)) {
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL);
    }
    CloseHandle(token);
}

static void enable_requested_privileges(void) {
    /* Windows에 실제 정의된 권한만 활성화된다. */
    enable_privilege(SE_DEBUG_NAME);           /* SeDebugPrivilege */
    enable_privilege(SE_BACKUP_NAME);          /* 로우 레벨 읽기(백업) */
    enable_privilege(SE_RESTORE_NAME);         /* 로우 레벨 쓰기(복원) */
    enable_privilege(SE_TAKE_OWNERSHIP_NAME);  /* SAM 등 개체 소유권 */
}

/* Yes/No 경고. Yes면 TRUE */
static BOOL confirm(const wchar_t *title) {
    int r = MessageBoxW(NULL, L"정말 실행하시겠습니까?", title,
                        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
    return r == IDYES;
}

int main(void) {
    if (!is_elevated()) {
        if (!relaunch_as_admin()) {
            return 1;
        }
        /* 승격된 새 프로세스가 이어서 진행하므로 현재 프로세스는 종료 */
        return 0;
    }

    enable_requested_privileges();

    if (!confirm(L"경고 (1/2)")) {
        return 1;
    }
    if (!confirm(L"경고 (2/2)")) {
        return 1;
    }

    run_all();
    return 0;
}
