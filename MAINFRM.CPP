// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VCad.h"

#include "MainFrm.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
static UINT PropertyButtons[] = 
{
//	ID_SETTING_GRID,
//	ID_SETTING_ORTH,
	ID_PROPERTY,	//0
	ID_SEPARATOR,	//1
	ID_SEPARATOR,	//2:LINESTYLE
	ID_SEPARATOR,	//3
	ID_SEPARATOR,	//4:LINECOLOR
	ID_SEPARATOR,	//5
	ID_SEPARATOR,	//6:LINEWIDTH
};

//--------------------------------------------------------//
//--------------------------------------------------------//
// 全局函数：Prompt
// 功能：在状态栏上给出操作过程的提示信息
void Prompt(char* pmt)
{
	CStatusBar* pStatus=(CStatusBar*)
		AfxGetApp()->m_pMainWnd->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	
	ASSERT(pStatus) ;
	if(pStatus)
		pStatus->SetPaneText(0, pmt, TRUE);
}
//--------------------------------------------------------//
//--------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // 创建失败
	}
	//创建修改工具栏
	if(!m_wndModifyBar.CreateEx(this, TBSTYLE_FLAT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndModifyBar.LoadToolBar(IDR_MODIFY))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // 创建失败
	}
	//设置修改工具栏的标题
	m_wndModifyBar.SetWindowText(_T("修改工具栏"));
	//创建绘制工具栏
	if(!m_wndCreateBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_LEFT| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndCreateBar.LoadToolBar(IDR_CREATE))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // 创建失败
	}
	//设置绘制工具栏的标题
	m_wndCreateBar.SetWindowText(_T("绘制工具栏"));
	//创建视图工具栏
	if(!m_wndViewBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | WS_VISIBLE | CBRS_LEFT| CBRS_GRIPPER |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndViewBar.LoadToolBar(IDR_VIEW))
	{
		TRACE0("Failed to create modify toolbar\n");
		return -1;      // 创建失败
	}
	m_wndViewBar.SetWindowText(_T("视图工具栏"));

	if(!CreatePropertyBar())
		return -1;      // 创建失败
	m_wndPropBar.SetWindowText(_T("属性工具栏")) ;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // 创建失败
	}

	//  停靠工具栏
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModifyBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCreateBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndViewBar.EnableDocking(CBRS_ALIGN_ANY) ;
	m_wndPropBar.EnableDocking(CBRS_ALIGN_ANY) ;
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndModifyBar) ;
	//通过自定义的函数定义各工具栏的停靠方式
	DockControlBarLeftOf(&m_wndCreateBar, &m_wndToolBar);
	DockControlBarLeftOf(&m_wndPropBar, &m_wndModifyBar);
	DockControlBarLeftOf(&m_wndViewBar, &m_wndPropBar) ;

	// 显示闪屏窗口
	CSplashWnd::ShowSplashScreen(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::DockControlBarLeftOf(CControlBar* Bar,
									  CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// 调用 MFC 函数来调整所有被停靠的工具栏的尺寸
	// 以便 GetWindowRect 函数可以正确调用
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? 
						AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? 
						AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? 
						AFX_IDW_DOCKBAR_RIGHT : n;

	// 当我们使用缺省的参数时，
	//每个工具栏将停靠在分割线上
	//通过计算矩形区域，事实上我们是在模仿
	//将工具栏拖放到指定的位置上
	DockControlBar(Bar,n,&rect);
}
BOOL CMainFrame::CreatePropertyBar()
{
	const int nDropHeight = 100 ;
	if(!m_wndPropBar.Create(this, WS_CHILD|WS_VISIBLE|
		CBRS_LEFT|CBRS_SIZE_DYNAMIC|CBRS_TOOLTIPS|
		CBRS_FLYBY,IDR_PROPERTYBAR)||
		!m_wndPropBar.LoadBitmap(IDR_PROPERTYBAR)||
		!m_wndPropBar.SetButtons(PropertyButtons,
		sizeof(PropertyButtons)/sizeof(UINT)))
	{
		TRACE0("Failed to create propertybar\n") ;
		return FALSE ;
	}
	// 设置 SEPARATOR 的宽度
	m_wndPropBar.SetButtonInfo(1, ID_SEPARATOR, 
		TBBS_SEPARATOR, 12) ;
	// 设置线型组合框的宽度
	m_wndPropBar.SetButtonInfo(2, IDW_LINESTYLE, 
		TBBS_SEPARATOR,150) ;
	// 设置 SEPARATOR 的宽度
	m_wndPropBar.SetButtonInfo(3, ID_SEPARATOR, 
		TBBS_SEPARATOR,12) ;
	// 设置颜色组合框的宽度
	m_wndPropBar.SetButtonInfo(4, IDW_LINECOLOR, 
		TBBS_SEPARATOR,100) ;
	// 设置 SEPARATOR 的宽度
	m_wndPropBar.SetButtonInfo(5, ID_SEPARATOR, 
		TBBS_SEPARATOR,12) ;
	// 设置线宽度组合框的宽度
	m_wndPropBar.SetButtonInfo(6, IDW_LINEWIDTH,
		TBBS_SEPARATOR,150);
	
	// 定义各组合框的边界矩形
	CRect rc ;
	// 绘制线型组合框
	m_wndPropBar.GetItemRect(2,&rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// 创建线型组合框
	if(!m_wndPropBar.m_wndLineStyleCmb.Create(
		CBS_DROPDOWNLIST | WS_VISIBLE|WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINESTYLE))
	{
		TRACE0("Failed to create linestyle combo--box!") ;
		return FALSE ;
	}

	// 向线型组合框添加选项
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_SOLID,"实线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASH,"虚线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DOT, "点线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASHDOT, "点划线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_DASHDOTDOT, "双点划线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_NULL, "空线") ;
	m_wndPropBar.m_wndLineStyleCmb.AddItem(
		PS_INSIDEFRAME, "内框线") ;
	// 设置缺省项
	m_wndPropBar.m_wndLineStyleCmb.SetCurSel(0) ;

	// 绘制线的颜色组合框
	m_wndPropBar.GetItemRect(4, &rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// 创建线的颜色组合框
	if(!m_wndPropBar.m_wndLineColorCmb.Create(
		CBS_DROPDOWNLIST |WS_VISIBLE| WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINECOLOR))
	{
		TRACE0("Failed to create COLOR combo--box!\n") ;
		return FALSE ;
	}
	// 向线的颜色组合框添加选项
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,0,0),"红色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,255,0),"绿色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,0,255),"兰色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,255,0),"黄色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,255,255),"青色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(255,0,255),"紫色") ;
	m_wndPropBar.m_wndLineColorCmb.AddItem(
		RGB(0,0,0),"黑色") ;
	// 设置当前缺省的选项
	m_wndPropBar.m_wndLineColorCmb.SetCurSel(6) ;
	// 绘制线宽组合框
	m_wndPropBar.GetItemRect(6,&rc) ;
	rc.bottom = rc.top + nDropHeight ;
	// 创建线宽组合框
	if(!m_wndPropBar.m_wndLineWidthCmb.Create(
		CBS_DROPDOWNLIST| WS_VISIBLE|WS_TABSTOP|
		WS_VSCROLL|CBS_OWNERDRAWVARIABLE,
		rc, &m_wndPropBar, IDW_LINEWIDTH))
	{
		TRACE0("Failed to create WIDTH combo--box!\n") ;
		return FALSE ;
	}
	//在线宽组合框添加选项
	m_wndPropBar.m_wndLineWidthCmb.AddItem(1, "宽度1") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(2, "宽度2") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(3, "宽度3") ;
	m_wndPropBar.m_wndLineWidthCmb.AddItem(4, "宽度4") ;
	//设置当前缺省的选项
	m_wndPropBar.m_wndLineWidthCmb.SetCurSel(0) ;

	return TRUE ;

}
