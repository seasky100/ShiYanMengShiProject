/*!
 * @file SystemLog.h
 * @brief 系统运行日志文件
 * @author FanKaiYu
 * @date 2019-03-21
 * @version 2.0
*/

#pragma once
#ifndef _SYSTEMLOG_H
#define _SYSTEMLOG_H

#include <string>
#include <chrono>
#include <cinttypes>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdarg.h>
#include <io.h>

using namespace std;

namespace SystemLog
{
	static string g_strLogPath = "../SystemLog";    /*!< 存放系统日志的目录 */

	/*!
	 * @brief 检测文件是否存在
	 * @param const char* 文件路径
	 * @return bool 文件存在返回true,否则返回false
	*/
	static bool IsFileExisted(const char* strPath)
	{
		if (::_access(strPath, 0) == -1)
		{
			return false;
		}

		return true;
	}

	/*!
	 * @brief 检测文件是否存在
	 * @param const wchar_t* 文件路径
	 * @return bool 文件存在返回true,否则返回false
	*/
	static bool IsFileExisted(const wchar_t* wstrPath)
	{
		if (::_waccess(wstrPath, 0) == -1)
		{
			return false;
		}

		return true;
	}

	/*!
	 * @brief 检测文件是否存在
	 * @param const string 文件路径
	 * @return bool 文件存在返回true,否则返回false
	*/
	static bool IsFileExisted(const string strPath)
	{
		return IsFileExisted(strPath.c_str());
	}

	/*!
	 * @brief 检测文件是否存在
	 * @param const wstring 文件路径
	 * @return bool 文件存在返回true,否则返回false
	*/
	static bool IsFileExisted(const wstring wstrPath)
	{
		return IsFileExisted(wstrPath.c_str());
	}

	/*!
	 * @brief 创建文件
	 * @param const char* 文件路径
	 * @return bool 创建成功true,否则返回false
	*/
	static bool CreateFileEx(const char* strPath)
	{
		char* _str = new char[strlen(strPath) + 1];
		memset(_str, 0, strlen(strPath) + 1);
		memcpy_s(_str, strlen(strPath), strPath, strlen(strPath));

		for (unsigned int i = 0; i < strlen(_str); ++i)
		{
			if (_str[i] == '/')
			{
				_str[i] = '\\';
			}
		}

		stringstream ss;

		char* pLast = nullptr;
		char* pNext = _str;

		pNext = strchr(pNext, '\\');

		// 存在目录
		if (pNext)
		{
			// 查找最后一级目录
			while (true)
			{
				pNext = strchr(pNext, '\\');
				if (pNext)
				{
					pLast = pNext;
					pNext += 1;
				}
				else
				{
					break;
				}
			}

			if (pLast)
			{
				// 存在文件
				if (strchr(pLast, '.'))
				{
					char* _path = new char[pLast - _str + 1];
					memset(_path, 0, pLast - _str + 1);
					memcpy_s(_path, pLast - _str, _str, pLast - _str);

					if (strcmp(_path, ".") != 0 && strcmp(_path,"..") != 0)
					{
						ss << "mkdir " << _path;

						delete[] _path;

						// 创建目录
						system(ss.str().c_str());
					}
				}
				// 不存在文件
				else
				{
					ss << "mkdir " << _str;

					// 创建目录
					system(ss.str().c_str());
					delete[] _str;

					return true;
				}		
			}
		}

		// 创建文件
		FILE* pReadFile = nullptr;  /*!< 打开文件*/

		fopen_s(&pReadFile, _str, "w+");

		if (pReadFile == nullptr)
		{
			delete[] _str;
			return false;
		}

		fclose(pReadFile);

		delete[] _str;
		return true;
	}

	/*!
	 * @brief 创建文件
	 * @param const wchar* 文件路径
	 * @return bool 创建成功true,否则返回false
	*/
	static bool CreateFileEx(const wchar_t* wstrPath)
	{
		wchar_t* _wstr = new wchar_t[wcslen(wstrPath) + 1];
		wmemset(_wstr, L'\0', wcslen(wstrPath) + 1);
		wmemcpy_s(_wstr, wcslen(wstrPath), wstrPath, wcslen(wstrPath));

		for (unsigned int i = 0; i < wcslen(_wstr); ++i)
		{
			if (_wstr[i] == L'/')
			{
				_wstr[i] = L'\\';
			}
		}

		wstringstream wss;

		wchar_t* pLast = nullptr;
		wchar_t* pNext = _wstr;

		pNext = wcschr(pNext, L'\\');

		// 存在目录
		if (pNext)
		{
			// 查找最后一级目录
			while (true)
			{
				pNext = wcschr(pNext, L'\\');
				if (pNext)
				{
					pLast = pNext;
					pNext += 1;
				}
				else
				{
					break;
				}
			}

			if (pLast)
			{
				// 存在文件
				if (wcschr(pLast, L'.'))
				{
					wchar_t* _path = new wchar_t[pLast - _wstr + 1];
					wmemset(_path, 0, pLast - _wstr + 1);
					wmemcpy_s(_path, pLast - _wstr, _wstr, pLast - _wstr);

					if (wcscmp(_path, L".") != 0 && wcscmp(_path, L"..") != 0)
					{
						wss << L"mkdir " << _path;

						delete[] _path;

						_wsystem(wss.str().c_str());
					}
				}
				// 不存在文件
				else
				{
					wss << L"mkdir " << _wstr;

					// 创建目录
					_wsystem(wss.str().c_str());
					delete[] _wstr;

					return true;
				}
			}
		}

		// 创建文件
		FILE* pReadFile = nullptr;  /*!< 打开文件*/

		_wfopen_s(&pReadFile, _wstr, L"w+");

		if (pReadFile == nullptr)
		{
			delete[] _wstr;
			return false;
		}

		fclose(pReadFile);

		delete[] _wstr;
		return true;
	}

	/*!
	 * @brief 创建文件
	 * @param const string 文件路径
	 * @return bool 创建成功true,否则返回false
	*/
	static bool CreateFileEx(const string strPath)
	{
		return CreateFileEx(strPath.c_str());
	}

	/*!
	 * @brief 创建文件
	 * @param const wstring 文件路径
	 * @return bool 创建成功true,否则返回false
	*/
	static bool CreateFileEx(const wstring wstrPath)
	{
		return CreateFileEx(wstrPath.c_str());
	}

#ifdef _AFX
	/*!
	 * @brief 创建文件
	 * @param const CString 文件路径
	 * @return bool 创建成功true,否则返回false
	*/
	static bool CreateFileEx(CString cstrPath)
	{
		return CreateFileEx(cstrPath.GetBuffer());
	}
#endif //_AFX

    /*!
     * @brief 记录系统日志
     * @param const char* 日志文本
     * @retrun bool 记录成功返回true,否则返回false
    */
	static bool Record(const char* strText)
    {
        if(strlen(strText) == 0)
        {
            return false;
        }

		// 获取系统当前时间
		std::stringstream ss;
		std::chrono::system_clock::time_point tpNow = std::chrono::system_clock::now();
		auto t = std::chrono::system_clock::to_time_t(tpNow);
		// 获取日期
		tm tmTime;
		localtime_s(&tmTime, &t);
		// 合成路径
		ss << g_strLogPath << "/" << std::put_time(&tmTime, "%Y%m%d") << "_系统日志.log";

		if (IsFileExisted(ss.str().c_str()) == false)
		{
			if (CreateFileEx(ss.str().c_str()) == false)
			{
				return false;
			}
		}

		// 打开文件
		FILE* pReadFile = nullptr;	/*!< 文件指针 */

		fopen_s(&pReadFile, ss.str().c_str(), "a+");

		if (pReadFile == nullptr)
		{
			return false;
		}

		ss.str("");

		// 定位至文件尾部
		fseek(pReadFile, 0, SEEK_END);

		ss << "[" << std::put_time(&tmTime, "%Y-%m-%d %X") << "]\n->>" << strText << "\n";

		fwrite(ss.str().c_str(), sizeof(char), ss.str().length(), pReadFile);

		fclose(pReadFile);

		return true;
    }

    /*!
     * @brief 记录系统日志
     * @param const wchar_t* 日志文本
     * @retrun bool 记录成功返回true,否则返回false
    */
	static bool Record(const wchar_t* wstrText)
	{
		if (wcslen(wstrText) == 0)
		{
			return false;
		}

		string _str = "";	/*!< ACSII字符串 */
#ifdef _ATL
		_str = (char*)_bstr_t(wstrText);
#else
		//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
		int len = WideCharToMultiByte(CP_ACP, 0, wstrText, (int)wcslen(wstrText), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		//宽字节编码转换成多字节编码  
		WideCharToMultiByte(CP_ACP, 0, wstrText, (int)wcslen(wstrText), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		//删除缓冲区并返回值  
		_str.append(buffer);
		delete[] buffer;
#endif // !_ATL

		return Record(_str.c_str());
	}

    /*!
     * @brief 记录系统日志
     * @param string 日志文本
     * @retrun bool 记录成功返回true,否则返回false
    */
	static bool Record(const string strText)
    {
        return Record(strText.c_str());
    }

    /*!
     * @brief 记录系统日志
     * @param wstring 日志文本
     * @retrun bool 记录成功返回true,否则返回false
    */
	static bool Record(const wstring wstrText)
    {
        return Record(wstrText.c_str());
    }

    #ifdef _AFX
    /*!
     * @brief 记录系统日志
     * @param CString 日志文本
     * @retrun bool 记录成功返回true,否则返回false
    */
	static bool Record(CString cstrText)
    {
        return Record(cstrText.GetBuffer());
    }
    #endif //_AFX

	/*!
 * @brief 记录系统日志
 * @param const char*
 * @return bool 记录成功返回true,否则返回false
 */
	static bool Recordf(const char* _Format, ...)
	{
		std::string tmp;

		va_list marker = NULL;
		va_start(marker, _Format);

		size_t num_of_chars = _vscprintf(_Format, marker);

		if (num_of_chars > tmp.capacity())
		{
			tmp.resize(num_of_chars + 1);
		}

		vsprintf_s((char *)tmp.data(), tmp.capacity(), _Format, marker);

		va_end(marker);

		return Record(tmp.c_str());
	}

	/*!
	 * @brief 记录系统日志
	 * @param const wchar_t*
	 * @return bool 记录成功返回true,否则返回false
	 */
	static bool Recordf(const wchar_t* _Format, ...)
	{
		std::wstring tmp;

		va_list marker = NULL;
		va_start(marker, _Format);

		size_t num_of_chars = _vscwprintf(_Format, marker);

		if (num_of_chars > tmp.capacity())
		{
			tmp.resize(num_of_chars + 1);
		}

		vswprintf_s((wchar_t *)tmp.data(), tmp.capacity(), _Format, marker);

		va_end(marker);

		return Record(tmp.c_str());
	}

    /*!
     * @brief 设置文件路径
     * @param const char* 文件路径
     * @return bool 设置成功返回true,否则返回false
    */
   static bool SetPath(const char* strPath)
   {
       if(strlen(strPath) == 0)
       {
           return false;
       }

	   g_strLogPath = strPath;

       return true;
   }

   /*!
	* @brief 设置文件路径
	* @param const wchar_t* 文件路径
	* @return bool 设置成功返回true,否则返回false
   */
   static bool SetPath(const wchar_t* wstrPath)
   {
	   if (wcslen(wstrPath) == 0)
	   {
		   return false;
	   }

#ifdef _ATL
	   g_strLogPath = (char*)_bstr_t(wstrPath);
#else
		//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	   int len = WideCharToMultiByte(CP_ACP, 0, wstrPath, (int)wcslen(wstrPath), NULL, 0, NULL, NULL);
	   char* buffer = new char[len + 1];
	   //宽字节编码转换成多字节编码  
	   WideCharToMultiByte(CP_ACP, 0, wstrPath, (int)wcslen(wstrPath), buffer, len, NULL, NULL);
	   buffer[len] = '\0';
	   //删除缓冲区并返回值  
	   g_strLogPath.append(buffer);
	   delete[] buffer;
#endif // !_ATL

	   return true;
   }

   /*!
	* @brief 设置文件路径
	* @param const string 文件路径
	* @return bool 设置成功返回true,否则返回false
   */
   static bool SetPath(const string strPath)
   {
	   return SetPath(strPath.c_str());
   }

   /*!
	* @brief 设置文件路径
	* @param const wstring 文件路径
	* @return bool 设置成功返回true,否则返回false
   */
   static bool SetPath(const wstring wstrPath)
   {
	   return SetPath(wstrPath.c_str());
   }

#ifdef _AFX
   /*!
	* @brief 设置文件路径
	* @param const CString 文件路径
	* @return bool 设置成功返回true,否则返回false
   */
   static bool SetPath(CString cstrPath)
   {
	   return SetPath(cstrPath.GetBuffer());
   }
#endif //_AFX
}

#endif //!_SYSTEMLOG_H