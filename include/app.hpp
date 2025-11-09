#ifndef APP_HPP_
#define APP_HPP_

#include "akinator.hpp"

enum AkinatorAppMode {
    PLAY      =  0,
    FIND      =  1,
    COMPARE   =  2,
    QUIT      =  3
};

AkinatorError AkinatorApp();

#endif // APP_HPP_