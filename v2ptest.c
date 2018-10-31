#include "types.h"
#include "user.h"

void test(void);

int j = 0;

int
main(int argc, char *argv[])
{
    int i = 0;
    int beep = 0x8000000;

    printf(1, "instruction: virtual: %p, physical %x\n", (void*)&test, v2p((int)&test));
    printf(1, "local: virtual: %p, physical: %x\n", (void*)&i, v2p((int)&i));
    printf(1, "global: virtual: %p, physical: %x\n", (void*)&j, v2p((int)&j));
    printf(1, "invalid: virtual: %x, physical: %x\n", beep, v2p(beep));
    exit();
}

void
test()
{
    printf(1, "Hello world");
}