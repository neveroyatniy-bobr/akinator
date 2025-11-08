#ifndef PROTECTED_FREE_HPP_
#define PROTECTED_FREE_HPP_

#include <stdlib.h>

#define PROTECTED_FREE(ptr) free(ptr); ptr = NULL

#endif // PROTECTED_FREE_HPP_