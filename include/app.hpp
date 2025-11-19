#ifndef APP_HPP_
#define APP_HPP_

#include "akinator.hpp"

enum AkinatorAppMode {
    PLAY      =  0,
    FIND      =  1,
    COMPARE   =  2,
    SAVE      =  3,
    LOAD      =  4,
    QUIT      =  5
};

AkinatorError AkinatorApp(int argc, const char** argv);

#endif // APP_HPP_