#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

/*
 * effect.c
 *
 * 재부팅 후 로그인 시 환영 메시지를 순서대로 표시한다.
 */

int main(void) {
    MessageBoxW(NULL,
                L"돌아온것을 환영해",
                L"우가바이러스",
                MB_OK | MB_ICONINFORMATION);

    MessageBoxW(NULL,
                L"내 이름은 우가바이러스야.",
                L"우가바이러스",
                MB_OK | MB_ICONINFORMATION);

    MessageBoxW(NULL,
                L"지금부터 즐겨볼까?",
                L"우가바이러스",
                MB_OK | MB_ICONWARNING);

    return 0;
}
