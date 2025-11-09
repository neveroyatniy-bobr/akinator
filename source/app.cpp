#include "app.hpp"
#include <stdio.h>

#include "akinator.hpp"
#include "tree.hpp"

AkinatorError AkinatorApp() {
    Tree akinator_tree = {};

    AkinatorError init_error = AkinatorTreeInit(&akinator_tree);
    if (init_error != AKINATOR_OK) {
        AKINATOR_PRINT_ERROR(init_error);
        TREE_DUMP(&akinator_tree);
        return init_error;
    }

    bool run = true;

    while (run) {
        printf("Что вы хотите сделать?\n");
        printf("1) Поиграть\n");
        printf("2) Найти\n");
        printf("3) Сравнить\n");
        printf("4) Выйти\n");


        int mode_num = 0;
        scanf("%d", &mode_num);

        AkinatorAppMode mode = (AkinatorAppMode)(mode_num - 1);

        AkinatorError mode_error = AKINATOR_OK;

        switch (mode) {
            case PLAY: 
                mode_error = AkinatorRequest(&akinator_tree);
                break;
            case FIND:
                mode_error = AkinatorFind(&akinator_tree);
                break;
            case COMPARE:
                mode_error = AkinatorCompare(&akinator_tree);
                break;
            case QUIT:
                run = false;
                break;
            default:
                printf("Неправильная команда\n");
                break;
        }

        if (mode_error != AKINATOR_OK) {
            AkinatorTreeDestroy(&akinator_tree);
            return mode_error;
        }
    }

    AkinatorTreeDestroy(&akinator_tree);

    return AKINATOR_OK;
}

