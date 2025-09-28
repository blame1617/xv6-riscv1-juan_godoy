#include "kernel/types.h"
#include "user/user.h"

int main() {
    // Si tienes la syscall getancestor implementada
    for (int i = 0; i < 5; i++) {
        printf("Ancestro %d: %d\n", i, getancestor(i));
    }

    exit(0);
}
