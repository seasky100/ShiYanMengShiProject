// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H

// TODO: 添加要在此处预编译的标头
#define TM_TCPCLIENT_RECEIVE WM_USER+1	/*!< 客户端接收数据的线程消息 */
#define TM_TCPSERVER_ACCEPT WM_USER+2	/*!< 服务端接收客户端连接请求的线程消息*/
#define TM_COMM_RXCHAR	WM_USER+3		/*!< 串口接收数据的线程消息 */

#ifdef _WIN64
#define ULL unsigned long long
#define GetTickCountEX() GetTickCount64()
#else
#define ULL unsigned long
#define GetTickCountEX() GetTickCount()
#endif
#endif //PCH_H
