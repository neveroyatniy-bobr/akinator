#include "utils.hpp"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

void CleanBuffer() {
    while (getchar() != '\n') { }
}

void SkipSpaces(FILE* stream) {
    while (isspace(getc(stream))) {}
    fseek(stream, -1, SEEK_CUR);    
}

size_t FileSize(FILE* file) {
    struct stat stats = {};
    fstat(fileno(file), &stats);

    return stats.st_size;
}