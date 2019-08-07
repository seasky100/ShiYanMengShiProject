/*
 * @file StringEx.h
 * @brief 字符串功能增强文件，主要用户字符串转换
 * @author FanKaiYu
 * @date 2019-03-20
 * @version 1.0
 */

#pragma once
#ifndef _STRINGEX_H
#define _STRINGEX_H

#include <Windows.h>
#include <string>

using namespace std;

namespace StringEx
{
    /*!
     * @brief 字符串转十进制数组
     * @param const char* 源字符串
     * @return int 转换后的十进制数字
    */
    static int string_to_int(const char* _str)
	{
		int nSum = 0;
		double x = 0.0f;
		for (int i = (int)strlen(_str) - 1; i > -1; --i, x += 1)
		{
			if (_str[i] == '+' || _str[i] == '-')
			{
				nSum = -nSum;
				break;
			}
			else if (_str[i] < '0' || _str[i] > '9')
			{
				break;
			}
			else
			{
				double dbDem = pow(10, x);
				nSum += (int)((_str[i] - '0') * dbDem);
			}
		}

		return nSum;
	}

	/*!
	 * @brief 字符串转十进制数组
	 * @param const char* 源字符串
	 * @return int 转换后的十进制数字
	*/
	static int wstring_to_int(const wchar_t* _wstr)
	{
		int nSum = 0;
		double x = 0.0f;
		for (int i = (int)wcslen(_wstr) - 1; i > -1; i--, x += 1)
		{
			if (_wstr[i] == L'+' || _wstr[i] == L'-')
			{
				nSum = -nSum;
				break;
			}
			else if (_wstr[i] < L'0' || _wstr[i] > L'9')
			{
				break;
			}
			else
			{
				double dbDem = pow(10, x);
				nSum += (int)((_wstr[i] - L'0') * dbDem);
			}
		}

		return nSum;
	}

    /*!
     * @brief 字符替换
     * @param string& 源字符串
     * @param char 要替换的字符
     * @param char 替换后的字符
    */
    static void string_replace(string& _str,const char* c,const char* r) 
	{
		string::size_type pos = 0;
		while ((pos = _str.find(c, pos)) != string::npos)
		{
			_str.replace(pos, strlen(c), r);
			pos += strlen(r);
		}

		return;
	}

	/*!
     * @brief 字符替换
     * @param wstring& 源字符串
     * @param char 要替换的字符
     * @param char 替换后的字符
    */
    static void wstring_replace(wstring& _wstr,const wchar_t* c,const wchar_t* r)
	{
		wstring::size_type pos = 0;
		while ((pos = _wstr.find(c, pos)) != wstring::npos)
		{
			_wstr.replace(pos, wcslen(c), r);
			pos += wcslen(r);
		}
		return;
	}

	/*!
	 * @brief 格式化字符串
	 * @param string& 格式化后的字符串
	 * @param const char* 格式串
	*/
	static string string_format(const char* _Format, ...)
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


		return tmp.c_str();
	}

	/*!
	 * @brief 格式化字符串
	 * @param wstring& 格式化后的字符串
	 * @param const wchar_t* 格式串
	*/
	static wstring string_format(const wchar_t* _Format, ...)
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

		return tmp.c_str();
	}

	/*!
	 * @brief 将ACSII字符串转换为UNICODE字符串
	 * @param const char* ACSII字符串
	 * @return wstring 转换后的UNICODE字符串
	*/
	static wstring string_to_wstring(const char* _str)
	{
		wstring _wstr = L"";	/*!< UNICODE字符串 */
#ifdef _ATL
		_wstr = (wchar_t*)_bstr_t(_str);
#else
		//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
		int len = MultiByteToWideChar(CP_ACP, 0, _str, (int)strlen(_str), NULL, 0);
		wchar_t* buffer = new wchar_t[len + 1];
		//多字节编码转换成宽字节编码  
		MultiByteToWideChar(CP_ACP, 0, _str, (int)strlen(_str), buffer, len);
		buffer[len] = L'\0';             //添加字符串结尾  
		//删除缓冲区并返回值  
		_wstr.append(buffer);
		delete[] buffer;
#endif // !_ATL

		return _wstr;
	}

	static wstring string_to_wstring(const string _str)
	{
		return string_to_wstring(_str.c_str());
	}

	/*!
	 * @brief 将UNICODE字符串转换为ACSII字符串
	 * @param const wchar_t* UNICODE字符串
	 * @return string 转换后的ACSII字符串 
	*/
	static string wstring_to_string(const wchar_t* _wstr)
	{
		string _str = "";	/*!< ACSII字符串 */
#ifdef _ATL
		_str = (char*)_bstr_t(_wstr);
#else
		//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
		int len = WideCharToMultiByte(CP_ACP, 0, _wstr,(int)wcslen(_wstr), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		//宽字节编码转换成多字节编码  
		WideCharToMultiByte(CP_ACP, 0, _wstr, (int)wcslen(_wstr), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		//删除缓冲区并返回值  
		_str.append(buffer);
		delete[] buffer;
#endif // !_ATL

		return _str;
	}

	static string wstring_to_string(const wstring _wstr)
	{
		return wstring_to_string(_wstr.c_str());
	}

#ifdef _AFX
	/*!
	 * @brief 将ACSII字符串存入CString
	 * @param const char* ACSII字符串
	 * @return CString 
	*/
	static CString string_to_CString(const char* _str)
	{
		CString _cstr = _T("");
#ifdef _UNICODE
		_cstr.Format(_T("%s"), string_to_wstring(_str).c_str());
#else
		_cstr.Format(_T("%s"), _str.c_str());
#endif
		return _cstr;
	}

	static CString string_to_CString(const string _str)
	{
		return string_to_CString(_str.c_str());
	}

	/*!
	 * @brief 将UNICODE字符串存入CString
	 * @param const wchar_t* UNICODE字符串
	 * @return CString
	*/
	static CString wstring_to_CString(const wchar_t* _wstr)
	{
		CString _cstr = _T("");
#ifdef _UNICODE
		_cstr.Format(_T("%s"), _wstr);
#else
		_cstr.Format(_T("%s"), wstring_to_string(_wstr).c_str());
#endif
		return _cstr;
	}

	static CString wstring_to_CString(const wstring _wstr)
	{
		return wstring_to_CString(_wstr.c_str());
	}

	/*!
	 * @brief 将CString内的字符串存入string
	 * @param CString
	 * @return string
	*/
	static string CString_to_string(CString _cstr)
	{
		string _str = "";

#ifdef _UNICODE
		_str = CT2A(_cstr.GetBuffer());
#else
		_str = _cstr.GetBuffer();
#endif // !_ATL

		return _str;
	}

	/*!
	 * @brief 将CString内的字符串存入wstring
	 * @param CString
	 * @return wstring
	*/
	static wstring CString_to_wstring(CString _cstr)
	{
		wstring _wstr = L"";
#ifdef _UNICODE
		_wstr = _cstr.GetBuffer();
#else
		_wstr = CT2W(_cstr.GetBuffer());
#endif // !_ATL

		return _wstr;
	}
#endif //_AFX

	/*!
	 * @brief ASCII码字符转换为16进制数
	 * @param chAscii 需要转换的ASCII码字符
	 * @return char 转换后的16进制数
	*/
	static char HexChar(char chAscii)
	{
		if ((chAscii >= '0') && (chAscii <= '9'))
		{
			return chAscii - 0x30;
		}
		else if ((chAscii >= 'A') && (chAscii <= 'F'))
		{
			return chAscii - 'A' + 10;
		}
		else if ((chAscii >= 'a') && (chAscii <= 'f'))
		{
			return chAscii - 'a' + 10;
		}
		else
		{
			return 0x10;
		}
	}

	/*!
	 * @brief 字符串转换为16进制数
	 * @param lpszStr 需要转换的字符串
	 * @param lpszData 转换后的字符串
	 * @return int 转换后的16进制数
	*/
	static int StrToHex(std::string lpszStr, char *lpszData)
	{
		int t, t1;
		int rlen = 0, len = (int)lpszStr.length();

		if (len / 2 != 0)
		{
			lpszStr += '0';
			len += 1;
		}

		for (int i = 0; i < len;)
		{
			char l, h = lpszStr[i];

			if (h == ' ')
			{
				++i;
				continue;
			}

			++i;

			if (i >= len)
			{
				break;
			}

			l = lpszStr[i];
			t = HexChar(h);
			t1 = HexChar(l);

			if ((t == 16) || (t1 == 16))
			{
				break;
			}
			else
			{
				t = t * 16 + t1;
			}

			++i;
			lpszData[rlen] = (char)t;
			++rlen;
		}

		return rlen;
	}

	/*!
	 * @brief 字符串转换为10进制数
	 * @param lpszStr 需要转换的字符串
	 * @return int 转换后的10进制数
	*/
	static int HexToDem(std::string lpszStr)
	{
		int dem = 0;

		for (unsigned int i = 0; i < lpszStr.length(); ++i)
		{
			dem = dem * 16;

			//0~9之间的字符
			if ((lpszStr[i] <= '9') && (lpszStr[i] >= '0'))
			{
				dem += lpszStr[i] - '0';
			}
			//A~F之间的字符
			else if ((lpszStr[i] <= 'F') && (lpszStr[i] >= 'A'))
			{
				dem += lpszStr[i] - 'A' + 10;
			}
			//a~f之间的字符
			else if ((lpszStr[i] <= 'f') && (lpszStr[i] >= 'a'))
			{
				dem += lpszStr[i] - 'a' + 10;
			}
			//出错时返回-1
			else
			{
				return -1;
			}
		}

		return dem;
	}

	static string UnicodeToUtf8(const wchar_t* _wstr)
	{
		// 预算-缓冲区中多字节的长度    
		int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, _wstr, -1, nullptr, 0, nullptr, nullptr);
		// 给指向缓冲区的指针变量分配内存    
		char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
		// 开始向缓冲区转换字节    
		WideCharToMultiByte(CP_UTF8, 0, _wstr, -1, pAssii, ansiiLen, nullptr, nullptr);
		string ret_str = pAssii;
		free(pAssii);
		return ret_str;
	}

	static string AcsiiToUtf8(const char* _str)
	{
		return UnicodeToUtf8(string_to_wstring(_str).c_str());
	}

	static wstring Utf8ToUnicode(const char* _str)
	{
		// 预算-缓冲区中宽字节的长度    
		int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, _str,-1, nullptr, 0);
		// 给指向缓冲区的指针变量分配内存    
		wchar_t* pUnicode = (wchar_t*)malloc(sizeof(wchar_t) * unicodeLen);
		// 开始向缓冲区转换字节    
		MultiByteToWideChar(CP_UTF8, 0, _str, -1 , pUnicode, unicodeLen);
		wstring ret_str = pUnicode;
		free(pUnicode);
		return ret_str;
	}

	static string Utf8ToAcsii(const char* _str)
	{
		return wstring_to_string(Utf8ToUnicode(_str));
	}
}

#endif //!_STRINGEX_H