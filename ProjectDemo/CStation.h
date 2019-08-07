/*!
 * @file CStation.h
 * @brief 工位属性以及功能文件
 * @author FanKaiyu
 * @date 2019-04-19
 * @version 2.1
*/

#pragma once
#ifndef _STATION_H
#define _STATION_H

#include <string>
#include <Windows.h>
#include "../String/StringEx.h"

using namespace std;

class CStation
{
public:
	CStation(unsigned char byNo, unsigned short usLocation, unsigned char byProcess, const char* strName,const char* strMESWP,bool bMES = true)
		:m_byNo(byNo), m_usLocation(usLocation), m_byProcess(byProcess) 
	{ 
		m_strName = strName;
		m_strMESWP = strMESWP;
		m_bMES = bMES;

		if (m_strMESWP == "")
		{
			m_bMES = false;
		}
	}

	CStation(unsigned char byNo, unsigned short usLocation, unsigned char byProcess, const wchar_t* wstrName, const wchar_t* wstrMESWP, bool bMES = true)
		:m_byNo(byNo), m_usLocation(usLocation), m_byProcess(byProcess)
	{
		m_strName = StringEx::wstring_to_string(wstrName).c_str();
		m_strMESWP = StringEx::wstring_to_string(wstrMESWP).c_str();
		m_bMES = bMES;

		if (m_strMESWP == "")
		{
			m_bMES = false;
		}
	}

	CStation(unsigned char byNo, unsigned short usLocation, unsigned char byProcess, string strName = "", string strMESWP = "", bool bMES = false)
		:m_byNo(byNo), m_usLocation(usLocation), m_byProcess(byProcess)
	{ 
		m_strName = strName;
		m_strMESWP = strMESWP;
		m_bMES = bMES;

		if (m_strMESWP == "")
		{
			m_bMES = false;
		}
	}

	CStation(unsigned char byNo, unsigned short usLocation, unsigned char byProcess, wstring wstrName, wstring wstrMESWP, bool bMES = true)
		:m_byNo(byNo), m_usLocation(usLocation), m_byProcess(byProcess)
	{
		m_strName = StringEx::wstring_to_string(wstrName).c_str();
		m_strMESWP = StringEx::wstring_to_string(wstrMESWP).c_str();
		m_bMES = bMES;

		if (m_strMESWP == "")
		{
			m_bMES = false;
		}
	}

	~CStation(){}

protected:
	unsigned char m_byNo;			/*!< 工位编号 */
	unsigned short m_usLocation;	/*!< 工位坐标 */
	string m_strName;				/*!< 工位名称 */
	unsigned char m_byProcess;		/*!< 工序步骤 */
	string m_strMESWP;				/*!< MES工站编号 */
	bool m_bMES;					/*!< MES启用标识 */

public:
	/*!
	 * @brief 获取编号
	 * @return unsigned char 工位编号
	*/
	const unsigned char GetNo() const { return m_byNo; }

	/*!
	 * @brief 获取坐标
	 * @return unsigned short 工位坐标
	*/
	const unsigned short GetLocation() const { return m_usLocation; }

	/*!
	 * @brief 绑定坐标
	 * @param unsigned short 新坐标
	 * @return bool 绑定成功返回true,否则返回false
	*/
	bool BindLocaion(const unsigned short usLocation)
	{
		if (m_usLocation == usLocation)
		{
			return false;
		}

		m_usLocation = usLocation;

		return true;
	}

	/*!
	 * @brief 获取工序
	 * @return unsigned char 工序
	*/
	const unsigned char GetProcess() const { return m_byProcess; }

	/*!
	 * @brief 绑定工序
	 * @param unsigned char 新工序
	 * @return bool 绑定成功返回true,否则返回false
	*/
	bool BindProcess(const unsigned char byProcess)
	{
		if (m_byProcess == byProcess)
		{
			return false;
		}

		m_byProcess = byProcess;

		return true;
	}

	/*!
	 * @brief 获取名称
	 * @return string 工位名称
	*/
	string GetName() const { return m_strName; }
	
	/*!
	 * @brief 获取名称
	 * @return wstring 工位名称
	*/
	wstring GetNameW()
	{
		return StringEx::string_to_wstring(m_strName);
	}

	/*!
	 * @brief 设置名称
	 * @param const char* 工位名称
	 * @return bool 设置成功返回true,否则返回false
	*/
	bool SetName(const char* strName)
	{
		if (strcmp(m_strName.c_str(), strName) == 0)
		{
			return false;
		}

		m_strName = strName;

		return true;
	}

	bool SetName(const wchar_t* wstrName)
	{
		return SetName(StringEx::wstring_to_string(wstrName).c_str());
	}
	bool SetName(string strName){ return SetName(strName.c_str()); }
	bool SetName(wstring wstrName) { return SetName(wstrName.c_str()); }

	void SetEnableMES(const bool bMES) { m_bMES = bMES; }
	const bool EnbaleMES() const { return m_bMES; }

	bool SetMESWP(const char* str)
	{
		if (strcmp(m_strName.c_str(), str) == 0)
		{
			return false;
		}

		m_strMESWP = str;

		return true;
	}

	bool SetMESWP(const wchar_t* wstr)
	{
		return SetName(StringEx::wstring_to_string(wstr).c_str());
	}
	bool SetMESWP(string str) { return SetName(str.c_str()); }
	bool SetMESWP(wstring wstr) { return SetName(wstr.c_str()); }

	string GetMESWP() const { return m_strMESWP; }

	wstring GetMESWPW()
	{
		return StringEx::string_to_wstring(m_strMESWP);
	}
};

#endif //_STATION_H