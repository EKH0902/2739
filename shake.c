#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define SHAKE_RANGE  2
#define SHAKE_DELAY  50

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int nShow)
{
    (void)hInst; (void)hPrev; (void)cmdLine; (void)nShow;

    srand((unsigned)time(NULL));

    for (;;) {
        POINT pt;
        GetCursorPos(&pt);

        int dx = (rand() % (SHAKE_RANGE * 2 + 1)) - SHAKE_RANGE;
        int dy = (rand() % (SHAKE_RANGE * 2 + 1)) - SHAKE_RANGE;

        SetCursorPos(pt.x + dx, pt.y + dy);
        Sleep(SHAKE_DELAY);
    }

    return 0;
}
