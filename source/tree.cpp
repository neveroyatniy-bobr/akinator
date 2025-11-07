#include "tree.hpp"

#include <stdio.h>
#include <assert.h>

const char* TreeStrError(TreeError error) {
    switch (error) {
        case TREE_OK:
            return "Выполнено без ошибок";
        case TREE_NODE_ALLOC_ERROR:
            return "Не получилось выделить память на ноду";
        default:
            return "Непредвиденная ошибка";
    }
}

void TreePrintError(TreeError error, const char* file, int line) {
    fprintf(stderr, "Error in %s:%d:\n %s\n", file, line, TreeStrError(error));
}

TreeNode* TreeNodeInit(tree_elem_t value) { // FIXME return node else return null
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));

    if (node == NULL) {
        return NULL;
    }

    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    node->value = value;

    return node;
}

TreeError TreeNodeDestroy(TreeNode** node) {
    assert(node != NULL);

    TreeNode* loc_node = *node;

    loc_node->parent = NULL;
    loc_node->left = NULL;
    loc_node->right = NULL;

    loc_node->value = 0;

    free(loc_node);
    loc_node = NULL;

    *node = loc_node;

    return TREE_OK;
}

TreeNode* TreeNodeGetParent(TreeNode* node) {
    assert(node != NULL);

    return node->parent;
}

TreeNode* TreeNodeGetLeft(TreeNode* node) {
    assert(node != NULL);

    return node->left;
}

TreeError TreeNodeLinkLeft(TreeNode* node, TreeNode* new_left) {
    assert(node != NULL);

    if (node->left != NULL) {
        node->left->parent = NULL;
    }

    node->left = new_left;

    if (new_left != NULL) {
        new_left->parent = node;
    }

    return TREE_OK;
}

TreeNode* TreeNodeGetRight(TreeNode* node) {
    assert(node != NULL);

    return node->right;
}

TreeError TreeNodeLinkRight(TreeNode* node, TreeNode* new_right) {
    assert(node != NULL);

    if (node->right != NULL) {
        node->right->parent = NULL;
    }

    node->right = new_right;

    if (new_right != NULL) {
        new_right->parent = node;
    }

    return TREE_OK;
}

tree_elem_t TreeNodeGetValue(TreeNode* node) {
    assert(node != NULL);

    return node->value;
}


TreeError TreeNodeSetValue(TreeNode* node, tree_elem_t new_value) {
    assert(node != NULL);

    node->value = new_value;

    return TREE_OK;
}

static void TreeNodesBuildDump(FILE* build_dump_file, TreeNode* node) {
    assert(build_dump_file != NULL);
    assert(node != NULL);

    tree_elem_t value = TreeNodeGetValue(node);

    TreeNode* parent = TreeNodeGetParent(node);

    TreeNode* left = TreeNodeGetLeft(node);

    TreeNode* right = TreeNodeGetRight(node);

    fprintf(build_dump_file, "    node_%p [label=\"value = %s\\nself = %p\\nparent = %p\\nleft = %p\\nright = %p\"];\n", node, value, node, parent, left, right);

    if (node->left != NULL) {
        TreeNodesBuildDump(build_dump_file, node->left);
    }

    if (node->right != NULL) {
        TreeNodesBuildDump(build_dump_file, node->right);
    }
}

static void TreeEdgesBuildDump(FILE* build_dump_file, TreeNode* node) {
    assert(build_dump_file != NULL);
    assert(node != NULL);

    TreeNode* parent = TreeNodeGetParent(node);

    TreeNode* left = TreeNodeGetLeft(node);

    TreeNode* right = TreeNodeGetRight(node);

    if (parent != NULL) {
        fprintf(build_dump_file, "    node_%p -> node_%p [label = \"parent\"];\n", node, parent);
    }

    if (left != NULL) {
        fprintf(build_dump_file, "    node_%p -> node_%p [label = \"left\"];\n", node, left);
    }

    if (right != NULL) {
        fprintf(build_dump_file, "    node_%p -> node_%p [label = \"right\"];\n", node, right);
    }

    if (node->left != NULL) {
        TreeEdgesBuildDump(build_dump_file, node->left);
    }

    if (node->right != NULL) {
        TreeEdgesBuildDump(build_dump_file, node->right);
    }
}

void TreeDump(Tree* tree, const char* file, int line) {
    FILE* build_dump_file = fopen(BUILD_DUMP_FILE_NAME, "w");

    if (build_dump_file == NULL) {
        fprintf(stderr, "Ошибка в создании файла дампа\n");
        return;
    }

    fprintf(build_dump_file, "digraph G {\n    rankdir=TB;\n    node [shape=record];\n\n");

    TreeNodesBuildDump(build_dump_file, tree->root);

    fprintf(build_dump_file, "\n");

    TreeEdgesBuildDump(build_dump_file, tree->root);

    fprintf(build_dump_file, "}");

    fclose(build_dump_file);


    char command[BUILD_DUMP_COMMAND_SIZE];

    sprintf(command, "dot -Tsvg %s -o %s", BUILD_DUMP_FILE_NAME, DUMP_FILE_NAME);

    system(command);

    FILE* dump_file = fopen(DUMP_FILE_NAME, "a");

    if (dump_file == NULL) {
        fprintf(stderr, "Ошибка в создании файла дампа\n");
        return;
    }

    fprintf(dump_file, "\n<p style=\"font-size: %upx;\">\n    size = %lu\n</p>\n", DUMP_FONT_SIZE, tree->size);

    fprintf(dump_file, "<p style=\"font-size: %upx;\">\n    ERROR in %s:%d: %s</p>", DUMP_FONT_SIZE, file, line, TreeStrError(tree->last_error));

    fclose(dump_file);
}

TreeError TreeInit(Tree* tree) {
    tree->root = TreeNodeInit(ROOT_VALUE);

    tree->size = 0;

    tree->last_error = TREE_OK;

    return TREE_OK;
}

TreeError TreeSubTreeDestroy(TreeNode** node) {
    TreeNode* loc_node = *node;

    if (loc_node->left != NULL) {
        TreeSubTreeDestroy(&loc_node->left);
    }

    if (loc_node->right != NULL) {
        TreeSubTreeDestroy(&loc_node->right);
    }

    TreeNodeDestroy(&loc_node);

    *node = loc_node;

    return TREE_OK;
}

TreeError TreeDestroy(Tree* tree) {
    TreeSubTreeDestroy(&tree->root);

    tree->size = 0;

    tree->last_error = TREE_OK;

    return TREE_OK;
}
