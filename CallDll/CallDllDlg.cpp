
// CallDllDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CallDll.h"
#include "CallDllDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCallDllDlg 对话框



CCallDllDlg::CCallDllDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CALLDLL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCallDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCallDllDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON, &CCallDllDlg::OnBnClickedButton)
END_MESSAGE_MAP()


// CCallDllDlg 消息处理程序

BOOL CCallDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCallDllDlg::OnPaint()
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
HCURSOR CCallDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCallDllDlg::OnBnClickedButton()
{
	//判断是否已加载动态链接库模块
	hModule = GetModuleHandleA("DllExample.dll");
	if (hModule == NULL)
	{
		//将动态链接库加载到调用进程的地址空间中
		hModule = LoadLibraryA("DllExample.dll");

		//若模块句柄为NULL，则说明加载失败
		if (hModule == NULL)
		{
			MessageBoxA(NULL, "加载动态链接库失败", "调用提示", MB_OK);
			return;
		}
	}

	//获取动态链接库模块句柄中导出函数的地址
	lpExpFun = (lpCallDll)GetProcAddress(hModule, "expFun");

	//调用导出函数
	lpExpFun();
}
