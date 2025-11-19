#ifndef PROTECTED_FREE_HPP_
#define PROTECTED_FREE_HPP_

#include <stdio.h>

#define PROTECTED_FREE(ptr) free(ptr); ptr = NULL

void CleanBuffer();

#define TO_STRING_(x) #x
#define TO_STRING(x) TO_STRING_(x)

void SkipSpaces(FILE* stream);

size_t FileSize(FILE* file);

#endif // PROTECTED_FREE_HPP_