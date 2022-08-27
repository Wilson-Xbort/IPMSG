#include "../src/ipmsg.h"
#include <assert.h>

ipmsg::Logger::ptr g_logger = LOG_ROOT();

void test_assert() {
    // assert(0);
    LOG_INFO(g_logger) << ipmsg::BacktraceToString(10);
    // ASSERT_MACRO(false);
    ASSERT_MACRO2(1 == 0, "xxx asss");
}

int main(int argc, char **argv) {
    test_assert();
    return 0;
}

