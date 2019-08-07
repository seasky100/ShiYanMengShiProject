/*!
 * @file ThreadEx.h
 * @brief 线程扩展文件
 * @author FanKaiyu
 * @date 2019-04-03
 * @version 1.0
 */

#pragma once
#ifndef _THREADEX_H
#define _THREADEX_H

#include <thread>
#include <sstream>

using namespace std;

namespace ThreadEx
{
    /*!
	 * @brief 获取线程ID
	 * @param thread* 线程指针
	 * @return unsigned long 线程ID
	*/
	static unsigned long GetThreadID(thread* pThread)
	{
		if (pThread == nullptr)
		{
			return 0;
		}

		thread::id thID = pThread->get_id();
		stringstream ss;
		ss << thID;

		return std::stoul(ss.str());
	}
};

#endif //!_THREADEX_H