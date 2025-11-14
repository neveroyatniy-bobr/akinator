#include "utils.hpp"

#include <stdio.h>

void CleanBuffer() {
    while (getchar() != '\n') { }
}