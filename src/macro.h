#ifndef __MACRO_H__
#define __MACRO_H__

#include <string>
#include <string.h>
#include "ipmsg.h"

#define ASSERT_MACRO(x) \
    if(!(x)) { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            << "\nbacktrace:\n" \
            << ipmsg::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#define ASSERT_MACRO2(x, w) \
    if((!(x))) { \
        LOG_ERROR(LOG_ROOT()) << "ASSERTION: " #x \
            << "\n" << #w \
            << "\nbacktrace:\n" \
            << ipmsg::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }

#endif
