#include "akinator.hpp"

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "utils.hpp"

const char* AkinatorStrError(AkinatorError error) {
    switch (error) {
        case AKINATOR_OK:
            return "Выполнено без ошибок";
        case AKINATOR_TREE_ERROR:
            return "Ошибка в дереве акинатора";
        case AKINATOR_NODE_ALLOC_ERROR:
            return "Не удалось выделить память на ноду";
        case AKINATOR_DATABASE_FILE_CREATE_ERROR:
            return "Ошибка при создании файла базы данных акинатора";
        case AKINATOR_DATABASE_FILE_OPEN_ERROR:
            return "Ошибка при открытии файла базы данных акинатора";
        default:
            return "Непредвиденная ошибка";
    }
}

void AkinatorPrintError(AkinatorError error, const char* file, int line) {
    assert(file != NULL);
    assert(line > 0);

    fprintf(stderr, "Error in %s:%d:\n%s\n", file, line, AkinatorStrError(error));
}

AkinatorError AkinatorTreeInit(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    if (TreeInit(akinator_tree) != TREE_OK) {
        return AKINATOR_TREE_ERROR;
    }

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeDestroy(Tree* akinator_tree) {
    assert(akinator_tree != NULL);    

    if (TreeDestroy(akinator_tree) != TREE_OK) {
        return AKINATOR_TREE_ERROR;
    }
    return AKINATOR_OK;
}

static AkinatorError AkinatorAnswerHandle(TreeNode* node) {
    assert(node != NULL);

    AkinatorPrintf("Вы загадали %s?\n", TreeNodeGetValue(node));

    char yes_or_no[1 + MAX_ANSWER_LEN];
    scanf("%"TO_STRING(MAX_ANSWER_LEN)"s", yes_or_no);

    if (strcmp(yes_or_no, "да") == 0) {
        AkinatorPrintf("Я крут!\n");
    }
    else {
        AkinatorPrintf("А кого вы загадали?\n");
        char name[1 + MAX_NAME_LEN] = "";
        scanf("\n%"TO_STRING(MAX_NAME_LEN)"[^\n]", name);

        AkinatorPrintf("Чем он(а) отличается от моего варианта? Он(а)(ваш вариант) ....\n");
        char attribute[1 + MAX_ATTRIBUTE_LEN] = "";
        scanf("\n%"TO_STRING(MAX_ATTRIBUTE_LEN)"[^\n]", attribute);

        TreeNode* current_answer = node;
        TreeNode* parent = TreeNodeGetParent(current_answer);
        TreeNode* new_answer = TreeNodeInit(name);
        TreeNode* attribute_node = TreeNodeInit(attribute);

        if (new_answer == NULL || attribute_node == NULL) {
            return AKINATOR_NODE_ALLOC_ERROR;
        }

        if (TreeNodeGetLeft(parent) == current_answer) {
            if (TreeNodeLinkLeft(parent, attribute_node) != TREE_OK) {
                return AKINATOR_TREE_ERROR;
            }
        }
        else {
            if (TreeNodeLinkRight(parent, attribute_node) != TREE_OK) {
                return AKINATOR_TREE_ERROR;
            }
        }

        if (TreeNodeLinkLeft(attribute_node, current_answer) != TREE_OK) {
            return AKINATOR_TREE_ERROR;
        }

        if (TreeNodeLinkRight(attribute_node, new_answer) != TREE_OK) {
            return AKINATOR_TREE_ERROR;
        }

        AkinatorPrintf("Спасибо, я его запомнил!\n");
    }

    return AKINATOR_OK;
}

static AkinatorError AkinatorQuestionHandle(TreeNode** node, TreeNode** node_parent) {
    assert(node != NULL);
    assert(node_parent != NULL);

    TreeNode* loc_node = *node;
    TreeNode* loc_node_parent = *node_parent;

    AkinatorPrintf("Он(a) %s?\n", TreeNodeGetValue(loc_node));

    char yes_or_no[1 + MAX_ANSWER_LEN] = "";
    scanf("%"TO_STRING(MAX_ANSWER_LEN)"s", yes_or_no);
    
    if (strcmp(yes_or_no, "да") != 0) {
        loc_node_parent = loc_node;
        loc_node = TreeNodeGetLeft(loc_node);
    }
    else {
        loc_node_parent = loc_node;
        loc_node = TreeNodeGetRight(loc_node);
    }

    *node = loc_node;
    *node_parent = loc_node_parent;

    return AKINATOR_OK;
}

AkinatorError AkinatorRequest(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    TreeNode* node_parent = TreeGetRoot(akinator_tree);
    TreeNode* node = TreeNodeGetLeft(node_parent);

    while (true) {
        bool is_answer = TreeNodeGetLeft(node) == NULL && TreeNodeGetRight(node) == NULL;
        if (is_answer) {
            AkinatorError ans_handle_err = AkinatorAnswerHandle(node);
            if (ans_handle_err != AKINATOR_OK) {
                return ans_handle_err;
            }

            break;
        }

        AkinatorError ans_handle_err = AkinatorQuestionHandle(&node, &node_parent);
        if (ans_handle_err != AKINATOR_OK) {
            return ans_handle_err;
        }
    }
    
    return AKINATOR_OK;
}

static AkinatorError AkinatorBuildSaveFile(TreeNode* node, FILE* database_file, size_t tab_count) {
    assert(database_file != NULL);

    if (node == NULL) {
        for (size_t tab_i = 0; tab_i < tab_count; tab_i++) { fprintf(database_file, "\t"); }
        fprintf(database_file, "{nil}\n");
        return AKINATOR_OK;
    }
    
    for (size_t tab_i = 0; tab_i < tab_count; tab_i++) { fprintf(database_file, "\t"); }
    fprintf(database_file, "{\n");

    for (size_t tab_i = 0; tab_i < tab_count; tab_i++) { fprintf(database_file, "\t"); }
    fprintf(database_file, "%s\n", TreeNodeGetValue(node));

    AkinatorBuildSaveFile(TreeNodeGetLeft(node), database_file, tab_count + 1);

    AkinatorBuildSaveFile(TreeNodeGetRight(node), database_file, tab_count + 1);

    for (size_t tab_i = 0; tab_i < tab_count; tab_i++) { fprintf(database_file, "\t"); }
    fprintf(database_file, "}\n");

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeSave(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    AkinatorPrintf("В какую базу данных вы хотите загрузить акинатора(введите имя файла):\n");
    char database_file_name[MAX_FILE_NAME_LEN + 1] = {};
    scanf("\n%"TO_STRING(MAX_FILE_NAME_LEN)"[^\n]", database_file_name);

    if (strlen(database_file_name) == 0) {
        snprintf(database_file_name, MAX_FILE_NAME_LEN, "%s", AKINATOR_STD_DATABASE_FILE_NAME);
    }

    FILE* database_file = fopen(database_file_name, "w");
    if (database_file == NULL) {
        return AKINATOR_DATABASE_FILE_CREATE_ERROR;
    }

    TreeNode* first_node = TreeNodeGetLeft(TreeGetRoot(akinator_tree));
    AkinatorError build_err = AkinatorBuildSaveFile(first_node, database_file, 0);
    if (build_err != AKINATOR_OK) {
        return build_err;
    }

    fclose(database_file);

    return AKINATOR_OK;
}

static AkinatorError AkinatorTreeBuild(TreeNode** node, TreeNode* node_parent, FILE* database_file) {
    assert(node != NULL);
    assert(node_parent != NULL);
    assert(database_file != NULL);

    TreeNode* loc_node = *node;

    char value[1 + MAX_TREE_CHAR_SIZE];
    fscanf(database_file, "{");
    SkipSpaces(database_file);
    fscanf(database_file, "%"TO_STRING(MAX_TREE_CHAR_SIZE)"[^\n]", value); //FIXME как пофиксить ввод?

    SkipSpaces(database_file);

    if (strcmp(value, "nil}") == 0) {
        *node = NULL;
        return AKINATOR_OK;
    }

    loc_node = TreeNodeInit(value);
    loc_node->parent = node_parent;

    AkinatorTreeBuild(&loc_node->left, loc_node, database_file);
    AkinatorTreeBuild(&loc_node->right, loc_node, database_file);
    
    fscanf(database_file, "}");

    SkipSpaces(database_file);

    *node = loc_node;

    return AKINATOR_OK;
}

static AkinatorError AkinatorDatabaseFillIfEmpty(const char* database_file_name) {
    FILE* database_file = fopen(database_file_name, "a");

    if (database_file == NULL) {
        return AKINATOR_DATABASE_FILE_CREATE_ERROR;
    }

    if (FileSize(database_file) == 0) {
        fwrite(AKINATOR_VOID_DATABASE, 1, sizeof(AKINATOR_VOID_DATABASE), database_file);
    }

    fclose(database_file);

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeLoad(Tree* akinator_tree, bool is_fast_load, char database_file_name[MAX_FILE_NAME_LEN + 1]) {
    assert(akinator_tree != NULL);

    if (akinator_tree->root != NULL) {
        AkinatorTreeDestroy(akinator_tree);

        AkinatorError init_error = AkinatorTreeInit(akinator_tree);
        if (init_error != AKINATOR_OK) {
            return init_error;
        }
    }

    char loc_database_file_name[MAX_FILE_NAME_LEN + 1] = {};
    
    if (is_fast_load) {
        strncpy(loc_database_file_name, database_file_name, MAX_FILE_NAME_LEN);
    }
    else {
        AkinatorPrintf("Из какой базы данных вы хотите загрузить акинатора(введите имя файла):\n");
        scanf("%"TO_STRING(MAX_FILE_NAME_LEN)"[^\n]", loc_database_file_name);
    }

    if (strlen(loc_database_file_name) == 0) {
        snprintf(loc_database_file_name, MAX_FILE_NAME_LEN, "%s", AKINATOR_STD_DATABASE_FILE_NAME);
    }

    AkinatorDatabaseFillIfEmpty(loc_database_file_name);

    FILE* database_file = fopen(loc_database_file_name, "r");
    if (database_file == NULL) {
        return AKINATOR_DATABASE_FILE_OPEN_ERROR;
    }

    AkinatorError build_err = AkinatorTreeBuild(&akinator_tree->root->left, akinator_tree->root, database_file);
    if (build_err != AKINATOR_OK) {
        return build_err;
    }

    fclose(database_file);

    return AKINATOR_OK;
}

static AkinatorError AkinatorGetAnswerList(TreeNode* node, const char* name, char* cur_ans_list, char* ans_list) {
    assert(node != NULL);
    assert(name != NULL);
    assert(cur_ans_list != NULL);
    assert(ans_list != NULL);

    bool is_answer = TreeNodeGetLeft(node) == NULL && TreeNodeGetRight(node) == NULL;
    if (is_answer) {
        if (strcmp(name, TreeNodeGetValue(node)) == 0) {
            strncpy(ans_list, cur_ans_list, MAX_ANSWER_LIST_LEN);
        }
        return AKINATOR_OK;
    }

    char left_answer_list[1 + MAX_ANSWER_LIST_LEN] = {};
    snprintf(left_answer_list, MAX_ANSWER_LIST_LEN, "%sn", cur_ans_list);
    AkinatorGetAnswerList(TreeNodeGetLeft(node), name, left_answer_list, ans_list);

    char right_answer_list[1 + MAX_ANSWER_LIST_LEN] = {};
    snprintf(right_answer_list, MAX_ANSWER_LIST_LEN, "%sy", cur_ans_list);
    AkinatorGetAnswerList(TreeNodeGetRight(node), name, right_answer_list, ans_list);

    return AKINATOR_OK;
}

AkinatorError AkinatorFind(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    AkinatorPrintf("Кого вы хотите найти?\n");

    char name[1 + MAX_NAME_LEN] = {};
    scanf("\n%"TO_STRING(MAX_NAME_LEN)"[^\n]", name);

    TreeNode* first_node = TreeNodeGetLeft(TreeGetRoot(akinator_tree));

    char ans_list[1 + MAX_ANSWER_LIST_LEN] = {};
    AkinatorGetAnswerList(first_node, name, "", ans_list);

    size_t ans_list_len = strlen(ans_list);

    if (ans_list_len == 0) {
        AkinatorPrintf("Я его не знаю\n");
        return AKINATOR_OK;
    }

    AkinatorPrintf("Он(а) ");

    TreeNode* node = first_node;
    for (size_t ans_i = 0; ans_i < ans_list_len; ans_i++) {
        if (ans_list[ans_i] == 'n') {
            AkinatorPrintf("не ");
        }

        AkinatorPrintf("%s ", TreeNodeGetValue(node));

        if (ans_list[ans_i] == 'n') {
            node = TreeNodeGetLeft(node);
        }
        else {
            node = TreeNodeGetRight(node);
        }
    }

    AkinatorPrintf("\n");

    return AKINATOR_OK;
}

AkinatorError AkinatorCompare(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    AkinatorPrintf("Кого вы хотите сравнить? Напишите в отдельные строчки по очереди:\n");

    char name1[1 + MAX_NAME_LEN] = {};
    scanf("\n%"TO_STRING(MAX_NAME_LEN)"[^\n]", name1);

    char name2[1 + MAX_NAME_LEN] = {};
    scanf("\n%"TO_STRING(MAX_NAME_LEN)"[^\n]", name2);

    TreeNode* first_node = TreeNodeGetLeft(TreeGetRoot(akinator_tree));
    
    char ans_list1[1 + MAX_ANSWER_LIST_LEN] = {};
    AkinatorGetAnswerList(first_node, name1, "", ans_list1);
    size_t ans_list1_len = strlen(ans_list1);

    char ans_list2[1 + MAX_ANSWER_LIST_LEN] = {};
    AkinatorGetAnswerList(first_node, name2, "", ans_list2);
    size_t ans_list2_len = strlen(ans_list2);

    size_t common_ans_cnt = 0;
    while (ans_list1[common_ans_cnt] == ans_list2[common_ans_cnt]) {
        common_ans_cnt++;
    }

    char common_ans_list[1 + MAX_ANSWER_LIST_LEN] = {};
    strncpy(common_ans_list, ans_list1, common_ans_cnt);

    char different_ans1[1 + MAX_ANSWER_LIST_LEN] = {};
    strncpy(different_ans1, ans_list1 + common_ans_cnt, MAX_ANSWER_LIST_LEN);
    size_t dif_ans1_len = ans_list1_len - common_ans_cnt;

    char different_ans2[1 + MAX_ANSWER_LIST_LEN];
    strncpy(different_ans2, ans_list2 + common_ans_cnt, MAX_ANSWER_LIST_LEN);
    size_t dif_ans2_len = ans_list2_len - common_ans_cnt;

    TreeNode* last_common_node = NULL;

    if (common_ans_cnt == 0) {
        AkinatorPrintf("Они ничем не похожи\n");
    }
    else {
        AkinatorPrintf("Каждый из них ");

        TreeNode* node = first_node;
        for (size_t common_ans_i = 0; common_ans_i < common_ans_cnt; common_ans_i++) {
            if (common_ans_list[common_ans_i] == 'n') {
                AkinatorPrintf("не ");
            }

            AkinatorPrintf("%s ", TreeNodeGetValue(node));

            if (common_ans_list[common_ans_i] == 'n') {
                node = TreeNodeGetLeft(node);
            }
            else {
                node = TreeNodeGetRight(node);
            }
        }
        
        last_common_node = TreeNodeGetParent(node);

        AkinatorPrintf("\n");
    }

    if (ans_list1_len - common_ans_cnt == 0) {
        AkinatorPrintf("Они ничем не отличаются");
    }
    else {
        AkinatorPrintf("Первый уникален тем что он: ");

        TreeNode* node1 = NULL;
        if (common_ans_list[common_ans_cnt - 1] == 'n') {
            node1 = TreeNodeGetLeft(last_common_node);
        }
        else {
            node1 = TreeNodeGetRight(last_common_node);
        }

        for (size_t ans1_i = 0; ans1_i < dif_ans1_len; ans1_i++) {
            if (different_ans1[ans1_i] == 'n') {
                AkinatorPrintf("не ");
            }

            AkinatorPrintf("%s ", TreeNodeGetValue(node1));

            if (different_ans1[ans1_i] == 'n') {
                node1 = TreeNodeGetLeft(node1);
            }
            else {
                node1 = TreeNodeGetRight(node1);
            }
        }

        AkinatorPrintf("\n");

        AkinatorPrintf("Второй уникален тем что он: ");

        TreeNode* node2 = NULL;
        if (common_ans_list[common_ans_cnt - 1] == 'n') {
            node2 = TreeNodeGetLeft(last_common_node);
        }
        else {
            node2 = TreeNodeGetRight(last_common_node);
        }

        for (size_t ans2_i = 0; ans2_i < dif_ans2_len; ans2_i++) {
            if (different_ans2[ans2_i] == 'n') {
                AkinatorPrintf("не ");
            }

            AkinatorPrintf("%s ", TreeNodeGetValue(node2));

            if (different_ans2[ans2_i] == 'n') {
                node2 = TreeNodeGetLeft(node2);
            }
            else {
                node2 = TreeNodeGetRight(node2);
            }
        }

        AkinatorPrintf("\n");
    }

    return AKINATOR_OK;
}

#ifndef NO_VOICE
AkinatorError AkinatorPrintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    char command_arg[MAX_PRINT_COMMAND_SIZE + 1];
    vsnprintf(command_arg, MAX_PRINT_COMMAND_SIZE, format, args);

    char command[2 * MAX_PRINT_COMMAND_SIZE + 1];
    snprintf(command, 2 * MAX_PRINT_COMMAND_SIZE, "espeak-ng -v ru \"%s\"", command_arg);

    system(command);

    va_end(args);

    return AKINATOR_OK;
}
#else
AkinatorError AkinatorPrintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);

    return AKINATOR_OK;
}
#endif