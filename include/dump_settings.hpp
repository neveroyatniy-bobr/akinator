#ifndef DUMP_SETTINGS_HPP_
#define DUMP_SETTINGS_HPP_

#include <stdlib.h>

static const char* PARENT_EDGE_COLOR = "blue";
static const char* LEFT_EDGE_COLOR = "red";
static const char* RIGHT_EDGE_COLOR = "green";

static const char* ROOT_NODE_COLOR = "red";
static const size_t ROOT_NODE_PEN_WIDTH = 4;

static const char* OBJECT_NODE_COLOR = "green";
static const size_t OBJECT_NODE_PEN_WIDTH = 4;

static const char* ATTRIBUTE_NODE_COLOR = "purple";
static const size_t ATTRIBUTE_NODE_PEN_WIDTH = 2;

static const unsigned int DUMP_FONT_SIZE = 36;

static const char DUMP_FILE_NAME[] = "dump_file.html";
static const char BUILD_DUMP_FILE_NAME[] = "build_dump_file.dot";

#endif // DUMP_SETTINGS_HPP_