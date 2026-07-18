#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("reboot\n");
    system("shutdown /r /t 0");
    return 0;
}
