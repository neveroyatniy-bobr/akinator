#ifndef AKINATOR_HPP_
#define AKINATOR_HPP_

#include <string.h>

#include "tree.hpp"

#define MAX_NAME_LEN 128
#define MAX_ATTRIBUTE_LEN 128
#define MAX_QUESTION_LEN 256
#define MAX_ANSWER_LIST_LEN 256
#define MAX_ANSWER_LEN 8

#define TO_STRING_(x) #x
#define TO_STRING(x) TO_STRING_(x)

static const char* AKINATOR_DATABASE_FILE_NAME = "database.aki";

#define FIRST_NODE_NAME "Ничего"

enum AkinatorError {
    AKINATOR_OK                         =  0,
    AKINATOR_TREE_ERROR                 =  1,
    AKINATOR_NODE_ALLOC_ERROR           =  2,
    AKINATOR_DATABASE_FILE_CREATE_ERROR =  3,
    AKINATOR_DATABASE_FILE_OPEN_ERROR   =  4
};

const char* AkinatorStrError(AkinatorError error);

void AkinatorPrintError(AkinatorError error, const char* file, int line);

#define AKINATOR_PRINT_ERROR(error) AkinatorPrintError(error, __FILE__, __LINE__)

AkinatorError AkinatorTreeInit(Tree* akinator_tree);

AkinatorError AkinatorTreeDestroy(Tree* akinator_tree);

AkinatorError AkinatorRequest(Tree* akinator_tree);

AkinatorError AkinatorTreeSave(Tree* akinator_tree);

AkinatorError AkinatorTreeLoad(Tree* akinator_tree);

AkinatorError AkinatorFind(Tree* akinator_tree);

AkinatorError AkinatorCompare(Tree* akinator_tree);

#endif // AKINATOR_HPP_