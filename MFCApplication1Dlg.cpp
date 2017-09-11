
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "inc.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ListenSock = INVALID_SOCKET;
	m_hListenThread = NULL;
	m_hConnectThread = NULL;
	m_ConnectSock = INVALID_SOCKET;
	my_state = -1;
	bShutDown = FALSE;
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, CSTOP_BUTTON, m_StopClientBtn);
	DDX_Control(pDX, MESSEGE_EDIT, m_msgedit);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(S_RADIO, &CMFCApplication1Dlg::OnBnClickedRadio)
	ON_BN_CLICKED(C_RADIO, &CMFCApplication1Dlg::OnBnClickedRadio1)
	ON_BN_CLICKED(CONNECT_BUTTON, &CMFCApplication1Dlg::ClickConnectButton)
	ON_BN_CLICKED(CREAT_BUTTON, &CMFCApplication1Dlg::ClickCreatButton)
	ON_BN_CLICKED(SET_BUTTON, &CMFCApplication1Dlg::OnBnClickedButton)
	ON_BN_CLICKED(SEND_BUTTON, &CMFCApplication1Dlg::OnpushClickedButton)
	ON_EN_CHANGE(SEND_EDIT, &CMFCApplication1Dlg::OnEnChangeEdit)
	ON_BN_CLICKED(CSTOP_BUTTON, &CMFCApplication1Dlg::OnstopclientClickedButton)
	ON_BN_CLICKED(SSTOP_BUTTON, &CMFCApplication1Dlg::OnstopserverClickedButton)
	ON_BN_CLICKED(CLOSE_BUTTON, &CMFCApplication1Dlg::OncloseClickedButton)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);//不需要菜单
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标，true
	SetIcon(m_hIcon, FALSE);		// 设置小图标，faalse

	// TODO: 在此添加额外的初始化代码
	dlggallinit();//调用初始化按钮函数
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CMFCApplication1Dlg::dlggallinit()
{
	//m_StopClientBtn.EnableWindow(FALSE);
	//GetDlgItem(CSTOP_BUTTON)->EnableWindow(FALSE);
	//调用enablewindow可以将控件选择启用或者禁用
	EnableWindow(CSTOP_BUTTON, FALSE);
	EnableWindow(SPORT_EDIT, FALSE);
	EnableWindow(CREAT_BUTTON, FALSE);
	EnableWindow(SSTOP_BUTTON, FALSE);
	EnableWindow(SEND_BUTTON, FALSE);

	CheckRadioButton(C_RADIO, S_RADIO, C_RADIO);//循环查找radio，并将其位置初始化为cradio
	//SetDlgItemText用于设置文本框中的内容，这里将其初始化。
	SetDlgItemText(C_IPADDRESS, _T("127.0.0.1"));
	SetDlgItemText(CPORT_EDIT, _T("9880"));
	SetDlgItemText(SPORT_EDIT, _T("9880"));
	ShowMsg(_T("程序启动成功"));
	ExtendDiaog(TRUE);//刚开始将窗口设置为缩小的窗口
}
BOOL  CMFCApplication1Dlg::EnableWindow(UINT uID, bool bEnable)
{
	return GetDlgItem(uID)->EnableWindow(bEnable);
}


void CMFCApplication1Dlg::OnBnClickedRadio()
{
	//将相关按钮禁用以及开启
	EnableWindow(SPORT_EDIT, TRUE);
	EnableWindow(CREAT_BUTTON, TRUE);
	EnableWindow(C_IPADDRESS, FALSE);
	EnableWindow(CPORT_EDIT, FALSE);
	EnableWindow(CONNECT_BUTTON, FALSE);
	((CButton*)GetDlgItem(C_RADIO))->SetCheck(FALSE);//控制客户端选项与服务器选项单选
	// TODO: 在此添加控件通知处理程序代码
}
void CMFCApplication1Dlg::ExtendDiaog(BOOL bshow)
{
	static CRect m_DlgRectLarge(0, 0, 0, 0);//展开后的大窗口
	static CRect m_DlgRectSmall(0, 0, 0, 0);//收缩后的小窗口
	if (m_DlgRectLarge.IsRectNull())
	{
		GetWindowRect(&m_DlgRectLarge);
		m_DlgRectSmall = m_DlgRectLarge;
		m_DlgRectSmall.right -= 230;//多次运行测试得出230数值
	}
	if (bshow == TRUE)
		SetWindowPos(NULL, 0, 0, m_DlgRectLarge.Width(), m_DlgRectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);//设置大窗口，即显示出网络设置版面
	else
		SetWindowPos(NULL, 0, 0, m_DlgRectSmall.Width(), m_DlgRectSmall.Height(), SWP_NOZORDER | SWP_NOMOVE);//设置小窗口，即将网络设置版面隐藏

}




void CMFCApplication1Dlg::OnBnClickedRadio1()
{
	EnableWindow(SPORT_EDIT, FALSE);
	EnableWindow(CREAT_BUTTON, FALSE);
	EnableWindow(C_IPADDRESS, TRUE);
	EnableWindow(CPORT_EDIT, TRUE);
	EnableWindow(CONNECT_BUTTON, TRUE);
	((CButton*)GetDlgItem(S_RADIO))->SetCheck(FALSE);//设置客户端与服务器单选
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::ClickConnectButton()
{
	//创建客户端连接线程，并将句柄返回赋值
	m_hConnectThread = CreateThread(NULL, 0, ConnectThreadFunc, this, 0, NULL);
	
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::ClickCreatButton()
{
	//创建服务器监听线程。
	//creatthread，第一个（安全设置）与第二个（栈堆大小）参数设置默认，第三个位线程入口函数
	//第四个为函数参数（即将本mfc类传入即可），第五个（启动选项）与第六个（输出线程idd）设置为默认
	//并用m_hlistenthread 得到函数的线程句柄，即服务器监听句柄
	m_hListenThread = CreateThread(NULL, 0, ListenThreadFunc, this, 0, NULL);
	//创建成功后将一些控件禁用和启动
	EnableWindow(CREAT_BUTTON, FALSE);
	EnableWindow(SSTOP_BUTTON, TRUE);
	EnableWindow(C_RADIO, FALSE);
	/*m_ListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ListenSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("新建SOCKET失败"));
		return;
	}
	UINT uPort = GetDlgItemInt(CPORT_EDIT);
	if (uPort <= 0 || uPort > 65535)
	{
		AfxMessageBox(_T("请输入合适的端口:1-65535"));
		goto __Error_End;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(uPort);
	if (bind(m_ListenSock, (sockaddr*)&service, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("绑定端口失败"));
		goto __Error_End;
	}
	if (listen(m_ListenSock, 5) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("监听失败"));
		goto __Error_End;
	}
	sockaddr_in clientAddr;
	int iLen = sizeof(sockaddr_in);
	SOCKET accSock = accept(m_ListenSock, (struct sockaddr*)&clientAddr, &iLen);
	if (accSock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("接收客户端失败"));
		goto __Error_End;
	}
	// TODO: 在此添加控件通知处理程序代码
__Error_End:
	closesocket(m_ListenSock);
	*/
}
void CMFCApplication1Dlg::ShowMsg(CString strMsg)
{
	//封装显示信息
	m_msgedit.SetSel(-1, -1);
	m_msgedit.ReplaceSel(strMsg + _T("\r\n"));
}
void CMFCApplication1Dlg::RemoveClientFromArray(CClientItem in_Item)
{
	for (int idx = 0; idx < m_ClientArray.GetCount(); idx++)
	{
		CClientItem item = m_ClientArray.GetAt(idx);
		if (item.m_Socket == in_Item.m_Socket &&
			item.m_strIp == in_Item.m_strIp &&
			item.hThread == in_Item.hThread)
		{
			m_ClientArray.RemoveAt(idx);
		}
	}
}


void CMFCApplication1Dlg::SendClientsMsg(CString strMsg, CClientItem *pNotSend)
{
	TCHAR szBuf[MAX_BUF_SIZE] = { 0 };
	_tcscpy_s(szBuf, MAX_BUF_SIZE, strMsg);//将strmsg中的字符转成unicode并传到szbuf中
	//遍历客户端队列，除了传入的pnotsend指向的客户端以外，向每一个客户端发送信息
	for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++)
	{
		if (!pNotSend || pNotSend->m_Socket != m_ClientArray.GetAt(idx).m_Socket
			|| pNotSend->m_strIp != m_ClientArray.GetAt(idx).m_strIp
			|| pNotSend->hThread != m_ClientArray.GetAt(idx).hThread)
		{
			//socket send函数，第一个要发送的socket，第二个是要发送的串，第三个是要发送数据的字节数，第四个？？？
			send(m_ClientArray.GetAt(idx).m_Socket, (char *)szBuf, _tcslen(szBuf) * sizeof(TCHAR), 0);
		}
	}
	// TODO: 在此添加控件通知处理程序代码
}




void CMFCApplication1Dlg::OnBnClickedButton()//点击网络设置隐藏和显示网络设置窗口
{
	static int i = 1;
	if (i == 1)
	{
		ExtendDiaog(FALSE);
		i = 0;
	}
	else
	{
		i=1;
		ExtendDiaog(TRUE);
	}
	
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnpushClickedButton()
{
	CString strMsg;
	GetDlgItemText(SEND_EDIT, strMsg);//得到文本框中的信息
	if (my_state == 1)//如果状态为服务器，则向所有的客户端发送信息，第二个参数默认NULL即可
	{
		strMsg = (_T("服务器:>")) + strMsg;
		ShowMsg(strMsg);
		SendClientsMsg(strMsg);
	}
	else if (my_state == 0)//如果是客户端，则向服务器发送后，再向除了当前的所有客户端发送，
	{
		CString strtmp = (_T("本地客户端:>")) + strMsg;
		ShowMsg(strtmp);
		int isend = send(m_ConnectSock, (char*)strMsg.GetBuffer(), strMsg.GetLength() * sizeof(TCHAR), 0);
		strMsg.ReleaseBuffer();
	}
	else
	{
		AfxMessageBox(_T("请先进行网络设置"));
	}
	SetDlgItemText(SEND_EDIT,_T(""));
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnEnChangeEdit()
{
	CString  strMsg;
	GetDlgItemText(SEND_EDIT,strMsg);//这个是判断当前文本框是否为空，如果为空的话则将发送按钮禁用
	if (strMsg.IsEmpty())
	{
		EnableWindow(SEND_BUTTON,FALSE);
	}
	else
	{
		EnableWindow(SEND_BUTTON, TRUE);
	}
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
void CMFCApplication1Dlg::StopClient()
{
	bShutDown = TRUE;//将状态置为true，表示客户端停止
	DWORD dwret = WaitForSingleObject(m_hConnectThread, 1000);//两个参数，第一个参数为线程句柄，第二个为等待时间
	if (dwret != WAIT_OBJECT_0)
	{
		//如果在等待时间内未能结束，则将其强行关闭
		TerminateThread(m_hConnectThread, -1);
		closesocket(m_ConnectSock);//释放资源
	}
	//参数初始化。
	m_hConnectThread = NULL;
	m_ConnectSock = INVALID_SOCKET;
	bShutDown = FALSE;
	
}
void CMFCApplication1Dlg::StopServer()
{
	UINT nCount = m_ClientArray.GetCount();//获取正在连接的客户端数目
	HANDLE *m_phandles = new HANDLE[nCount + 1];//新建句柄数组并开空间
	m_phandles[0] = m_hListenThread;//第一个为监听
	for (int idx = 0; idx < nCount; idx++)
	{
		m_phandles[idx + 1] = m_ClientArray.GetAt(idx).hThread;
	}//之后将所有的客户端句柄赋值
	bShutDown = TRUE;//状态置为true
	//函数等待多个线程，第一个参数线程个数，第二个参数线程句柄数组指针，第三个参数为是否等待所有
	//第四个参数为等待时间。
	DWORD dwret = WaitForMultipleObjects(nCount + 1, m_phandles, TRUE, 1000);
	if(dwret != WAIT_OBJECT_0)
	{
		//如果有线程未结束，则手动强制结束所有线程并释放资源
		for (INT_PTR i = 0; i < m_ClientArray.GetCount() + 1; i++)
		{
			TerminateThread(m_ClientArray.GetAt(i).hThread, -1);
			closesocket(m_ClientArray.GetAt(i).m_Socket);
		}
		TerminateThread(m_hListenThread, -1);
		closesocket(m_ListenSock);
	}
	delete[] m_phandles;//释放
	//初始化
	m_hListenThread = NULL;
	m_ListenSock = INVALID_SOCKET;
	bShutDown = FALSE;
}

void CMFCApplication1Dlg::OnstopclientClickedButton()
{
	StopClient();
	//停止并初始化
	ShowMsg(_T("停止客户端"));
	my_state = -1;
	EnableWindow(CONNECT_BUTTON, TRUE);
	EnableWindow(CSTOP_BUTTON, FALSE);
	EnableWindow(S_RADIO, TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCApplication1Dlg::OnstopserverClickedButton()
{
	StopServer();
	ShowMsg(_T("停止服务器"));
	//停止并初始化
	my_state = -1;
	EnableWindow(C_RADIO, TRUE);
	EnableWindow(CREAT_BUTTON, TRUE);
	EnableWindow(SSTOP_BUTTON, FALSE);

	// TODO: 在此添加控件通知处理程序代码
}




void CMFCApplication1Dlg::OncloseClickedButton()
{
	if (my_state == 1) {
		StopServer();
	}
	else if (my_state == 0) {
		StopClient();
	}
	OnCancel();
	// TODO: 在此添加控件通知处理程序代码
}
