#include <stdio.h>

/*
 * start.c
 *
 * 즉시 실행 (UAC 승격/경고 없음).
 */

static void start(void) {
    printf("start\n");
}

int main(void) {
    start();
    return 0;
}
