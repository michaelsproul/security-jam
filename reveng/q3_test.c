#include <stdio.h>
#include <string.h>

void strip_dots(char *s);

int main() {
    char * s = strdup("Hello.World.This.Is.Me");
    strip_dots(s);
    printf("%s\n", s);
    return 0;
}
