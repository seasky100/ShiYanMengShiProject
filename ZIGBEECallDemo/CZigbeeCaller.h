/*!
 * @file CZigbeeCaller.h
 * @brief Zigbee呼叫器属性描述文件
 * @author FanKaiyu
 * @date 2019-04-19
 * @version 1.0
*/
#pragma once
#ifndef _ZGBCALLER_H
#define _ZGBCALLER_H

/*! @brief 呼叫器信号 */
enum CALLERSIGNAL
{
	SIGNAL_OFF,
	SIGNAL_ON,
};

/*! @brief 呼叫器类型 */
enum SIGNAL_TYPE
{
	CALL_SIGNAL = 1,
	CANCELCALL_SIGNAL,
	PASS_SIGNAL,
	CANCELPASS_SIGNAL,
	NG_SIGNAL,
	CANCELNG_SIGNAL,
	SCREAM,
};

typedef class CZigbeeCaller
{
public:
	CZigbeeCaller(const unsigned char byNo, const unsigned char byStation, bool bEnable = true) { Init(byNo, byStation, bEnable); }
	~CZigbeeCaller() {}

protected:
	unsigned char m_byNo;			/*!< 呼叫器编号 */
	unsigned char m_byStation;		/*!< 绑定工位编号 */
	unsigned char m_byCall;			/*!< 呼叫标识 */
	unsigned char m_byCancelCall;	/*!< 取消呼叫标识 */
	unsigned char m_byPass;			/*!< 放行标识 */
	unsigned char m_byCancelPass;	/*!< 取消放行标识 */
	unsigned char m_byNG;			/*!< NG标识 */
	unsigned char m_byCancelNG;		/*!< 取消NG标识 */
	unsigned char m_byScream;		/*!< 急停标识 */
	bool m_bPackOutBound;			/*!< 出站校验 */
	bool m_bEnable;					/*!< 启用标识 */
	char m_achMAC[4];				/*!< 呼叫器MAC地址 */

public:
	/*!
	 * @brief 初始化属性
	 * @param unsigned char 呼叫器编号
	*/
	void Init(const unsigned char byNo, const unsigned char byStation, bool Enable)
	{
		m_byNo = byNo;
		m_byStation = byStation;
		m_bEnable = Enable;

		m_byCall = SIGNAL_OFF;
		m_byCancelCall = SIGNAL_OFF;
		m_byPass = SIGNAL_OFF;
		m_byCancelPass = SIGNAL_OFF;
		m_byNG = SIGNAL_OFF;
		m_byCancelNG = SIGNAL_OFF;
		m_byScream = SIGNAL_OFF;
		m_bPackOutBound = false;

		memset(m_achMAC, 0, 4);
	}

	/*!
	 * @brief 获取编号
	 * @return unsigned char 呼叫器编号
	*/
	const unsigned char GetNo() const { return m_byNo; }

	/*!
	 * @brief 获取绑定工位
	 * @return unsigned char 绑定工位编号
	*/
	const unsigned char GetStation() const { return m_byStation; }

	/*!
	 * @brief 绑定工位
	 * @param unsigned char 新工位编号
	 * @return bool 绑定成功返回true,否则返回false
	*/
	const bool BindStation(const unsigned char byStation)
	{
		if (m_byStation == byStation)
		{
			return false;
		}

		m_byStation = byStation;

		return true;
	}

	/*!
	 * @brief 获取呼叫标识
	 * @reutrn unsigned char 呼叫标识
	*/
	const unsigned char GetCallSignal() const
	{ 
		return m_byCall;
	}

	/*!
	 * @brief 更新呼叫标识
	 * @param unsigned char 呼叫标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateCallSignal(const unsigned char byCall)
	{
		if (m_byCall == byCall)
		{
			return false;
		}

		m_byCall = byCall;

		return true;
	}

	/*!
	 * @brief 获取取消呼叫标识
	 * @return unsigned char 取消呼叫标识
	*/
	const unsigned char GetCancelCallSignal() const { return m_byCancelCall; }

	/*!
	 * @brief 更新取消呼叫标识
	 * @param unsigned char 取消呼叫标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateCancelCallSignal(const unsigned char byCancelCall)
	{
		if (m_byCancelCall == byCancelCall)
		{
			return false;
		}

		m_byCancelCall = byCancelCall;

		return true;
	}

	/*!
	 * @brief 获取放行标识
	 * @return unsigned char 放行标识
	*/
	const unsigned char GetPassSignal() const { return m_byPass; }

	/*!
	 * @brief 更新放行标识
	 * @param unsigned char 放行标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdatePassSignal(const unsigned char byPass)
	{
		if (m_byPass == byPass)
		{
			return false;
		}

		m_byPass = byPass;

		if (m_bPackOutBound && m_byPass == SIGNAL_OFF)
		{
			m_bPackOutBound = false;
		}

		return true;
	}

	/*!
	 * @brief 获取取消放行标识
	 * @return unsigned char 取消放行标识
	*/
	const unsigned char GetCancelPassSignal() const { return m_byCancelPass; }

	/*!
	 * @brief 更新取消放行标识
	 * @param unsigned char 取消放行标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateCancelPassSignal(const unsigned char byCancelPass)
	{
		if (m_byCancelPass == byCancelPass)
		{
			return false;
		}

		m_byCancelPass = byCancelPass;

		return true;
	}

	/*!
	 * @brief 是否通过出站校验
	 * @return bool 通过返回true,否则返回false
	*/
	const bool IsPackOutBounded() const { return m_bPackOutBound; }

	/*!
	 * @brief 更新出站校验
	 * @param bool 出站校验
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdatePackOutBound(bool bPackOutBound)
	{
		if (m_bPackOutBound == bPackOutBound)
		{
			return false;
		}

		m_bPackOutBound = bPackOutBound;

		return true;
	}

	/*!
	 * @brief 获取NG标识
	 * @return unsigned char NG标识
	*/
	const unsigned char GetNGSignal() const { return m_byNG; }

	/*!
	 * @brief 更新NG标识
	 * @param unsigned char NG标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateNGSignal(const unsigned char byNG)
	{
		if (m_byNG == byNG)
		{
			return false;
		}

		m_byNG = byNG;

		return true;
	}

	/*!
	 * @brief 获取取消NG标识
	 * @return unsigned char 取消NG标识
	*/
	const unsigned char GetCancelNGSignal() const { return m_byCancelNG; }

	/*!
	 * @brief 更新取消NG标识
	 * @param unsigned char 取消NG标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateCancelNGSignal(const unsigned char byCancelNG)
	{
		if (m_byCancelNG == byCancelNG)
		{
			return false;
		}

		m_byCancelNG = byCancelNG;

		return true;
	}

	/*!
	 * @brief 获取急停标识
	 * @return unsigned char 急停标识
	*/
	const unsigned char GetScreamSignal() const { return m_byScream; }

	/*!
	 * @brief 更新急停标识
	 * @param unsigned char 急停标识
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateScreamSignal(const unsigned char byScream)
	{
		if (m_byScream == byScream)
		{
			return false;
		}

		m_byScream = byScream;

		return true;
	}

	void SetEnableUse(const bool bEnable) { m_bEnable = bEnable; }
	const bool EnableUse() const { return m_bEnable; }

	void BindMACAddress(const char achMAC[4])
	{
		memcpy_s(m_achMAC, 4, achMAC, 4);

		return;
	}

	void GetMACAddress(char* achMAC)
	{
		memcpy_s(achMAC, 4, m_achMAC, 4);

		return;
	}

} ZGBCaller;

#endif //!_ZGBCALLER_H