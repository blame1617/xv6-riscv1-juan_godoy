#include "kernel/types.h"
#include "user/user.h"

int main() {
    // Testear getppid()
    int pid_padre = getppid();
    printf("Mi padre tiene PID: %d\n", pid_padre);

    // Testear getancestor()
    for (int i = 0; i < 5; i++) {
        printf("Ancestro %d: %d\n", i, getancestor(i));
    }

    exit(0);
}

