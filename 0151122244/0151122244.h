/////////////////////////////////////////////////////////////////////
//    文件名： 0151122244.h
//    工程:  0151122244
//    作者：刘嘉磊，修改者：刘嘉磊
//    描述: 应用程序的主头文件。
//    版本: 1.0 2017/8/14。
//    修改: 创建 2017/8/14。
/////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMainPageApp:
// 有关此类的实现，请参阅 0151122244.cpp
//

class CMainPageApp : public CWinApp
{
public:
	CMainPageApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMainPageApp theApp;