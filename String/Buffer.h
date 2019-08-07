/*!
 * @file Buffer.h
 * @brief 数据缓存区结构体文件
 * @author FanKaiyu
 * @date 2019-03-20
 * @version 2.0
 */

#pragma once
#ifndef _BUFFER_H
#define _BUFFER_H

#include <iostream>

static const unsigned int MAX_BUFFER_SIZE = 1000 * 1024; /*!< 1000KB */

/*! @brief 数据缓存区 */
typedef struct TheBufferOfDataStruct
{
    unsigned char* m_pbyBuffer; /*!< 缓存区指针 */
    unsigned int m_unSize;      /*!< 缓存区大小 */

    TheBufferOfDataStruct()
    {
        m_pbyBuffer = nullptr;
        m_unSize = 0;
    }
    
    TheBufferOfDataStruct(const TheBufferOfDataStruct& buffer)
    {
		m_pbyBuffer = nullptr;
		m_unSize = 0;

		Set(buffer.m_pbyBuffer, buffer.m_unSize);

		return;
    }

	TheBufferOfDataStruct(const unsigned char* pbyBuffer, const unsigned int unSize)
	{
		m_pbyBuffer = nullptr;
		m_unSize = 0;

		Set(pbyBuffer, unSize);

		return;
	}

	~TheBufferOfDataStruct()
	{
		Release();
	}

	void Release()
	{
		if (m_unSize == 0)
		{
			return;
		}
		
		// 释放
		delete[] m_pbyBuffer;
		m_pbyBuffer = nullptr;
		m_unSize = 0;
	}

	void operator= (const TheBufferOfDataStruct& buffer)
	{
		Set(buffer.m_pbyBuffer, buffer.m_unSize);

		return;
	}

	void operator+= (const TheBufferOfDataStruct& buffer)
	{
		Add(buffer.m_pbyBuffer, buffer.m_unSize);

		return;
	}

	TheBufferOfDataStruct operator+ (const TheBufferOfDataStruct& buffer)
	{
		TheBufferOfDataStruct newBuffer(m_pbyBuffer, m_unSize);
		newBuffer += buffer;

		return newBuffer;
	}

	bool operator== (const TheBufferOfDataStruct& buffer)
	{
		return Compare(buffer.m_pbyBuffer, buffer.m_unSize);
	}

	/*!
	 * @brief 赋值
	 * @param const unsigned char* 数据块指针
	 * @param const unsigned int 数据块大小
	*/
	void Set(const unsigned char* pbyBuffer, const unsigned int unSize)
	{
		if (Compare(pbyBuffer, unSize))
		{
			return;
		}

		Release();

		if (pbyBuffer == nullptr || unSize == 0)
		{
			return;
		}

		m_unSize = unSize;
		m_pbyBuffer = new unsigned char[m_unSize];
		// 初始化
		memset(m_pbyBuffer, 0, m_unSize);
		// 拷贝
		memcpy_s(m_pbyBuffer, m_unSize, pbyBuffer, m_unSize);

		return;
	}

	/*!
	 * @brief 增加数据
	 * @param const unsigned char* 数据块指针
	 * @param const unsigned int 数据块大小
	*/
	void Add(const unsigned char* pbyBuffer, const unsigned int unSize)
	{
		if (pbyBuffer == nullptr || unSize == 0)
		{
			return;
		}

		unsigned int unNewBufferSize = m_unSize + unSize;		  /*!< 合并后的缓存区大小 */

		int nDiff = unNewBufferSize - MAX_BUFFER_SIZE;			/*!< 溢出的字节数 */

		// 存在数据溢出
		if (nDiff > 0)
		{
			unNewBufferSize -= nDiff;
		}

		unsigned char* pbyNewBuffer = new unsigned char[unNewBufferSize]; /*!< 合并后的缓存区 */

		// 初始化
		memset(pbyNewBuffer, 0, unNewBufferSize);

		if (m_unSize > 0)
		{
			// 存在数据溢出
			if (nDiff > 0)
			{
				memcpy(pbyNewBuffer, m_pbyBuffer + nDiff, m_unSize - nDiff);
			}
			// 不存在数据溢出
			else
			{
				memcpy(pbyNewBuffer, m_pbyBuffer, m_unSize);
			}
		}

		// 存在数据溢出
		if (nDiff > 0)
		{
			memcpy(pbyNewBuffer + m_unSize - nDiff, pbyBuffer, unSize);
		}
		// 不存在数据溢出
		else
		{
			memcpy(pbyNewBuffer + m_unSize, pbyBuffer, unSize);
		}

		Release();

		m_pbyBuffer = pbyNewBuffer;
		m_unSize = unNewBufferSize;

		return;
	}

	/*!
	 * @brief 比较
	 * @param const unsigned char* 数据块指针
	 * @param const unsigned int 数据块大小
	 * @return bool 相同返回true,否则返回false
	*/
	bool Compare(const unsigned char* pbyBuffer, const unsigned int unSize)
	{
		if (m_unSize != unSize)
		{
			return false;
		}

		for (unsigned int i = 0; i < m_unSize; ++i)
		{
			if (m_pbyBuffer[i] != pbyBuffer[i])
			{
				return false;
			}
		}

		return true;
	}
}DataBuffer;

#endif //!_BUFFER_H