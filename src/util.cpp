#include "util.h"
#include <sys/syscall.h> // SYS_gettid
#include <sys/types.h> // pid_t
#include <unistd.h> // syscall
#include <execinfo.h>
#include "ipmsg.h"
namespace ipmsg {

ipmsg::Logger::ptr g_logger = LOG_NAME("system");

pid_t GetThreadId() {
	return syscall(static_cast<long>(SYS_gettid));
}

uint32_t GetFiberId() {
	return ipmsg::Fiber::GetFiberId();
}


/**
 * @brief 获取当前的调用栈
 * @param[out] bt 保存调用栈
 * @param[in] size 最多返回层数
 * @param[in] skip 跳过栈顶的层数
 */
void Backtrace(std::vector<std::string>&bt, int size, int skip) {
    void** array = (void**)malloc((sizeof(void *) * size));
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if(strings == NULL) {
        LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }

    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); i++) {
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}


}
