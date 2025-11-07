#ifndef TREE_HPP_
#define TREE_HPP_

#include <stdlib.h>
#include <limits.h>

typedef char* tree_elem_t;

enum TreeError {
    TREE_OK                   =  0,
    TREE_NODE_ALLOC_ERROR     =  1
};

const char* TreeStrError(TreeError error);

void TreePrintError(TreeError error, const char* file, int line);

#define TREE_PRINT_ERROR(error) TreePrintError(error, __FILE__, __LINE__)

static const tree_elem_t ROOT_VALUE = NULL;

static const char DUMP_FILE_NAME[] = "dump_file.html";
static const char BUILD_DUMP_FILE_NAME[] = "build_dump_file.dot";
static const size_t BUILD_DUMP_COMMAND_SIZE = 128;
static const unsigned int DUMP_FONT_SIZE = 36;

struct TreeNode {
    tree_elem_t value;

    TreeNode* parent;
    TreeNode* left;
    TreeNode* right;
};


struct Tree {
    TreeNode* root;

    size_t size;

    TreeError last_error;
};

TreeNode* TreeNodeInit(tree_elem_t value);

TreeError TreeNodeDestroy(TreeNode** node);

TreeNode* TreeNodeGetParent(TreeNode* node);

TreeNode* TreeNodeGetLeft(TreeNode* node);

TreeError TreeNodeLinkLeft(TreeNode* node, TreeNode* new_left);

TreeNode* TreeNodeGetRight(TreeNode* node);

TreeError TreeNodeLinkRight(TreeNode* node, TreeNode* new_right);

tree_elem_t TreeNodeGetValue(TreeNode* node);

TreeError TreeNodeSetValue(TreeNode* node, tree_elem_t new_value);

void TreeDump(Tree* tree, const char* file, int line);

#define TREE_DUMP(tree) TreeDump(tree, __FILE__, __LINE__)

TreeError TreeInit(Tree* tree);

TreeError TreeSubTreeDestroy(TreeNode** node);

TreeError TreeDestroy(Tree* tree);

#endif // TREE_HPP_