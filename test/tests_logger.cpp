#include "log.h"
#include "util.h"
#include "singleton.h"
#include <iostream>
#include <sys/syscall.h> // call for thread id 


int main(int argc, char*argv[]) {
	// call logger constructor 
	log::Logger::ptr logger(new log::Logger);
	// first call LogAppender constructor then call StdoutLogAppender
	logger->addAppender(log::LogAppender::ptr(new log::StdoutLogAppender));
	// xbort::LogLevel::Level level = xbort::LogLevel::DEBUG;

	log::FileLogAppender::ptr file_appender(new log::FileLogAppender("./log.txt"));
	log::LogFormatter::ptr fmt(new log::LogFormatter("%d%T%p%T%m%n"));
	file_appender->setFormatter(fmt);
	file_appender->setLevel(log::LogLevel::ERROR);
		
	logger->addAppender(file_appender);
	/*log::LogEvent::ptr event(new log::LogEvent(logger, log::LogLevel::DEBUG, __FILE__, __LINE__, 0,
		util::GetThreadId(), util::GetFiberId(), time(0)));
	event->getSS() << std::endl <<  "2020-12-19 Debug Test";

	logger->log(log::LogLevel::DEBUG, event);*/

	LOG_ERROR(logger) << "test error" << std::endl;
	LOG_INFO(logger) << "test info" << std::endl;
	LOG_FATAL(logger) << "test fatal" << std::endl;
	LOG_WARN(logger) << "test warn" << std::endl;
	LOG_DEBUG(logger) << "test debug" << std::endl;
	LOG_FMT_ERROR(logger, "error %s\n", "aa"); 
	auto i = log::LoggerMgr::GetInstance()->getLogger("xx");
	/*std::string str = i ? "true" : "false";
	std::cout << str << std::endl;*/
	LOG_INFO(i) << "22:17" << std::endl;
	LOG_ERROR(i) << "22:17" << std::endl;
	//LOG_FATAL(i) << "22:17" << std::endl;
	//LOG_WARN(i) << "22:17" << std::endl;
	//LOG_DEBUG(i) << "22:17" << std::endl;

	getchar();
}