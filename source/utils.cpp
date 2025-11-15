#include "utils.hpp"

#include <stdio.h>
#include <ctype.h>

void CleanBuffer() {
    while (getchar() != '\n') { }
}

void SkipSpaces(FILE* stream) {
    while (isspace(getc(stream))) {}
    fseek(stream, -1, SEEK_CUR);    
}