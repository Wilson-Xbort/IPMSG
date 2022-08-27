#include <map>
#include <cstdarg>
#include <functional>
#include "log.h"
#include "config.h"
#include <time.h>

namespace ipmsg {

const char* LogLevel::ToString(LogLevel::Level level){
	switch (level) {
#define XX(name) \
	case LogLevel::name: \
		return #name; \
		break;

		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);
#undef XX
	default:
		return "UNKNOW";
	}
	return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }

	XX(DEBUG, debug);
	XX(INFO, info);
	XX(WARN, warn);
	XX(ERROR, error);
	XX(FATAL, fatal);

	XX(DEBUG, DEBUG);
	XX(INFO, INFO);
	XX(WARN, WARN);
	XX(ERROR, ERROR);
	XX(FATAL, FATAL);
	return LogLevel::UNKNOW;
#undef XX
}

LogEventWrap::LogEventWrap(LogEvent::ptr e)
	:m_event(e){
}

LogEventWrap::~LogEventWrap() {
	m_event->getLogger()->log(m_event->getLevel(), m_event);
}

void LogEvent::format(const char* fmt, ...)
{
	va_list vs;
	// std::cout << __FILE__ << "  " << __LINE__ << "  " << std::endl;
	va_start(vs, fmt);
	// std::cout << fmt << std::endl;
	format(fmt, vs);
	va_end(vs);
}

void LogEvent::format(const char* fmt, va_list al)
{
	char* buf = nullptr;
	int len = vasprintf(&buf, fmt, al); // 根据fmt 和 va_list 分配内存
	/*std::cout << "fmt = " << fmt << std::endl;
	std::cout << "buf = " << buf << std::endl;
	std::cout << __FILE__ << "  " << __LINE__ << "  " << "len = " << len << std::endl;*/
	if (len != -1) { // vasprintf 失败返回 -1
		m_ss << std::string(buf, len);
		free(buf);
	}
}

std::stringstream& LogEventWrap::getSS() {
	return m_event->getSS(); // 调用shared_ptr 里面的getSS()
}


void LogAppender::setFormatter(LogFormatter::ptr val)
{
    MutexType::Lock lock(m_mutex);
	m_formatter = val;
    if(m_formatter) {
        m_hasFormatter = true;
    }else {
        m_hasFormatter = false;
    }
}

LogFormatter::ptr LogAppender::getFormatter()
{
     MutexType::Lock lock(m_mutex);
	return m_formatter;
}

/**
*	@brief 返回日志内容
*/
class MessageFormatItem : public LogFormatter::FormatItem {
public:
	MessageFormatItem(const std::string& str = "") {
		// std::cout << "MessageFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getContent();
	}
};

/**
*	@brief 返回日志级别
*/
class LevelFormatItem : public LogFormatter::FormatItem {
public:
	LevelFormatItem(const std::string& str = "") {
		// std::cout << "LevelFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << LogLevel::ToString(level);
	}
};

/**
*	@brief 返回启动时间
*/
class ElapseFormatItem : public LogFormatter::FormatItem {
public:
	ElapseFormatItem(const std::string& str = "") {
		// std::cout << "ElapseFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getElapse();
	}
};

/**
*	@brief 返回日志名称
*/
class NameFormatItem : public LogFormatter::FormatItem {
public:
	NameFormatItem(const std::string& str = "") {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getLogger()->getName();
	}
};

/**
*	@brief 返回进程ID
*/
class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
	ThreadIdFormatItem(const std::string& str = "") {
		// std::cout << "ThreadIdFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getThreadId();
	}
};

/**
*	@brief 返回协程ID
*/
class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
	FiberIdFormatItem(const std::string& str = "") {
		// std::cout << "FiberIdFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getFiberId();
	}
};

/**
*	@brief 返回进程名
*/
class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
	ThreadNameFormatItem(const std::string& str = "") {
		// std::cout << "ThreadNameFormatItem constructor" << std::endl;
	}
	void format(std::ostream&os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getThreadName();
	}
};

/**
*	@brief 返回特定样式的时间
*/
class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
	DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
		:m_format(format) {
		if (m_format.empty()) {
			m_format = "%Y-%m-%d %H:%M:%S";
		}
	}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		struct tm tm; // 临时定义的结构体类型,非引用
		time_t time = event->getTime();
		localtime_r(&time, &tm); // 线程安全
		char buf[64];
		strftime(buf, sizeof(buf), m_format.c_str(), &tm);
		os << buf;
	}
private:
	std::string m_format;
};

/**
*	@brief 返回文件名
*/
class FilenameFormatItem : public LogFormatter::FormatItem {
public:
	FilenameFormatItem(const std::string& str = "") {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getFile();
	}
};

/**
*	@brief 返回行号
*/
class LineFormatItem : public LogFormatter::FormatItem {
public:
	LineFormatItem(const std::string& str = "") {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << event->getLine();
	}
};

/**
*	@brief 返回换行符
*/
class NewLineFormatItem : public LogFormatter::FormatItem {
public:
	NewLineFormatItem(const std::string& str = "") {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << std::endl;
	}
};

/**
*	@brief 字符串
*/
class StringFormatItem : public LogFormatter::FormatItem {
public:
	StringFormatItem(const std::string& str)
		:m_string(str) {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << m_string;
	}
private:
	std::string m_string;
};

/**
*	@brief 输出一个制表符
*/
class TabFormatItem : public LogFormatter::FormatItem {
public:
	TabFormatItem(const std::string& str = "") {}
	void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
		os << "\t";
	}
private:
	std::string m_string;
};

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t thread_id,
	uint32_t fiber_id, uint64_t time, const std::string& thread_name)
	:m_logger(logger)
	,m_level(level)
	,m_file(file)
	,m_line(line)
	,m_elapse(elapse)
	,m_threadId(thread_id)
	,m_fiberId(fiber_id)
	,m_time(time)
	,m_threadName(thread_name)
{
//	std::cout << m_file << " - " << m_line << " - "
//		<< m_elapse << " - " << m_threadId << " - "
//		<< m_fiberId <<  std::endl;
}

Logger::Logger(const std::string& name)
	:m_name(name) /// defalut value is "root"
	,m_level(LogLevel::DEBUG) {
	// std::cout << "m_name = " << m_name << std::endl;
	/**
	*	以 ptr 所指向的对象替换被管理对象
	*	销毁旧的实例
	*/
	m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n")); // 默认的日志级别
}

void Logger::setFormatter(LogFormatter::ptr val) {
    MutexType::Lock lock(m_mutex);
    m_formatter = val;

    /// 改root formatter 影响到AppendFormatter
    for(auto& i : m_appenders) {
        MutexType::Lock ll(i->m_mutex);
        if(!i->m_hasFormatter) {
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val) {
    std::cout << "---" << val << std::endl;
    ipmsg::LogFormatter::ptr new_val(new ipmsg::LogFormatter(val));
    if(new_val->isError()) {
        std::cout << "Logger Formatter name=" << m_name
                  << " value =" << val << " invalid formatter"
                  << std::endl;
        return;
    }
    // m_formatter = new_val;
    setFormatter(new_val);
}

std::string Logger::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    // std::cout << std::endl << "name =" << m_name << std::endl;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    // std::cout << std::endl << "level =" << node["level"] << std::endl;
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    // std::cout << std::endl << "formatter =" << node["formatter"] << std::endl;
    /**
     *  /// 日志目标集合
     *  std::list<LogAppender::ptr> m_appenders;
     */
    for(auto& i : m_appenders) {
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }

    std::stringstream ss;
    // std::cout << "===================" << "Logger::toYamlString()" << std::endl;
    ss << node;
    // std::cout << "=================== finish" << std::endl;
    return ss.str();
}

LogFormatter::ptr Logger::getFormatter()
{
    MutexType::Lock lock(m_mutex);
	return m_formatter;
}

void Logger::addAppender(LogAppender::ptr appender)
{
    MutexType::Lock lock(m_mutex);
	// m_appenders is a list
	if (!appender->getFormatter()) {
        MutexType::Lock ll(appender->m_mutex);
		appender->m_formatter = m_formatter;
        // appender->setFormatter(m_formatter);
	}
	m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
	// m_appenders is a list
	for (auto it = m_appenders.begin(); it != m_appenders.end(); it++) {
		if (*it == appender) {
			m_appenders.erase(it);
			break;
		}
	}
}

void Logger::clearAppenders()
{
    MutexType::Lock lock(m_mutex);
	// m_appenders is a list
	m_appenders.clear();
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
	if (level >= m_level) {
		auto self = shared_from_this(); /// 当类A被shared_ptr管理，且在类A的成员函数里需要把当前类对象作为参数传给其他函数时，就需要传递一个指向自身的share_ptr。
		MutexType::Lock lock(m_mutex);
		if (!m_appenders.empty()) { /// m_appenders 不为空的时候
			for (auto &i : m_appenders) {
				i->log(self, level, event); // enable_shared_from_this, call LogAppender::log
			}
		} else if (m_root) { /// 如果m_appender为空且m_root不为空的时候
			m_root->log(level, event);
		}
	}
}

void Logger::debug(LogEvent::ptr event) {
	log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
	log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
	log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
	log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
	log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string & filename)
	:m_filename(filename) {
	reopen();
}

// void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	if (level >= m_level) {
        /// 解决文件被删除后还在 输出在已被删除的文件中的问题的解决方案
        uint64_t now = time(0);
        if(now != m_lastTime) {
            reopen();
            m_lastTime = now;
            // std::cout << m_filename << std::endl;
        }
        MutexType::Lock lock(m_mutex);
		if (!(m_formatter->format(m_filestream, logger, level, event))) {   /// 写入
			std::cout << "error" << std::endl;
		}
	}
}

std::string FileLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }

    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    // std::cout << "===================" << "FileLogAppender::toYamlString()" << std::endl;
    ss << node;
    // std::cout << "=================== finish" << std::endl;
    return ss.str();
}

bool FileLogAppender::reopen()
{
    MutexType::Lock lock(m_mutex);
	/**
	*	m_filestream -- 文件流
	*	ofstream -- 写文件 / 不能读
	*   ifstream -- 读文件 / 不能写
	*   ostream 是 ofstream 的基类, ofstream专门处理文件流
	*/
	if (m_filestream) {	// 文件流
		m_filestream.close();
	}

	m_filestream.open(m_filename, std::ios::app);
	if (!m_filestream)
		std::cout << "!!!! open error !!!!" << std::endl;

	return !!m_filestream;
}

// void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	// m_level 初始化为 LogLevel::DEBUG
	if (level >= m_level) {
        MutexType::Lock lock(m_mutex);
		m_formatter->format(std::cout, logger, level, event);
	}
}

std::string StdoutLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }

    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    // std::cout << "===================" << "StdoutLogAppender::toYamlString()" << std::endl;
    ss << node;
    // std::cout << "=================== finish" << std::endl;
    return ss.str();
}

LogFormatter::LogFormatter(const std::string& pattern)
	:m_pattern(pattern) {
	init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level,
	LogEvent::ptr event) {
	std::stringstream ss;
	for (auto& i : m_items) {
		i->format(ss, logger, level, event);
	}
	return ss.str();
}

std::ostream& LogFormatter::format(std::ostream & ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	for (auto& i : m_items) {
		i->format(ofs, logger, level, event); // 调用子类的format 方法
	}
	return ofs;
}

/**
* @brief 初始化,解析日志模板
* %d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
*/
void LogFormatter::init() {
	/// 格式 %xxx %xxx{xxx} %%
	/// string, format, type
	std::vector<std::tuple<std::string, std::string, int> > vec;
	std::string str;
	for (size_t i = 0; i < m_pattern.size(); ++i) {
		if (m_pattern[i] != '%') {
			str.append(1, m_pattern[i]);
			continue;
		}

		// %% 的类型
		if ((i + 1) < m_pattern.size()) {
			if (m_pattern[i + 1] == '%') {
				str.append(1, '%');
				continue;
			}
		}

		size_t pos = i + 1;
		size_t fmt_begin = 0;
		// 初始状态
		int fmt_status = 0;
		std::string iwstr;
		std::string iwfmt;

		// %xxx / %xxx{xxx} 的类型
		while (pos < m_pattern.size()) {
			// 如果是空格,退出循环
			if (!fmt_status && (!isalpha(m_pattern[pos]) && m_pattern[pos] != '{'
				&& m_pattern[pos] != '}')) {
				iwstr = m_pattern.substr(i + 1, pos - i - 1);
				break;
			}

			if (fmt_status == 0) {
				if (m_pattern[pos] == '{') {
					iwstr = m_pattern.substr(i + 1, pos - i - 1);
					// std::cout << "start analysis --  " << iwstr << std::endl;
					fmt_begin = pos;
					fmt_status = 1; /// 解析状态
					++pos;
					continue;
				}
			}
			else if (fmt_status == 1) {
				if (m_pattern[pos] == '}') {
					iwfmt = m_pattern.substr(fmt_begin + 1, pos - fmt_begin - 1);
					fmt_status = 0; /// 解析结束
					++pos;
					break;
				}
			}

			++pos;
			if (pos == m_pattern.size()) {
				if (iwstr.empty()) {
					iwstr = m_pattern.substr(i + 1); // 截取从i + 1 后的所有字符串
					// std::cout << "Debug<iwstr> : " << iwstr << std::endl;
				}
			}
		}

		/// 正确解析
		if (fmt_status == 0) {
			if (!str.empty()) {
				vec.push_back(std::make_tuple(str, std::string(), 0));
				str.clear();
			}
			vec.push_back(std::make_tuple(iwstr, iwfmt, 1));
			i = pos - 1;
		}
		/// 解析出问题
		else if (fmt_status == 1) {
			std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
			m_error = true;
			vec.push_back(std::make_tuple("<< pattern_error >> ", iwfmt, 0));
		}
	}

	if (!str.empty()) {
		vec.push_back(std::make_tuple(str, "", 0));
	}


	static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, c) \
		{ #str, [](const std::string& fmt) { return FormatItem::ptr(new c(fmt)); }}

		XX(m, MessageFormatItem),           //m:消息
		XX(p, LevelFormatItem),             //p:日志级别
		XX(r, ElapseFormatItem),            //r:累计毫秒数
		XX(c, NameFormatItem),              //c:日志名称
		XX(t, ThreadIdFormatItem),          //t:线程id
		XX(n, NewLineFormatItem),           //n:换行
		XX(d, DateTimeFormatItem),          //d:时间
		XX(f, FilenameFormatItem),          //f:文件名
		XX(l, LineFormatItem),              //l:行号
		XX(T, TabFormatItem),               //T:Tab
		XX(F, FiberIdFormatItem),           //F:协程id
		XX(N, ThreadNameFormatItem),        //N:线程名称

};

	/**
	*	将解析放入 m_items
	*/
	for (auto& i : vec) {
		if (std::get<2>(i) == 0) {
			m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
		}
		else {
			auto it = s_format_items.find(std::get<0>(i));
			if (it == s_format_items.end()) {
				m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
				m_error = true;
			}
			else {
				m_items.push_back(it->second(std::get<1>(i)));
			}
		}
		 // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
	}
}

LoggerManager::LoggerManager() {
    /// m_root : 主日志器
	m_root.reset(new Logger);
	m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

	/**
	 *    m_loggers : std::map<std::string, Logger::ptr> m_loggers; // 日志器容器
	 *    m_root    : Logger::ptr m_root; // 主日志器
	 *    m_name    : std::string m_name; (Logger 类下) // 日志名称
	 *    Logger的友元类 -> LoggerManager
	 */
	m_loggers[m_root->m_name] = m_root;
	// std::cout << "m_root->name :" << m_root->m_name << std::endl;

	init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name)
{
    MutexType::Lock lock(m_mutex);
    /// m_loggers : std::map<std::string, Logger::ptr> m_loggers; // 日志器容器
	auto it = m_loggers.find(name);
	if (it != m_loggers.end()) {
		return it->second;  /* return logger;*/
	}

	/// 如果m_loggers不存在， 那么我们创建一个新的Logger
	Logger::ptr logger(new Logger(name));
	logger->m_root = m_root;
	m_loggers[name] = logger;
	return logger;
}

struct LogAppenderDefine {
    int type = 0; /// 1 - FileLogAppender  2 - StdoutLogAppender
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.level
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
	std::string name;
	LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};


template<> /// 模板特化
class LexicalCast<std::string, std::set<LogDefine> > {
public:
    std::set<LogDefine> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::set<LogDefine> set;
        // std::cout << "node =" << node << std::endl << "finish" << std::endl;
        /// 进入"logs" IsSequence()
        for(size_t i = 0; i < node.size(); i++) {
            // std::cout << node.size() << std::endl << "=======================" << std::endl;
            auto n = node[i]; /// node[i] = node["log"][i]
            if(!n["name"].IsDefined()) {
                std::cout << "log config error : name is null, " << n << std::endl;
                continue;
            }
            LogDefine ld;
            ld.name = n["name"].as<std::string>(); /// n["name"] = node["log"][i]["name"]
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            /// n["formatter"] = node["log"][i]["formatter"]
            if(n["formatter"].IsDefined()) {
                ld.formatter = n["formatter"].as<std::string>();
            }
            /// n["appenders"] = node["log"][i]["appenders"]
            if(n["appenders"].IsDefined()) {
                for(size_t x = 0; x < n["appenders"].size(); ++x) {
                    // std::cout << __FILE__ << __LINE__ << "appenders node count " << n["appenders"].size() << std::endl;
                    /// node["log"][i]["appenders"][0] || node["log"][i]["appenders"][1]
                    auto a = n["appenders"][x];
                    /// /// node["log"][i]["appenders"][0]["type"] node["log"][i]["appenders"][0]["file"]
                    if(!a["type"].IsDefined()) {
                        std::cout << "log config error : fileappender type is null, " << a << std::endl;
                        continue;
                    }
                    std::string type = a["type"].as<std::string>(); /// 转换成string
                    LogAppenderDefine lad;
                    if(type == "FileLogAppender") {
                        lad.type = 1;
                        if(!a["file"].IsDefined()) {
                            std::cout << "log config error: fileappender file is null, " << a<< std::endl;
                            continue;
                        }
                        lad.file = a["file"].as<std::string>();
                        if(a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    }else if(type == "StdoutLogAppender") {
                        lad.type = 2;
                        if(a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                        // std::cout << "log config error: stdoutlogappender type is invaild, " << a << std::endl;
                    }else {
                        std::cout << "log config error: appender type is invalid, " << a << std::endl;
                        continue;
                    }

                    ld.appenders.push_back(lad);

                }
            }
            set.insert(ld);
        }
        return set;
    }
};

/**
 *  @brief 对vector容器的偏特化，vector类型转换成string类型
 */
template<> /// 模板特化
class LexicalCast<std::set<LogDefine>, std::string> {
public:
    std::string operator()(const std::set<LogDefine>& var) {
        YAML::Node node;
        for(auto &i : var) {
            YAML::Node n;
            n["name"] = i.name;
            if(i.level != LogLevel::UNKNOW) {
                n["level"] = LogLevel::ToString(i.level);
            }

            if(i.formatter.empty()) {
                n["formatter"] = i.formatter;
            }

            for(auto& a : i.appenders) {
                YAML::Node na;
                if(a.type == 1) {
                   na["type"] = "FileLogAppender";
                   na["file"] = a.file;
                }
                else if(a.type == 2) {
                    na["type"] = "StdoutLogAppender";
                }

                if(a.level != LogLevel::UNKNOW) {
                    na["level"] = LogLevel::ToString(a.level);
                }
                if(!a.formatter.empty()) {
                    na["formatter"] = a.formatter;
                }
                n["appenders"].push_back(na);
            }
            node.push_back(n);
        }

        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

ipmsg::ConfigVar<std::set<LogDefine> >::ptr g_log_defines =
    ipmsg::Config::Lookup("logs", std::set<LogDefine>(), "logs config");


struct LogIniter {
public:
    /**
     *  @brief 添加事件
     */
    LogIniter() {
        // g_log_defines->addListener(0xF1E231,[](const std::set<LogDefine>& old_value,
        g_log_defines->addListener([](const std::set<LogDefine>& old_value,
                                const std::set<LogDefine>& new_value){

                LOG_INFO(LOG_ROOT()) << "on_logger_conf_changed";
                /// 1. 新增, i -- new value 的值
                /// 新增 -- new value 里有, old_value里没有
                /// 修改 -- new value 里有， old value 里也有, logger 是否发生变化
                for(auto &i : new_value) {
                    auto it = old_value.find(i);
                    ipmsg::Logger::ptr logger;

                    if(it == old_value.end()) { /// 找不到值
                        /// 新增logger
                        // logger.reset(new ipmsg::Logger(i.name));
                        std::cout << "new name :" << i.name << std::endl;
                        logger = LOG_NAME(i.name);
                    }
                    else {
                        if(!(i == *it)) {
                            /// 修改的logger
                           logger = LOG_NAME(i.name);
                           // logger->setFormatter(i.formatter);
                        }
                    }

                    logger->setLevel(i.level);
                    if(!i.formatter.empty()){
                        logger->setFormatter(i.formatter);
                    }

                        /// 清除appenders
                    logger->clearAppenders();
                        /// 添加新的appenders
                    for(auto& a : i.appenders){
                        ipmsg::LogAppender::ptr ap;
                        if(a.type == 1) {
                            ap.reset(new FileLogAppender(a.file));
                        }else if(a.type == 2) {
                            ap.reset(new StdoutLogAppender);
                        }

                        ap->setLevel(a.level);
                        if(!a.formatter.empty()) {
                            LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                            if(!fmt->isError()) {
                                ap->setFormatter(fmt);
                            } else {
                                std::cout << std::endl << __FILE__ << __LINE__ << std::endl << "log.name" << i.name << " appender type = " << a.type << " formatter = " << a.formatter << "is invaild" << std::endl;
                            }
                        }
                        logger->addAppender(ap);
                    }

                }

                /// 删除 -- new value 里有, old_value里也有
                for(auto &i : old_value) {
                    auto it = new_value.find(i);
                    if(it == old_value.end()) {
                        /// 删除logger
                        auto logger = LOG_NAME(i.name);
                        logger->setLevel((LogLevel::Level)100);
                        logger->clearAppenders();
                    }
                }
        });
    }
};

/**
*   @brief 全局对象在main函数之前构造,调用构造函数，触发main函数构造事件
*/
static LogIniter __log_init;

std::string LoggerManager::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    for(auto& i : m_loggers) {
        // std::cout << std::endl << i.second->toYamlString()  << std::endl;

        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void LoggerManager::init() {}
}
