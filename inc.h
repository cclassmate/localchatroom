#pragma once
class CMFCApplication1Dlg;
#define MAX_BUF_SIZE 1024
class CClientItem//客户端信息
{
public:
	CString m_strIp;//客户端ip
	SOCKET m_Socket;//客户端socket
	HANDLE hThread;//客户端线程句柄
	CMFCApplication1Dlg *m_pMainWnd;//客户端主窗口句柄
	CClientItem()//初始化
	{
		m_pMainWnd = NULL;
		m_Socket = INVALID_SOCKET;
		hThread = NULL;
	}
};
DWORD WINAPI ListenThreadFunc(LPVOID pParam);//监听线程函数
DWORD WINAPI ClientThreadProc(LPVOID ipParameter);//客户端线程函数
DWORD WINAPI ConnectThreadFunc(LPVOID pParam);//服务器现场函数
bool Socket_Select(SOCKET hsock, int timeout=100, BOOL bRead=FALSE);//select封装函数
