/*!
 * @file CMES.h
 * @brief 描述MES接口基本属性以及功能的文件
 * @author FanKaiyu
 * @date 2019-03-18
 * @version 1.0
*/

#pragma once
#ifndef _MES_H
#define _MES_H

#include <Windows.h>
#include <string>
#include "../String/DebugPrint.h"

using namespace std;

#import "../MES_SDK/Kel.IFactory.Mqtt.Client.tlb" named_guids raw_interfaces_only
using namespace Kel_IFactory_Mqtt_Client;

/*!< @brief MES公共错误代码 */
enum MES_ERROR_CODE
{
	MES_ERROR_CODE_SUCCESS = 0,			/*!< 操作成功 */
	MES_ERROR_CODE_SERVER = 100,		/*!< 服务器异常:{异常消息} */
	MES_ERROR_CODE_ACTION,				/*!< 未找到匹配的接口:{routingKey}#{action} */
	MES_ERROR_CODE_ACTPARAM,			/*!< 未找到接口{routingKey}#{action}的参数配置信息 */
	MES_ERROR_CODE_LINKPARAM = 105,		/*!< 共享目录连接参数不正确:{requestJsonData} */
	MES_ERROR_CODE_OVERTIME = 110,		/*!< 接口请求超时 */
	MES_ERROR_CODE_UNKNOWN = 1000,		/*!< 发生未知异常:{异常消息} */
	MES_ERROR_CODE_CHECK = 1005,		/*!< 生产数据校验失败:{校验信息};校验规则:craft_rule */
	MES_ERROR_CODE_UPDATEDATA,			/*!< 更新数据失败:未找到产品条码{ProductBarCode}在{工艺编号}工艺的生产数据 */
	MES_ERROR_CODE_INSERTDATA,			/*!< 插入数据失败:{错误消息} */
	MES_ERROR_CODE_CTRLDATA,			/*!< 数据操作异常:{异常消息} */
	MES_ERROR_CODE_CHECKPARAM,			/*!< 生产数据校验失败:参数{0}不能为空且长度不超过{1}个字符;校验规则:craft_metadata */
	MES_ERROR_CODE_PARAM = 1002,		/*!< 输入参数错误:参数{0}不能为空且数据长度不能超过{1}个字符;检测规则:mq_interface_metadata */
	MES_ERROR_CODE_SAVE = 1010,			/*!< 保存失败 */
	MES_ERROR_CODE_UPDATE,				/*!< 更新失败 */
	MES_ERROR_CODE_DELETE,				/*!< 删除失败 */
	MES_ERROR_CODE_INSERT,				/*!< 插入失败 */
	MES_ERROR_CODE_PATH = 1102,			/*!< 路径错误:错误消息 */
	MES_ERROR_CODE_LINK,				/*!< 连接共享目录错误:错误消息 */
};

/*!< SDK客户端连接错误代码 */
enum SDK_ERROR_CODE
{
	SDK_ERROR_CODE_SYSTEM = -2,			/*!< 系统错误 */
	SDK_ERROR_CODE_BUSY,				/*!< 系统繁忙 */
	SDK_ERROR_CODE_OPEN = 111,			/*!< 客户端连接已打开 */
	SDK_ERROR_CODE_CLOSE,				/*!< 客户端连接已关闭 */
	SDK_ERROR_CODE_SERVER,				/*!< 连接MQ服务器失败:{错误消息} */
	SDK_ERROR_CODE_UNKNOWN = 1000,		/*!< 未知错误:{异常消息} */
};

/*!< PACK出站校验错误代码 */
enum PACK_ERROR_CODE
{
	PACK_ERROR_CODE_UNFINISH = 2201,	/*!< 部件装配未完成，部件{0}要求配置数据{1},已装配数据{2} */
	PACK_ERROR_CODE_CHECK,				/*!< 工艺规则校验失败:{ResultMsg} */
};

/*!< @biref 工站信息 */
typedef struct MESWorkPositionStruct
{
	unsigned char m_byNo;	/*!< 工位编号 */
	_bstr_t m_bstrNo;		/*!< 工站编号 */
	_bstr_t m_bstrName;		/*!< 工位名称 */
	_bstr_t m_bstrLevel;	/*!< 工段 */
	_bstr_t m_bstrAction;	/*!< 工序 */
}MESWP;

/*!
 * @class CMES
 * @brief 实现与MES系统的对接
*/
class CMES
{
public:
	CMES(_bstr_t bstrProductLineNo);
	~CMES();
protected:
	static bool g_bInstall;				/*!< dll加载标识 */
	_bstr_t m_bstrProductLineNo;		/*!< 产线编号 */
	IIFactoryMqttClientPtr m_client;	/*!< MES接口对象实例 */
public:
	/*!
	 * @brief 加载
	*/
	static void Install();

	/*!
	 * @brief 卸载
	*/
	static void Uninstall();
public:
	/*!
	 * @brief 是否已连接
	*/
	bool IsOpen() const;

	/*!
	 * @brief 接收到中控控制命令
	 *
	 * 客户端连接后可通过此事件接收中控发送的设备控制指令，
	 * 如“一键启动”、“设备停机”等指令。
	*/
	void OnReceiveControlCmd();

	/*!
	 * @brief 接收到服务返回结果
	 *
	 * 当客户端接收到服务端返回的处理结果时发生此事件。
	 * 当使用异步方式调用MES服务时，必须通过此事件接收返回结果。
	*/
	void OnReceiveResult();

	/*!
	 * @brief 接收到中控消息
	 *
	 * 客户端接收到中控发送的重要消息时发送，
	 * 可用于接收中控发出的通知消息、报警信息等内容。
	*/
	void OnReceiveMsg();

	/*!
	 * @brief 关闭客户端连接
	 *
	 * 上位机退出时关闭连接
	*/
	void Close();

	/*!
	 * @brief 连接到MES服务器
	 *
	 * 上位机启动时通过他连接应用服务器，成功后才能进行产线MES接口的调用。
	 * 连接时需输入此上位机的唯一标识，以工位编号作为标识。
	 * @param _bstr_t 工位编号
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(_bstr_t bstrClientID);

	/*!
	 * @brief 请求MES接口调用
	 * @param _bstr_t 接口资源标识(URI)，各种生产管控服务接口的唯一标识。
	 * @param _bstr_t 请求操作，如“add/edit/query/validate/get”等。
	 * @param _bstr_t 请求操作的数据对象，以JSON格式封装
	 * @param bool 等待（同步方式）为true，否则（异步方式）为false，此时须通过事件OnReceiveResult接收返回结果
	 * @param _bstr_t& 接口返回结果，waiting为true时返回接口结果，false时在resutl的msg中返回请求的序号。
	 * @return bool 请求成功返回true,否则返回false
	*/
	bool RequestWithResult(_bstr_t bstrIri, _bstr_t bstrAction, _bstr_t bstrJsonData, bool bWaiting, _bstr_t& bstrResult);

	/*!
	 * @brief Pack出站校验
	 * @param _bstr_t 工站编号
	 * @return bool 成功返回true,否则返回false
	*/
	bool PackOutBound(_bstr_t bstrWPNo);

protected:
	/*!
	 * @brief 获取异常信息
	 * @param HRESULT 错误码
	 * @param _BaseResult* 返回信息指针
	 * @return _bstr_t 异常信息
	*/
	_bstr_t GetErrorMsg(HRESULT erroCode, _BaseResult* pResult);
};

#endif //!_MES_H