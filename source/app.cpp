#include "app.hpp"
#include <stdio.h>

#include "akinator.hpp"
#include "tree.hpp"
#include "utils.hpp"

AkinatorError AkinatorApp(int argc, const char** argv) {
    Tree akinator_tree = {};

    bool is_fast_load = false;
    char database_file_name[MAX_FILE_NAME_LEN + 1] = {};
    if (argc == 3) {
        if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "-file") == 0) {
            is_fast_load = true;
            strncpy(database_file_name, argv[2], MAX_FILE_NAME_LEN);
        }
    }

    AkinatorError init_error = AkinatorTreeInit(&akinator_tree);
    if (init_error != AKINATOR_OK) {
        AKINATOR_PRINT_ERROR(init_error);
        TREE_DUMP(&akinator_tree);
        return init_error;
    }

    AkinatorError load_err = AkinatorTreeLoad(&akinator_tree, is_fast_load, database_file_name);
    if (load_err != AKINATOR_OK) {
        AKINATOR_PRINT_ERROR(init_error);
        TREE_DUMP(&akinator_tree);
        return load_err;
    }

    TREE_DUMP(&akinator_tree);

    bool run = true;

    while (run) {
        AkinatorPrintf("Что вы хотите сделать?\n");
        AkinatorPrintf("1) Поиграть\n");
        AkinatorPrintf("2) Найти\n");
        AkinatorPrintf("3) Сравнить\n");
        AkinatorPrintf("4) Сохранить\n");
        AkinatorPrintf("5) Загрузить\n");
        AkinatorPrintf("6) Выйти\n");


        int mode_num = 0;
        scanf("%d", &mode_num);
        CleanBuffer();

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
            case SAVE:
                mode_error = AkinatorTreeSave(&akinator_tree);
                break;
            case LOAD:
                mode_error = AkinatorTreeLoad(&akinator_tree, false, NULL);
                break;
            case QUIT:
                run = false;
                break;
            default:
                AkinatorPrintf("Неправильная команда\n");
                break;
        }

        if (mode_error != AKINATOR_OK) {
            AkinatorTreeDestroy(&akinator_tree);
            return mode_error;
        }
    }

    AkinatorTreeSave(&akinator_tree);

    AkinatorTreeDestroy(&akinator_tree);

    return AKINATOR_OK;
}

