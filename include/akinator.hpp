#ifndef AKINATOR_HPP_
#define AKINATOR_HPP_

#include <string.h>

#include "tree.hpp"

static const size_t MAX_NAME_LEN = 128;
static const size_t MAX_ATTRIBUTE_LEN = 128;
static const size_t MAX_QUESTION_LEN = 256;
static const size_t MAX_ANSWER_LEN = 8;

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

#endif // AKINATOR_HPP_