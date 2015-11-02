#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// executable stack, defeat ASLR with provided bufaddr

int main(int argc, char ** argv) {
    setbuf(stdout, NULL);
    char buf[9447 - 12];
    printf(">> this one should be easy ;) <<\n");
    printf("bufaddr: %p\n", buf);
    scanf("%s", buf);
    printf("buf: '%s'\n", buf);
    return 0;
}
