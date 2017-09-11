#include "stdafx.h"
#include "MFCApplication1Dlg.h"
#include "MFCApplication1.h"
#define MAX_BUF_SIZE 1024

//封装select操作，三个参数，socket，轮询时间，和是否读取
bool Socket_Select(SOCKET hsock, int timeout, BOOL bRead)
{
	//select参数说明，第一个文件描述符范围（不设置），第二个参数是指向可读性
	//第三个参数是指向可写性，第四个参数是指向错误检查，最后一个参数为等待时间结构timeval
	//select最多1024个描述符。
	//poll无描述符限制
	//epoll，无描述符限制，采用回掉函数的方式，再有描述符时通知，而不是轮询，并且等待时间可以精确到毫秒
	//select和poll比较适用于描述符少，且都很活跃的情况。
	fd_set fdset;
	timeval tv;//设置轮询等待时间
	FD_ZERO(&fdset);//将描述符初始化
	FD_SET(hsock, &fdset);//将传入的socket设置
	timeout = timeout > 1000 ? 1000 : timeout;
	tv.tv_sec = 0;//秒
	tv.tv_usec = timeout;//毫秒
	int iRet = 0;
	if (bRead)
	{
		iRet = select(0, &fdset, NULL, NULL, &tv);
	}
	else
	{
		iRet = select(0, NULL, &fdset, NULL, &tv);
	}
	if (iRet <= 0)//超时
		return FALSE;
	else if (FD_ISSET(hsock, &fdset))//表示已经准备好描述符，而且hsock已经设置成功
		return TRUE;
	return FALSE;
}

DWORD WINAPI ListenThreadFunc(LPVOID pParam)//服务器监听线程
{
	CMFCApplication1Dlg *pChatRoom = (CMFCApplication1Dlg *)pParam;//获得mfc窗口对象
	ASSERT(pChatRoom != NULL);//如果为空打印错误，abort终止

	//创建监听socket，第一个参数协议簇（af_inet是ipv4协议簇），第二个参数指定socket类型（sock_strem流字节传输）
	//第三个参数是指定传输协议（tcp传输协议）
	pChatRoom->m_ListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (pChatRoom->m_ListenSock == INVALID_SOCKET)//创建失败处理
	{
		AfxMessageBox(_T("新建socket失败"));
		return FALSE;
	}
	int iport = pChatRoom->GetDlgItemInt(SPORT_EDIT);//获得端口号
	if (iport <= 0 || iport > 65535)
	{
		AfxMessageBox(_T("请输入合适的端口: 1-65535"));
		goto __Error_End;
	}//端口号不在0-65535之间的处理

	//service为监听socket的信息，协议簇，ip地址，端口号。
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;//任意地址，即如果当前server主机有多ip，则可以任选
	service.sin_port = htons(iport);//主机字节流转网络字节流

	//绑定，第一个参数为要绑定的socket，第二个参数为socket信息（强转为sockaddr），第三个参数为第二个参数长度
	if (bind(pChatRoom->m_ListenSock, (sockaddr*)&service, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("绑定端口失败"));
		goto __Error_End;
	}

	//监听，第一个参数为要监听的socket，第二个参数为链接队列最大长度
	if (listen(pChatRoom->m_ListenSock, 5) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("监听失败"));
		goto __Error_End;
	}

	//到此，创建，绑定socket，监听已经成功，服务器端已经建立
	pChatRoom->my_state = 1;//将状态置为1（服务器状态）
	//如果线程退出则结束循环
	while (TRUE&&pChatRoom->bShutDown == FALSE)
	{

		//io复用，防止一直阻塞，100秒一次轮询监听socket，检查有没有描述符（客户端连接）变化。
		if (Socket_Select(pChatRoom->m_ListenSock, 100, TRUE))
		{
			//检测有人到接入监听的socket

			//clientaddr接入的客户端socket信息
			sockaddr_in clientaddr;
			int ilen = sizeof(sockaddr_in);
			//accept函数，创建一个类似监听socket的socket，用来连接客户端socket，阻塞函数
			//第一个参数是监听sockeet，第二个是客户端socket信息，第三个是客户端socket信息的长度
			//函数返回一个socket，即用这个socket和客户端连接
			SOCKET accSock = accept(pChatRoom->m_ListenSock, (struct sockaddr*)&clientaddr, &ilen);
			if (accSock == INVALID_SOCKET)
			{
				continue;
			}
			//将该客户端加入消息队列
			CClientItem tItem;//消息节点
			tItem.m_Socket = accSock;//将创建的客户端线程赋给消息节点
			tItem.m_strIp = inet_ntoa(clientaddr.sin_addr);
			tItem.m_pMainWnd = pChatRoom;
			INT_PTR idx = pChatRoom->m_ClientArray.Add(tItem);

			//为每一个客户端新开一个线程处理，否则会（等待接收recv）一直阻塞。
			//第五个参数启动选项为暂时挂起，第四个参数为线程函数参数
			tItem.hThread = CreateThread(NULL, 0, ClientThreadProc, &(pChatRoom->m_ClientArray.GetAt(idx)), CREATE_SUSPENDED, NULL);
			//将函数返回得到的线程句柄赋给消息节点
			pChatRoom->m_ClientArray.GetAt(idx).hThread = tItem.hThread;
			//唤醒线程
			ResumeThread(tItem.hThread);
			Sleep(100);
			//其他操作
		}
	}
	// TODO: 在此添加控件通知处理程序代码
__Error_End:
	closesocket(pChatRoom->m_ListenSock);//失败处理，关闭socket
	return TRUE;
}
DWORD WINAPI ClientThreadProc(LPVOID ipParameter)
{
	//为每一个客户端新开的线程
	CString strMsg;
	CClientItem m_clientitem = *(CClientItem *)ipParameter;
	while (TRUE&&m_clientitem.m_pMainWnd->bShutDown == FALSE)
	{
		//监听该客户端的socket
		if (Socket_Select(m_clientitem.m_Socket, 100, TRUE))
		{
			//取得了socket中发送的信息
			TCHAR szBUF[MAX_BUF_SIZE] = { 0 };
			//recv第一个参数socket，第二个参数为一个字符串指针，第三个为大小，第四个处理带外数据（默认0）
			int IRet = recv(m_clientitem.m_Socket, (char*)szBUF, MAX_BUF_SIZE, 0);
			//将接收到的消息拼接后显示
			if (IRet > 0)
			{
				strMsg = szBUF;
				strMsg = _T("客户端:") + m_clientitem.m_strIp + _T(">") + strMsg;
				m_clientitem.m_pMainWnd->ShowMsg(strMsg);
				m_clientitem.m_pMainWnd->SendClientsMsg(strMsg, &m_clientitem);
			}
			else
			{
				//如果recv返回的是<=0表示客户端断开连接
				strMsg = _T("客户端:") + m_clientitem.m_strIp + _T("离开了聊天室！");
				m_clientitem.m_pMainWnd->ShowMsg(strMsg);
				//调用将客户端从数组中清除
				m_clientitem.m_pMainWnd->RemoveClientFromArray(m_clientitem);
				break;
			}
		}
		Sleep(500);
	}
	return true;
}
