#include "tree.hpp"

#include <stdio.h>
#include <time.h>

int main() {
    srand((unsigned int)time(NULL));

    Tree tree;
    TreeInit(&tree);
    
    const size_t nodes_count = 100;
    for (size_t i = 0; i < nodes_count; i++) {
        TreeAdd(&tree, rand() % 1000);
    }
    
    TREE_DUMP(&tree);

    TreeDestroy(&tree);

    return 0;
}