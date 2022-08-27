#include <iostream>
#include "../src/log.h"
#include "../src/util.h"

int main(int argc, char** argv) {
    ipmsg::Logger::ptr logger(new ipmsg::Logger);
    logger->addAppender(ipmsg::LogAppender::ptr(new ipmsg::StdoutLogAppender));

    ipmsg::FileLogAppender::ptr file_appender(new ipmsg::FileLogAppender("./log.txt"));
    ipmsg::LogFormatter::ptr fmt(new ipmsg::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(ipmsg::LogLevel::ERROR);

    logger->addAppender(file_appender);

    std::cout << "hello ipmsg log" << std::endl;

    LOG_INFO(logger) << "test macro";
    LOG_ERROR(logger) << "test macro error";

    LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = ipmsg::LoggerMgr::GetInstance()->getLogger("xx");
    LOG_INFO(l) << "xxx";
    return 0;
}
