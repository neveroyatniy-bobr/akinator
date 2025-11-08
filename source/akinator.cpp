#include "akinator.hpp"

#include <stdio.h>
#include <assert.h>

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

    AkinatorTreeLoad(akinator_tree);

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeDestroy(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    AkinatorTreeSave(akinator_tree);    

    if (TreeDestroy(akinator_tree) != TREE_OK) {
        return AKINATOR_TREE_ERROR;
    }
    return AKINATOR_OK;
}

static AkinatorError AkinatorAnswerHandle(TreeNode* node) {
    assert(node != NULL);

    printf("Вы загадали %s?\n", TreeNodeGetValue(node));

    char yes_or_no[MAX_ANSWER_LEN];
    scanf("%s", yes_or_no);

    if (strcmp(yes_or_no, "да") == 0) {
        printf("Я крут!\n");
    }
    else {
        printf("А кого вы загадали?\n");
        char name[MAX_NAME_LEN] = "";
        scanf("\n%[^\n]", name);

        printf("Чем он(а) отличается от моего варианта? Он(а)(ваш вариант) ....\n");
        char attribute[MAX_ATTRIBUTE_LEN] = "";
        scanf("\n%[^\n]", attribute);

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

        printf("Спасибо, я его запомнил!\n");
    }

    return AKINATOR_OK;
}

static AkinatorError AkinatorDontKnowHandle(TreeNode* node_parent) {
    assert(node_parent != NULL);

    printf("Я не знаю такого!\n");

    printf("А кого вы загадали?\n");
    char name[MAX_NAME_LEN] = "";
    scanf("%s", name);

    TreeNode* new_answer = TreeNodeInit(name);
    if (new_answer == NULL) {
        return AKINATOR_NODE_ALLOC_ERROR;
    }

    if (node_parent->left == NULL) {
        if (TreeNodeLinkLeft(node_parent, new_answer) != TREE_OK) {
            return AKINATOR_TREE_ERROR;
        }
    }

    else {
        if (TreeNodeLinkRight(node_parent, new_answer) != TREE_OK) {
            return AKINATOR_TREE_ERROR;
        }
    }

    printf("Спасибо, я его запомнил!\n");

    return AKINATOR_OK;
}

static AkinatorError AkinatorQuestionHandle(TreeNode** node, TreeNode** node_parent) {
    assert(node != NULL);
    assert(node_parent != NULL);

    TreeNode* loc_node = *node;
    TreeNode* loc_node_parent = *node_parent;

    printf("Он(a) %s?\n", TreeNodeGetValue(loc_node));

    char yes_or_no[MAX_ANSWER_LEN] = "";
    scanf("%s", yes_or_no);
    
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


        //FIXME кажется можно удалить
        bool is_dont_know = node == NULL;
        if (is_dont_know) {
            AkinatorError ans_handle_err = AkinatorDontKnowHandle(node_parent);
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

static AkinatorError AkinatorBuildSaveFile(TreeNode* node, FILE* database_file) {
    assert(database_file != NULL);

    if (node == NULL) {
        fprintf(database_file, "{nil}");
        return AKINATOR_OK;
    }

    fprintf(database_file, "{");

    fprintf(database_file, "%s", TreeNodeGetValue(node));

    AkinatorBuildSaveFile(TreeNodeGetLeft(node), database_file);

    AkinatorBuildSaveFile(TreeNodeGetRight(node), database_file);

    fprintf(database_file, "}");

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeSave(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    FILE* database_file = fopen(AKINATOR_DATABASE_FILE_NAME, "w");
    if (database_file == NULL) {
        return AKINATOR_DATABASE_FILE_CREATE_ERROR;
    }

    TreeNode* first_node = TreeNodeGetLeft(TreeGetRoot(akinator_tree));
    AkinatorError build_err = AkinatorBuildSaveFile(first_node, database_file);
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

    char value[MAX_TREE_CHAR_SIZE];
    fscanf(database_file, "{%[^{}]", value);

    if (strcmp(value, "nil") == 0) {
        *node = NULL;
        fscanf(database_file, "}");
        return AKINATOR_OK;
    }

    loc_node = TreeNodeInit(value);
    loc_node->parent = node_parent;

    AkinatorTreeBuild(&loc_node->left, loc_node, database_file);
    AkinatorTreeBuild(&loc_node->right, loc_node, database_file);
    
    fscanf(database_file, "}");

    *node = loc_node;

    return AKINATOR_OK;
}

AkinatorError AkinatorTreeLoad(Tree* akinator_tree) {
    assert(akinator_tree != NULL);

    FILE* database_file = fopen(AKINATOR_DATABASE_FILE_NAME, "r");
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
    bool is_answer = TreeNodeGetLeft(node) == NULL && TreeNodeGetRight(node) == NULL;
    if (is_answer) {
        if (strcmp(name, TreeNodeGetValue(node)) == 0) {
            strncpy(ans_list, cur_ans_list, MAX_ANSWER_LIST_LEN);
        }
        return AKINATOR_OK;
    }

    char left_answer_list[MAX_ANSWER_LIST_LEN] = {};
    snprintf(left_answer_list, MAX_ANSWER_LIST_LEN, "%sn", cur_ans_list);
    AkinatorGetAnswerList(TreeNodeGetLeft(node), name, left_answer_list, ans_list);

    char right_answer_list[MAX_ANSWER_LIST_LEN] = {};
    snprintf(right_answer_list, MAX_ANSWER_LIST_LEN, "%sy", cur_ans_list);
    AkinatorGetAnswerList(TreeNodeGetRight(node), name, right_answer_list, ans_list);

    return AKINATOR_OK;
}

AkinatorError AkinatorGetDefine(Tree* akinator_tree, const char* name) {
    TreeNode* first_node = TreeNodeGetLeft(TreeGetRoot(akinator_tree));
    char ans_list[MAX_ANSWER_LIST_LEN] = {};
    
    AkinatorGetAnswerList(first_node, name, "", ans_list);

    size_t ans_list_len = strlen(ans_list);

    if (ans_list_len == 0) {
        printf("Я его не знаю\n");
        return AKINATOR_OK;
    }

    printf("Он(а) ");

    TreeNode* node = first_node;
    for (size_t ans_i = 0; ans_i < ans_list_len; ans_i++) {
        if (ans_list[ans_i] == 'n') {
            printf("не ");
        }

        printf("%s ", TreeNodeGetValue(node));
    }

    printf("\n");

    return AKINATOR_OK;
}