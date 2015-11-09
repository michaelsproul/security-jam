#include <stdlib.h>
#include <string.h>

// Strip full-stop characters from a string, writing back to the original string.
void strip_dots(char * s) {
    char * start = s + 1;
    char * end = s;

    while (*end != '\0') {
        end++;
    }

    // Length of the string, not including null byte.
    size_t len = end - start;

    char * buffer = malloc(len);
    memset(buffer, 0, len);


    // Copy all non '.' characters to the buffer.
    int buf_len = 0;

    for (int i = 0; i < len; i++) {
        if (s[i] == '.') {
            continue;
        }

        buffer[buf_len] = s[i];
        buf_len++;
    }

    // Write the buffer back to the string.
    char * buf_ptr = buffer;
    char * s_ptr = s;

    while (*buf_ptr != '\0') {
        *s_ptr = *buf_ptr;
        buf_ptr++;
        s_ptr++;
    }

    s[buf_len] = '\0';
    free(buffer);

    return;
}
