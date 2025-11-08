#include "tree.hpp"
#include "akinator.hpp"

#include <stdio.h>
#include <time.h>

int main() {
    Tree akinator_tree = {};

    AkinatorError init_error = AkinatorTreeInit(&akinator_tree);
    if (init_error != AKINATOR_OK) {
        AKINATOR_PRINT_ERROR(init_error);
        TREE_DUMP(&akinator_tree);
        return 1;
    }

    AkinatorGetDefine(&akinator_tree, "илья д.");
    AkinatorCompare(&akinator_tree, "илья д.", "машануладно");

    AkinatorError request_error = AkinatorRequest(&akinator_tree);
    if (request_error != AKINATOR_OK) {
        AKINATOR_PRINT_ERROR(request_error);
        TREE_DUMP(&akinator_tree);
        return 1;
    }

    AkinatorTreeDestroy(&akinator_tree);

    return 0;
}