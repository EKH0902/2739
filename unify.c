#include <stdio.h>
#include <stdlib.h>

/*
 * unify.c
 *
 * 저장소의 모든 프로그램(main / effect / effect2 / start / control / reboot)을
 * 하나로 통합한 단일 실행 파일.
 *
 * UAC 승격이나 경고 대화상자 없이 즉시 실행된다.
 */

static void prog_main(void)    { printf("[main] Hello, world!\n"); }
static void prog_effect(void)  { printf("[effect] effect\n"); }
static void prog_effect2(void) { printf("[effect2] effect2\n"); }
static void prog_start(void)   { printf("[start] start\n"); }
static void prog_control(void) { printf("[control] control\n"); }
static void prog_reboot(void)  { printf("[reboot] reboot\n"); system("shutdown /r /t 0"); }

static void run_all(void) {
    prog_main();
    prog_effect();
    prog_effect2();
    prog_start();
    prog_control();
    prog_reboot();
}

int main(void) {
    run_all();
    return 0;
}
