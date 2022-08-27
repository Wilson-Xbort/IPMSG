#include "ipmsg.h"
#include <assert.h>

ipmsg::Logger::ptr g_logger = LOG_ROOT();

void run_in_fiber() {
    LOG_INFO(g_logger) << "run_in_fiber begin";
    ipmsg::Fiber::YieldToHold();
    LOG_INFO(g_logger) << "run_in_fiber end";
    ipmsg::Fiber::YieldToHold();
}

void test_fiber() {
    LOG_INFO(g_logger) << "main begin -1";
    {
        ipmsg::Fiber::GetThis();
        LOG_INFO(g_logger) << "main begin";
        ipmsg::Fiber::ptr fiber(new ipmsg::Fiber(run_in_fiber));
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char **argv) {
    // test_assert();
    ipmsg::Thread::setName("main");

    std::vector<ipmsg::Thread::ptr> thrs;
    for(int i = 0; i < 3; i++) {
        thrs.push_back(ipmsg::Thread::ptr
            (new ipmsg::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }

    return 0;
}

