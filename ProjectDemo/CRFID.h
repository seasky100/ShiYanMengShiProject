/*!
 * @file CRFID.h
 * @breif 描述RFID卡的基本属性信息以及功能文件
 * @author FanKaiyu
 * @date 2019-04-10
 * @version 2.1
*/
#pragma once
#ifndef _RFID_H
#define _RFID_H

#include <chrono>

using namespace std;
using namespace chrono;

/*!
 * @class RFIDMark
 * @brief RFID地标卡类
 *
 * 描述RFID卡的基本属性信息以及功能
*/
typedef class RFIDMark
{
public:
	RFIDMark() :m_usNo(0), m_pLocker(nullptr), m_pPreLocker(nullptr), m_tpLockTime(steady_clock::now()) {};
	RFIDMark(unsigned short usNo) :m_usNo(usNo), m_pLocker(nullptr), m_pPreLocker(nullptr), m_tpLockTime(steady_clock::now()){};
	~RFIDMark() {};

protected:
	unsigned short m_usNo;								/*!< 编号 */
	void* m_pLocker;									/*!< 锁定此RFID卡的指针 */
	steady_clock::time_point m_tpLockTime;				/*!< 锁定此RFID卡的时间 */
	void* m_pPreLocker;									/*!< 预先锁定此RFID卡的指针 */
public:
	/*!
	 * @brief 获取RFID卡编号
	 * @return unsigned short 编号
	*/
	unsigned short GetNo() const { return m_usNo; }

	/*!
	 * @brief 设置RFID卡编号
	 * @param unsigned short 新RFID卡编号
	*/
	bool SetNo(const unsigned short usRFID) 
	{
		if (m_usNo == usRFID)
		{
			return false;
		}

		m_usNo = usRFID;

		return true;
	}

	/*!
	 * @brief 锁定
	 * @param void* 锁定此RFID卡的指针
	 * @return bool 锁定成功返回true,否则返回false
	*/
	bool Lock(void* pLocker)
	{
		if (pLocker == nullptr)
		{
			return false;
		}

		if (m_pLocker == nullptr)
		{
			m_pLocker = pLocker;
			m_tpLockTime = steady_clock::now();
		}

		if (m_pLocker != pLocker)
		{
			return false;
		}


		if (m_pPreLocker == m_pLocker)
		{
			m_pPreLocker = nullptr;
		}	

		return true;
	}

	/*!
	 * @brief 解除锁定
	 * @param void* 锁定此RFID卡的指针
	*/
	void Unlock(void* pLocker)
	{
		if (m_pLocker == pLocker)
		{
			m_pLocker = nullptr;

			return;
		}

		return;
	}

	/*!
	 * @brief 预先锁定
	 * @param void* 锁定此RFID卡的指针
	 * @return bool 锁定成功返回true,否则返回false
	*/
	bool PreformLock(void* pLocker)
	{
		if (pLocker == nullptr)
		{
			return false;
		}

		if (m_pPreLocker == nullptr)
		{
			m_pPreLocker = pLocker;
		}

		if (m_pPreLocker != pLocker)
		{
			return false;
		}

		return true;
	}

	/*!
	 * @brief 解除预先锁定
	 * @param void* 锁定此RFID卡的指针
	*/
	void PreformUnlock(void* pLocker)
	{
		if (m_pPreLocker == pLocker)
		{
			m_pPreLocker = nullptr;

			return;
		}

		return;
	}

	/*!
	 * @brief 释放
	 *
	 * 清楚RFID卡所有锁定
	*/
	void Release()
	{
		m_pLocker = nullptr;
		m_pPreLocker = nullptr;
	}

	/*!
	 * @brief 获取锁定此RFID卡的指针
	 * @return void* 锁定此RFID卡的指针
	*/
	void* GetLocker() const { return m_pLocker; }

	/*!
	 * @brief 获取锁定此RFID卡的时间
	 * @param chrono::system_clock::time_point& 锁定此RFID卡的时间
	 * @return void* 锁定此RFID卡的指针
	*/
	void* GetLockTime(steady_clock::time_point& tpLockTime) const
	{
		tpLockTime = m_tpLockTime;

		return m_pLocker;
	}

	/*!
	 * @brief 对比锁定的RFID卡的时间
	 * @param const chrono::system_clock::time_point& 对比的时间
	 * @return long long 时间大于对比时间返回正数,等于返回0,小于返回负数
	*/
	long long Compare(const steady_clock::time_point& tpCompareTime)
	{
		return duration_cast<milliseconds>(m_tpLockTime - tpCompareTime).count();
	}

	/*!
	 * @brief 对比锁定的RFID卡
	 * @param const RFIDMark& 对比的RFID卡
	 * @return long long 时间大于对比时间返回正数,等于返回0,小于返回负数
	*/
	long long Compare(const RFIDMark& rfid)
	{
		steady_clock::time_point tpCompareTime;	/*!< 对比时间 */
		if (rfid.GetLockTime(tpCompareTime) == nullptr)
		{
			return -1;
		}

		return Compare(tpCompareTime);
	}

	/*!
	 * @brief 获取预先锁定此RFID卡的指针
	 * @return void* 预先锁定此RFID卡的指针
	*/
	void* GetPreformLocker() const
	{
		return m_pPreLocker;
	}
} RFID;

#endif //!_RFID_H