#ifndef AKINATOR_HPP_
#define AKINATOR_HPP_

#include <string.h>

#include "tree.hpp"

#define MAX_NAME_LEN 128
#define MAX_ATTRIBUTE_LEN 128
#define MAX_QUESTION_LEN 256
#define MAX_ANSWER_LIST_LEN 256
#define MAX_ANSWER_LEN 8
#define MAX_FILE_NAME_LEN 256
#define MAX_PRINT_COMMAND_SIZE 256

static const char* AKINATOR_STD_DATABASE_FILE_NAME = "database.aki";
static const char AKINATOR_VOID_DATABASE[] = "{\n\tничего\n\t{nil}\n\t{nil}\n}\n";

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

AkinatorError AkinatorPrintf(const char* format, ...);

#endif // AKINATOR_HPP_