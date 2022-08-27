#pragma once
#include <list>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <tuple>
#include <stdarg.h>
#include <map>
#include <stdint.h> // uint_64
#include "util.h"
#include "singleton.h"
#include "thread.h"

/**
 * @file log.h
 * @brief 日志模块封装
 * @author ipmsg.yin
 * @email 564628276@qq.com
 * @date 2019-05-23
 * @copyright Copyright (c) 2019年 ipmsg.yin All rights reserved (www.ipmsg.top)
 */
#ifndef __ipmsg_LOG_H__
#define __ipmsg_LOG_H__

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 */
#define LOG_LEVEL(logger, level) \
	if(logger->getLevel() <= level) \
		 ipmsg::LogEventWrap(ipmsg::LogEvent::ptr(new ipmsg::LogEvent(logger, level,__FILE__, __LINE__, \
                         0, ipmsg::GetThreadId(),	\
                ipmsg::GetFiberId(), time(0), ipmsg::Thread::GetName()))).getSS()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define LOG_DEBUG(logger) LOG_LEVEL(logger, ipmsg::LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别INFO的日志写入到logger
 */
#define LOG_INFO(logger) LOG_LEVEL(logger, ipmsg::LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 */
#define LOG_WARN(logger) LOG_LEVEL(logger, ipmsg::LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 */
#define LOG_ERROR(logger) LOG_LEVEL(logger, ipmsg::LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 */
#define LOG_FATAL(logger) LOG_LEVEL(logger, ipmsg::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
	ipmsg::LogEventWrap(ipmsg::LogEvent::ptr(new ipmsg::LogEvent( \
		logger, level,__FILE__, __LINE__, 0, ipmsg::GetThreadId(), \
	ipmsg::GetFiberId(), time(0), ipmsg::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, ipmsg::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define LOG_FMT_INFO(logger, fmt, ...)  LOG_FMT_LEVEL(logger, ipmsg::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define LOG_FMT_WARN(logger, fmt, ...)  LOG_FMT_LEVEL(logger, ipmsg::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, ipmsg::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, ipmsg::LogLevel::FATAL, fmt, __VA_ARGS__)

/**
 * @date  2020-12-25 20:45
 * @brief 获取主日志器
 */
#define LOG_ROOT() ipmsg::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获取name的日志器
 */
#define LOG_NAME(name) ipmsg::LoggerMgr::GetInstance()->getLogger(name)

namespace ipmsg {

class Logger;
class LoggerManager;
/**
 * @brief 日志级别
 */
class LogLevel {
public:
	enum Level {
		/// 未知级别
		UNKNOW = 0,
		/// DEBUG 级别
		DEBUG = 1,
		/// INFO 级别
		INFO = 2,
		/// WARN 级别
		WARN = 3,
		/// ERROR 级别
		ERROR = 4,
		/// FATAL 级别
		FATAL = 5
	};
	/**
	* @brief 将日志级别转成文本输出
	* @param[in] level 日志级别
	*/
	static const char* ToString(LogLevel::Level level);

	/**
	* @brief 将文本转换成日志级别
	* @param[in] str 日志级别文本
	*/
	static LogLevel::Level FromString(const std::string& str);
};

/**
* @brief 日志事件
*/
class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	/**
	* @brief 构造函数
	* @param[in] logger 日志器
	* @param[in] level 日志级别
	* @param[in] file 文件名
	* @param[in] line 文件行号
	* @param[in] elapse 程序启动依赖的耗时(毫秒)
	* @param[in] thread_id 线程id
	* @param[in] fiber_id 协程id
	* @param[in] time 日志事件(秒)
	* @param[in] thread_name 线程名称
	*/
	LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
			,const char* file, int32_t line, uint32_t elapse
			,uint32_t thread_id, uint32_t fiber_id, uint64_t time
			,const std::string& thread_name);
	/**
	* @brief 返回文件名
	*/
	const char* getFile() const { return m_file; }

	/**
	* @brief 返回行号
	*/
	int32_t getLine() const { return m_line; }

	/**
	* @brief 返回耗时
	*/
	uint32_t getElapse() const { return m_elapse; }

	/**
	* @brief 返回线程ID
	*/
	uint32_t getThreadId() const { return m_threadId; }

	/**
	* @brief 返回协程ID
	*/
	uint32_t getFiberId() const { return m_fiberId; }

	/**
	* @brief 返回时间
	*/
	uint64_t getTime() const { return m_time; }

	/**
	* @brief 返回线程名称
	*/
	const std::string& getThreadName() const { return m_threadName; }

	/**
	* @brief 返回日志内容
	*/
	std::string getContent() const { return m_ss.str(); }

	/**
	* @brief 返回日志器
	*/
	std::shared_ptr<Logger> getLogger() const { return m_logger; }

	/**
	* @brief 返回日志级别
	*/
	LogLevel::Level getLevel() const { return m_level; }

	/**
	* @brief 返回日志内容字符串流
	*/
	std::stringstream& getSS() { return m_ss; }

	/**
	* @brief 格式化写入日志内容
	*/
	void format(const char* fmt, ...);

	/**
	 * @brief 格式化写入日志内容
	 */
	void format(const char* fmt, va_list al);
private:
	// 文件名
	const char* m_file = nullptr;
	// 行号
	int32_t m_line = 0;
	// 程序启动开始到现在的毫秒数
	uint32_t m_elapse = 0;
	// 线程ID
	uint32_t m_threadId = 0;
	// 协程ID
	uint32_t m_fiberId = 0;
	// 时间戳
	uint64_t m_time = 0;
	// 线程名称
	std::string m_threadName;
	// 日志内容流
	std::stringstream m_ss;
	// 日志器,用于LogEventWrap
	std::shared_ptr<Logger> m_logger;
	// 日志等级
	LogLevel::Level m_level;
};


class LogEventWrap {
public:
	LogEventWrap(LogEvent::ptr e);
	~LogEventWrap();

	/**
	* @brief 返回日志内容字符串流
	*/
	std::stringstream& getSS();

	/**
	 * @brief 获取日志事件
	 */
	LogEvent::ptr getEvent() const { return m_event; }
private:
	LogEvent::ptr m_event;
};

/**
* @brief 日志格式化
*/
class LogFormatter {
public:
	/**
	*	@brief 被共享指针管理
	*/
	typedef std::shared_ptr<LogFormatter> ptr;

	/**
	* @brief 构造函数
	* @param[in] pattern 格式模板
	* @details
	*  %m 消息
	*  %p 日志级别
	*  %r 累计毫秒数
	*  %c 日志名称
	*  %t 线程id
	*  %n 换行
	*  %d 时间
	*  %f 文件名
	*  %l 行号
	*  %T 制表符
	*  %F 协程id
	*  %N 线程名称
	*
	*  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
	*/
	LogFormatter(const std::string& pattern);

	/**
	* @brief 返回格式化日志文本
	* @param[in] logger 日志器
	* @param[in] level 日志级别
	* @param[in] event 日志事件
	*/
	std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,
		LogEvent::ptr event);
	std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level,
		LogEvent::ptr event);

	/**
	* @brief 初始化,解析日志模板
	*/
	void init();

	bool isError() const { return m_error; }

	const std::string getPattern() const { return m_pattern; }
public:
	class FormatItem {
	public:
		typedef std::shared_ptr<FormatItem> ptr;
		/**
		* @brief 析构函数
		*/
		virtual ~FormatItem() {}

		/**
		* @brief 纯虚函数
		* @brief 格式化日志到流
		* @param[in, out] os 日志输出流
		* @param[in] logger 日志器
		* @param[in] level 日志等级
		* @param[in] event 日志事件
		*/
		virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
	};

private:
	// 日志格式模板
	std::string m_pattern;
	// 日志格式解析后格式
	std::vector<FormatItem::ptr> m_items;
	// 是否有错误
	bool m_error = false;

};

/**
* @brief 日志输出目标
*/
class LogAppender {
friend class Logger; // logger 调用 成员变量
public:
    typedef Spinlock MutexType;
	/**
	*	@brief 被共享指针管理
	*/
	typedef std::shared_ptr<LogAppender> ptr;

	LogAppender() {
        // std::cout << "LogAppender construct()..." << std::endl;
	}

	/**
	* @brief 析构函数
	*/
	virtual ~LogAppender() {}

	/**
	* @brief 写入日志
	* @param[in] logger 日志器
	* @param[in] level 日志级别
	* @param[in] event 日志事件
	*/
	virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

	/**
	* @brief 更改日志格式器
	*/
	void setFormatter(LogFormatter::ptr val);

	/**
	* @brief 获取日志格式器
	*/
	LogFormatter::ptr getFormatter();

	/**
	 * @brief 设置日志级别
	 */
	void setLevel(LogLevel::Level val) { m_level = val; }

	/**
	* @brief 获取日志级别
	*/
	LogLevel::Level getLevel() const { return m_level; }

	virtual std::string toYamlString() = 0;
protected:
	LogLevel::Level m_level = LogLevel::DEBUG;
	bool m_hasFormatter = false;
	// 定义输出的格式
	LogFormatter::ptr m_formatter;

	MutexType m_mutex;
};

/**
* @brief 日志器
* @detail enable_shared_from_this : 需要把当前类作为参数传给其他函数
*/
class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager; // 使LoggerManger可以访问 Logger下的m_name
public:
    typedef Spinlock MutexType;
	/**
	*	@brief 被共享指针管理
	*/
	typedef std::shared_ptr<Logger> ptr;
	/**
	* @brief 构造函数
	* @param[in] name 日志器名称
	*/
	Logger(const std::string& name = "root");

	/**
	* @brief 写日志
	* @param[in] level 日志级别
	* @param[in] event 日志事件
	*/
	void log(LogLevel::Level level, LogEvent::ptr event);

	/**
	* @brief 写debug级别日志
	* @param[in] event 日志事件
	*/
	void debug(LogEvent::ptr event);

	/**
	* @brief 写info级别日志
	* @param[in] event 日志事件
	*/
	void info(LogEvent::ptr event);

	/**
	* @brief 写warn级别日志
	* @param[in] event 日志事件
	*/
	void warn(LogEvent::ptr event);

	/**
	* @brief 写error级别日志
	* @param[in] event 日志事件
	*/
	void error(LogEvent::ptr event);

	/**
	* @brief 写fatal级别日志
	* @param[in] event 日志事件
	*/
	void fatal(LogEvent::ptr event);

	/**
	* @brief 添加日志目标
	* @param[in] appender 日志目标
	*/
	void addAppender(LogAppender::ptr appender);

	/**
	* @brief 删除日志目标
	* @param[in] appender 日志目标
	*/
	void delAppender(LogAppender::ptr appender);

	/**
	* @brief 清空日志目标
	*/
	void clearAppenders();

	/**
	* @brief 返回日志级别
	*/
	LogLevel::Level getLevel() const { return m_level; }

	/**
	* @brief 设置日志级别
	*/
	inline void setLevel(LogLevel::Level val) { m_level = val; }

	/**
	* @brief 获取日志格式器
	*/
	LogFormatter::ptr getFormatter();


   void setFormatter(LogFormatter::ptr val);

   void setFormatter(const std::string& val);

    std::string toYamlString();

	/**
	*	@brief 返回日志名称
	*/
	const std::string& getName() const { return m_name; }
private:
	/// 日志名称
	std::string m_name;
	/// 日志级别
	LogLevel::Level m_level;
	/// 日志目标集合
	std::list<LogAppender::ptr> m_appenders;
	/// 日志格式器
	LogFormatter::ptr m_formatter;
	/// 主日志器
	Logger::ptr m_root;
    MutexType m_mutex;
};

/**
* @brief 输出到控制台的Appender
*/
class StdoutLogAppender : public LogAppender {
public:
	StdoutLogAppender() {
        // std::cout << "StdoutLogAppender construct()" << std::endl;
	}
	typedef std::shared_ptr<StdoutLogAppender> ptr;
    // void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

/**
* @brief 输出到文件的Appender
*/
class FileLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string& filename);
	// void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
	void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
	/**
	* @brief 重新打开日志文件
	* @return 成功返回true
	*/
	bool reopen();
private:
	// 文件路径
	std::string m_filename;
	// 文件流
	std::ofstream m_filestream;
    /// 上次重新打开时间
    uint64_t m_lastTime = 0;
};

/**
 * @brief  日志器管理类
 * @detail 管理所有的日志器，并且可以通过解析Yaml配置，动态创建或修改日志器相关的内容
 */
class LoggerManager {
public:
    typedef Spinlock MutexType;
	LoggerManager();

	/**
	* @brief 获取日志器
	* @param[in] name 日志器名称
	*/
	Logger::ptr getLogger(const std::string& name);

	/**
	 * @brief 初始化
	 */
	void init();

	/**
	 * @date  2020-12-25 20:50
	 * @brief 返回主日志器
	 */
	Logger::ptr getRoot() const { return m_root; }

    /**
     * @brief 将所有的日志器配置转成YAML String
     */
	std::string toYamlString();
private:
    MutexType m_mutex;
	/// 日志器容器
	std::map<std::string, Logger::ptr> m_loggers;
	/// 主日志器
	Logger::ptr m_root;
};

/// 日志器管理类单例模式
typedef ipmsg::Singleton<LoggerManager> LoggerMgr;

}

#endif
