/*!
 * @file DebugPrint.h
 * @brief 用以在DEBUG调试时输出调试信息的函数封装
 * @author FanKaiYu
 * @date 2019-03-20
 * @version 2.0
*/

#pragma once
#ifndef _DEBUGPRINT_H
#define _DEBUGPRINT_H

#include <Windows.h>
#include <string>
#include <chrono>
#include <cinttypes>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdarg.h>

using namespace std;
using namespace chrono;

namespace DebugPrint
{
	static void Printf(const char *clpszFormat, ...)
	{
		string tmp;

		va_list marker = NULL;
		va_start(marker, clpszFormat);

		size_t num_of_chars = _vscprintf(clpszFormat, marker);

		if (num_of_chars > tmp.capacity()) 
		{
			tmp.resize(num_of_chars + 1);
		}

		vsprintf_s((char *)tmp.data(), tmp.capacity(), clpszFormat, marker);

		va_end(marker);

		// 获取系统当前时间
		stringstream ss;
		system_clock::time_point tpNow = system_clock::now();
		auto t = system_clock::to_time_t(tpNow);
		// 获取毫秒数
		auto m = tpNow.time_since_epoch();
		auto diff = duration_cast<microseconds>(m).count();
		auto const msecs = diff % 1000;
		// 获取日期
		tm tmTime;
		localtime_s(&tmTime, &t);
		// 合成文本
		ss << put_time(&tmTime, "%Y-%m-%d %X") << ":" << msecs << " " << tmp.c_str() << "\n\n";

		// 输出字符串
		OutputDebugStringA(ss.str().c_str());

		return;
	}

	static void Printf(const wchar_t *clpszFormat, ...)
	{
		std::wstring tmp;

		va_list marker = NULL;
		va_start(marker, clpszFormat);

		size_t num_of_chars = _vscwprintf(clpszFormat, marker);

		if (num_of_chars > tmp.capacity()) 
		{
			tmp.resize(num_of_chars + 1);
		}

		vswprintf_s((wchar_t *)tmp.data(), tmp.capacity(), clpszFormat, marker);

		va_end(marker);

		// 获取系统当前时间
		wstringstream wss;
		system_clock::time_point tpNow = system_clock::now();
		auto t = system_clock::to_time_t(tpNow);
		// 获取毫秒数
		auto m = tpNow.time_since_epoch();
		auto diff = duration_cast<std::chrono::microseconds>(m).count();
		auto const msecs = diff % 1000;
		// 获取日期
		tm tmTime;
		localtime_s(&tmTime, &t);
		// 合成文本
		wss << put_time(&tmTime, L"%Y-%m-%d %X") << L":" << msecs << L" " << tmp.c_str() << L"\n\n";

		// 输出字符串
		OutputDebugStringW(wss.str().c_str());

		return;
	}

	static void Print(const char* lpszText)
	{
		DebugPrint::Printf(lpszText);
	}

	static void Print(const wchar_t* lpszText)
	{
		DebugPrint::Printf(lpszText);
	}

	static void Print(const std::string strText)
	{
		DebugPrint::Printf(strText.c_str());
	}

	static void Print(const std::wstring wstrText)
	{
		DebugPrint::Printf(wstrText.c_str());
	}

    #ifdef _AFX
	static void Print(CString cstrText)
	{
		DebugPrint::Printf(cstrText.GetBuffer());
	}
    #endif //_AFX

    #ifdef _ATL
	static void Print(_bstr_t bstrText)
	{
#ifdef _UNICODE
		DebugPrint::Printf((wchar_t*)bstrText);
#else
		DebugPrint::Printf((char*)bstrText);
#endif //_UNICODE
	}
    #endif //_ATL 
}

#endif //!_DEBUGPRINT_H