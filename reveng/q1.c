int readBanner(char * s) {
    FILE * file = fopen("./banner.txt", "wb");
    if (file == NULL) {
        exit(0);
    }
    s[20] = '\0';
    fputs(s, stream);
    return fclose(stream);
}
