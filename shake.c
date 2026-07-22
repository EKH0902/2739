#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define SHAKE_RANGE  2
#define SHAKE_DELAY  50

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int nShow)
{
    (void)hInst; (void)hPrev; (void)cmdLine; (void)nShow;

    srand((unsigned)time(NULL));

    POINT anchor;
    GetCursorPos(&anchor);

    int prev_dx = 0, prev_dy = 0;

    for (;;) {
        POINT cur;
        GetCursorPos(&cur);

        anchor.x = cur.x - prev_dx;
        anchor.y = cur.y - prev_dy;

        int dx = (rand() % (SHAKE_RANGE * 2 + 1)) - SHAKE_RANGE;
        int dy = (rand() % (SHAKE_RANGE * 2 + 1)) - SHAKE_RANGE;

        SetCursorPos(anchor.x + dx, anchor.y + dy);
        prev_dx = dx;
        prev_dy = dy;

        Sleep(SHAKE_DELAY);
    }

    return 0;
}
