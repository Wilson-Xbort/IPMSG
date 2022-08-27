#pragma once
/**
*	@header  该头文件用于存放公共方法
*   @time	 创建时间为： 2020-12-19
*/

#include <sys/types.h> // pid_t
#include <iostream> // unit32_t
#include <string>
#include <vector>


namespace ipmsg {

pid_t GetThreadId();
uint32_t GetFiberId();

void Backtrace(std::vector<std::string>&bt, int size, int skip = 1);
/**
 * @brief 获取当前栈信息的字符串
 * @param[in] size 栈的最大层数
 * @param[in] skip 跳过栈顶的层数
 * @param[in] prefix 栈信息前输出的内容
 */
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
}


