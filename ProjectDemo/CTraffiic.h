/*!
 * @file CTraffic.h
 * @brief 描述交通管制区域的基本属性以及功能
 * @author FanKaiyu
 * @date 2019-04-10
 * @version 2.0
*/
#pragma once
#ifndef _TRAFFIC_H
#define _TRAFFIC_H

#include <set>

using namespace std;

/*!
 * @class CTrafficArea
 * @brief 描述交通管制区域的基本属性以及功能
*/
typedef class CTrafficArea
{
public:
	CTrafficArea(unsigned short usRFID) :m_usCtrlRFID(usRFID) {}
	~CTrafficArea() {}
protected:
	unsigned short m_usCtrlRFID;		/*!< 管制RFID卡:AGV在此RFID卡停止并变更为交通管制状态,等待程序放行 */
	set<unsigned short> m_setStop;		/*!< 禁行RFID卡组:当组中RFID上有AGV在行进时,包括此管制区域在内所有与此管制区域相关的管制AGV均不放行 */
	set<unsigned short> m_setCompare;	/*!< 管制比对组:当管制RFID卡上存在行进AGV时,与组中其他管制区域对比,先到先行 */

public:
	/*!
	 * @brief 添加禁行RFID卡
	 * @param unsigned short 禁行RFID卡编号
	 * @return bool 添加成功返回true,失败返回false
	*/
	bool AddStopRFID(const unsigned short usRFID)
	{
		if (m_usCtrlRFID == usRFID)
		{
			return false;
		}

		if (m_setStop.find(usRFID) != m_setStop.end())
		{
			return false;
		}

		m_setStop.insert(usRFID);

		return true;
	}

	/*!
	 * @brief 添加比对管制区域
	 * @param unsigned short 管制区域编号
	 * @return bool 添加成功返回true,失败返回false
	*/
	bool AddCompareArea(const unsigned short usArea)
	{
		if (m_usCtrlRFID == usArea)
		{
			return false;
		}

		if (m_setCompare.find(usArea) != m_setCompare.end())
		{
			return false;
		}

		m_setCompare.insert(usArea);

		return true;
	}

	/*!
	 * @brief 获取管制RFID卡
	 * @return unsigned short 管制RFID卡
	*/
	const unsigned short GetControlRFID() const { return m_usCtrlRFID; }

	/*!
	 * @brief 获取禁行RFID卡组
	 * @return set<unsigned short>& 禁行RFID卡组
	*/
	const set<unsigned short>& GetStopArray() const { return m_setStop; }

	/*!
	 * @brief 获取比对管制组
	 * @return set<unsigned short>& 比对管制组
	*/
	const set<unsigned short>& GetCompareArray() const { return m_setCompare; }

}Traffic;

#endif //!_TRAFFIC_H