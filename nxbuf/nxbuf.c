#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// non-executable stack, defeat ASLR with provided bufaddr

int main(int argc, char ** argv) {
    setbuf(stdout, NULL);
    char buf[9447 - 12];
    printf(">> well done, but this one is trickier! <<\n");
    printf("bufaddr: %p\n", buf);
    scanf("%s", buf);
    printf("buf: '%s'\n", buf);
    return 0;
}
