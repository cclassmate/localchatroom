#include "inc.h"
#include "stdafx.h"
#include "MFCApplication1Dlg.h"
#include "MFCApplication1.h"
//客户端连接线程函数
DWORD WINAPI ConnectThreadFunc(LPVOID pParam)
{

	CMFCApplication1Dlg *pChatRoom = (CMFCApplication1Dlg*)pParam;
	ASSERT(pParam != NULL);
	pChatRoom->m_ConnectSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//建立连接的socket
	if (pChatRoom->m_ConnectSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("新建socket失败"));
		return false;
	}
	CString strServIp;
	pChatRoom->GetDlgItemText(C_IPADDRESS, strServIp);//得到编辑框中的文本
	int iPort = pChatRoom->GetDlgItemInt(CPORT_EDIT);//得到绑定的ip端口
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("请输入合适的端口号:1-65535"));
		goto _Error_End;
	}
	char szIpAddr[16] = { 0 };
	USES_CONVERSION;
	strcpy_s(szIpAddr, 16, T2A(strServIp));//宽字节转换成窄字节
	//客户端套接字信息
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szIpAddr);
	//connect第一个参数为客户端socket，第二个参数为要连接的主机socket信息，第三个函数为信息长度。
	if (connect(pChatRoom->m_ConnectSock, (struct sockaddr*)&server, sizeof(struct sockaddr))==SOCKET_ERROR)
	{
		AfxMessageBox(_T("连接失败,请重试"));
		goto _Error_End;
	}
	pChatRoom->my_state = 0;//连接成功，将状态设为0，表示当前状态为客户端。
	pChatRoom->EnableWindow(CONNECT_BUTTON,FALSE);
	pChatRoom->EnableWindow(CSTOP_BUTTON, TRUE);
	pChatRoom->EnableWindow(S_RADIO, FALSE);
	pChatRoom->ShowMsg(_T("系统信息:连接服务器成功"));
	while (TRUE && pChatRoom->bShutDown == FALSE)
	{
		if (Socket_Select(pChatRoom->m_ConnectSock))
		{
			//连接的socket每100秒轮询检检查一次
			TCHAR szBUF[MAX_BUF_SIZE] = { 0 };
			//从socket中获取信息，赋给szBUF
			int iRet = recv(pChatRoom->m_ConnectSock, (char*)szBUF, MAX_BUF_SIZE, 0);
			if (iRet > 0)
			{
				//显示信息
				pChatRoom->ShowMsg(szBUF);
			}
			else
			{
				pChatRoom->ShowMsg(_T("已经断开连接"));
				break;
			}

		}
		Sleep(500);
	}

_Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}
