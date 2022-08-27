#pragma once
/**
*	@header  ��ͷ�ļ����ڴ�Ź�������
*   @time	 ����ʱ��Ϊ�� 2020-12-19
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
 * @brief ��ȡ��ǰջ��Ϣ���ַ���
 * @param[in] size ջ��������
 * @param[in] skip ����ջ���Ĳ���
 * @param[in] prefix ջ��Ϣǰ���������
 */
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
}


