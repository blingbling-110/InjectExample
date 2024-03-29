﻿
// CallDllDlg.h: 头文件
//

#pragma once


// CCallDllDlg 对话框
class CCallDllDlg : public CDialogEx
{
// 构造
public:
	CCallDllDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALLDLL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton();
private:
	typedef void (*lpCallDll)();//定义函数指针
	lpCallDll lpExpFun;//声明导出函数
	HMODULE hModule;//声明模块句柄
};
