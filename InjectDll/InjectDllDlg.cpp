
// InjectDllDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "InjectDll.h"
#include "InjectDllDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInjectDllDlg 对话框



CInjectDllDlg::CInjectDllDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECTDLL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInjectDllDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INJECT, &CInjectDllDlg::OnBnClickedButtonInject)
	ON_BN_CLICKED(IDC_BUTTON_FREE, &CInjectDllDlg::OnBnClickedButtonFree)
END_MESSAGE_MAP()


// CInjectDllDlg 消息处理程序

BOOL CInjectDllDlg::OnInitDialog()
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

void CInjectDllDlg::OnPaint()
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
HCURSOR CInjectDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//根据进程名称获取操作系统中该进程的标识符（PID）
DWORD getProcIDByName(const wchar_t* lpProcName)
{
	/*
	Snapshots are at the core of the tool help functions. A snapshot is a read-only copy of the current state of one
	or more of the following lists that reside in system memory: processes, threads, modules, and heaps.

	Processes that use tool help functions access these lists from snapshots instead of directly from the operating
	system. The lists in system memory change when processes are started and ended, threads are created and destroyed,
	executable modules are loaded and unloaded from system memory, and heaps are created and destroyed. The use of
	information from a snapshot prevents inconsistencies. Otherwise, changes to a list could possibly cause a thread
	to incorrectly traverse the list or cause an access violation (a GP fault). For example, if an application
	traverses the thread list while other threads are created or terminated, information that the application is using
	to traverse the thread list might become outdated and could cause an error for the application traversing the list.

	快照是工具帮助功能的核心。快照是驻留在系统内存中的以下一个或多个列表的当前状态的只读副本：进程，线程，模块和堆。

	使用工具帮助功能的进程从快照而不是直接从操作系统访问这些列表。当启动和结束进程，创建和销毁线程，从系统内存中加载
	和卸载可执行模块以及创建和销毁堆时，系统内存中的列表会更改。使用快照中的信息可以避免不一致。否则，对列表所做的
	更改可能会导致线程错误地遍历该列表或导致访问冲突（GP错误）。例如，如果在创建或终止其他线程时应用程序遍历线程列表，
	则该应用程序用于遍历线程列表的信息可能已过时，并可能导致应用程序遍历列表时出错。

	https://docs.microsoft.com/zh-cn/windows/win32/toolhelp/snapshots-of-the-system
	*/

	//获取当前系统中所有进程的快照句柄
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//判断获取是否成功
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	//结构体实例化，PROCESSENTRY32是用来存放快照进程信息的一个结构体
	PROCESSENTRY32 pe = { sizeof(pe) };

	//遍历快照并将进程信息存入指定的PROCESSENTRY32结构体
	for (BOOL ret = Process32First(hSnapShot, &pe); ret; ret = Process32Next(hSnapShot, &pe))
	{
		//比较进程的可执行文件名称
		if (wcscmp(pe.szExeFile, lpProcName) == 0)
		{
			//关闭快照句柄
			CloseHandle(hSnapShot);
			//返回查找到的进程标识符
			return pe.th32ProcessID;
		}
	}

	CloseHandle(hSnapShot);
	//没有指定名称的进程则返回0
	return 0;
}

//根据模块名称获取指定进程中该模块的地址
LPVOID getModuleAddressByName(DWORD	dwProcID, const wchar_t* lpModuleName)
{
	//获取当前系统中指定进程的模块快照句柄
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcID);

	//判断获取是否成功
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return (LPVOID)-1;
	}

	//结构体实例化，MODULEENTRY32是用来存放指定进程模块列表的一个结构体
	MODULEENTRY32 me = { sizeof(me) };

	//遍历快照并将模块信息存入指定的MODULEENTRY32结构体
	for (BOOL ret = Module32First(hModuleSnap, &me); ret; ret = Module32Next(hModuleSnap, &me))
	{
		//比较模块名称
		if (wcscmp(me.szModule, lpModuleName) == 0)
		{
			//关闭快照句柄
			CloseHandle(hModuleSnap);
			//返回查找到的模块地址
			return me.modBaseAddr;
		}
	}

	CloseHandle(hModuleSnap);
	//没有指定名称的模块则返回0
	return 0;
}

//根据进程标识符向该进程中注入指定动态链接库（Dll）
HANDLE inject(DWORD dwProcID, char* lpDllPath)
{
	//获取待注入进程的句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);

	//获取kernel32.dll的模块句柄
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");

	//获取LoadLibraryA函数的起始地址
	LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");

	//在待注入进程中为线程函数（LoadLibraryA）的参数（动态链接库的路径）分配虚拟内存
	LPVOID lpBaseAddress = VirtualAllocEx(hProcess, NULL, strlen(lpDllPath) + 1, MEM_COMMIT, PAGE_READWRITE);

	//将动态链接库的路径写入上一语句分配的虚拟内存
	WriteProcessMemory(hProcess, lpBaseAddress, lpDllPath, strlen(lpDllPath) + 1, NULL);

	//在待注入进程中创建远程线程（注入动态链接库）
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, lpBaseAddress, 0, NULL);

	//等待远程线程结束，防止线程阻塞，等待时间：两秒钟
	WaitForSingleObject(hRemoteThread, 2000);

	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	return hRemoteThread;
}

//根据进程标识符从该进程中卸载指定动态链接库（Dll）
HANDLE free(DWORD dwProcID, LPVOID lpModuleAddress)
{
	//获取待卸载进程的句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);

	//获取kernel32.dll的模块句柄
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");

	//获取FreeLibrary函数的起始地址
	LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "FreeLibrary");

	//在待卸载进程中创建远程线程（卸载动态链接库模块）
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, lpModuleAddress, 0, NULL);

	//等待远程线程结束，防止线程阻塞，等待时间：两秒钟
	WaitForSingleObject(hRemoteThread, 2000);

	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);

	return hRemoteThread;
}

void CInjectDllDlg::OnBnClickedButtonInject()
{
	DWORD dwProcID = getProcIDByName(L"CallDll.exe");
	if (dwProcID == -1)
	{
		MessageBoxA(NULL, "获取系统进程快照失败", "注入提示", MB_OK);
		return;
	}
	else if (dwProcID == 0)
	{
		MessageBoxA(NULL, "未发现CallDll.exe进程", "注入提示", MB_OK);
		return;
	}

	//判断进程中是否已有该模块
	LPVOID lpModuleAddress = getModuleAddressByName(dwProcID, L"DllExample.dll");
	if (lpModuleAddress == (LPVOID)-1)
	{
		MessageBoxA(NULL, "获取进程的模块快照句柄失败", "注入提示", MB_OK);
		return;
	}
	else if (lpModuleAddress != 0)
	{
		MessageBoxA(NULL, "进程中已有DllExample.dll模块", "注入提示", MB_OK);
		return;
	}

	//获取待注入动态链接库的绝对路径
	char lpDllPath[256];
	GetCurrentDirectoryA(256, lpDllPath);
	strcat_s(lpDllPath, "\\DllExample.dll");

	HANDLE hRemoteThread = inject(dwProcID, lpDllPath);
	if (hRemoteThread == NULL)
	{
		MessageBoxA(NULL, "注入失败", "注入提示", MB_OK);
	}
	else
	{
		MessageBoxA(NULL, "注入成功", "注入提示", MB_OK);
	}
}

void CInjectDllDlg::OnBnClickedButtonFree()
{
	DWORD dwProcID = getProcIDByName(L"CallDll.exe");
	if (dwProcID == -1)
	{
		MessageBoxA(NULL, "获取系统进程快照失败", "卸载提示", MB_OK);
		return;
	}
	else if (dwProcID == 0)
	{
		MessageBoxA(NULL, "未发现CallDll.exe进程", "卸载提示", MB_OK);
		return;
	}

	//获取待卸载动态链接库的模块地址
	LPVOID lpModuleAddress = getModuleAddressByName(dwProcID, L"DllExample.dll");
	if (lpModuleAddress == (LPVOID)-1)
	{
		MessageBoxA(NULL, "获取进程的模块快照句柄失败", "卸载提示", MB_OK);
		return;
	}
	else if (lpModuleAddress == 0)
	{
		MessageBoxA(NULL, "未发现DllExample.dll模块", "卸载提示", MB_OK);
		return;
	}

	HANDLE hRemoteThread = free(dwProcID, lpModuleAddress);
	if (hRemoteThread == NULL)
	{
		MessageBoxA(NULL, "卸载失败", "卸载提示", MB_OK);
	}
	else
	{
		MessageBoxA(NULL, "卸载成功", "卸载提示", MB_OK);
	}
}
