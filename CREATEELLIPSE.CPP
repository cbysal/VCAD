#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateEllipse::CCreateEllipse()
	: m_LeftTop(0,0), m_RightBottum(0,0)
{
	m_nStep = 0; // 初始化操作步为 0	
}

CCreateEllipse::~CCreateEllipse()
{
}

int CCreateEllipse::GetType()
{
	return ctCreateEllipse; 
}

int	CCreateEllipse::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // 每次单击鼠标左键时操作步加 1 
	switch(m_nStep) // 根据操作步执行相应的操作
	{
		case 1: 
		{
			m_LeftTop = m_RightBottum = pos;
			::Prompt("请输入椭圆的右下点：");
			break;
		}
		case 2:
		{
			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 

			// 擦除在拖动状态时显示的橡皮线
			MLines*		pTempLine = new MLines(m_LeftTop, m_RightBottum);
			MEllipse*	pTempCircle = new MEllipse(m_LeftTop,m_RightBottum); 
			pTempLine->Draw(pDC, dmDrag);
			pTempCircle->Draw(pDC, dmDrag);
			delete pTempLine;
			delete pTempCircle;

			m_RightBottum = pos;
						
			MEllipse*	pNewCircle = new MEllipse(m_LeftTop,m_RightBottum);
			pNewCircle->Draw(pDC,dmNormal); 
			g_pDoc->m_EntityList.AddTail(pNewCircle); 
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pNewCircle->m_nOperationNum = g_pView->m_nCurrentOperation;
			g_pView->ReleaseDC(pDC); // 释放设备环境指针
			
			m_nStep = 0;  // 将操作步重置为 0
			::Prompt("请输入椭圆的左上点：") ;
			break;
		}
		
	}
	return 0;
}

int	CCreateEllipse::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_ELLIPSE));

	// 用一静态变量nPreRefresh记录进入OnMouseMove状态时的刷新次数
	static	int nPreRefresh = g_nRefresh; 
	// 布尔变量bRefresh说明在OnMouseMove过程中视窗是否被刷新
	BOOL	bRefresh = FALSE; 
	// nCurRefresh用于记录当前的刷新次数
	int		nCurRefresh = g_nRefresh; 
	// 如果nCurRefresh和nPreRefresh不相等，说明视窗曾被刷新过
	if(nCurRefresh != nPreRefresh){ 
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh; 
	}

	switch(m_nStep)
	{
		case 0:
			::Prompt("请输入椭圆的左上点：") ;
			break;
		case 1:
		{
			Position	prePos, curPos;
			prePos = m_RightBottum; // 获得鼠标所在的前一个位置
		
			curPos = pos;

			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
			
			// 创建临时对象擦除上一次的橡皮线
			MLines*		pTempLine1 = new MLines(m_LeftTop, prePos);
			MEllipse*	pTempCircle1 = new MEllipse(m_LeftTop, prePos);
			if(!bRefresh){ // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine1->Draw(pDC, dmDrag);
				pTempCircle1->Draw(pDC, dmDrag);
			}
			delete pTempCircle1;
			delete pTempLine1;
			// 创建临时对象，根据当前位置绘制橡皮线
			MLines*		pTempLine2 = new MLines(m_LeftTop, curPos);
			MEllipse*	pTempCircle2 = new MEllipse(m_LeftTop, curPos);
			pTempLine2->Draw(pDC, dmDrag);
			pTempCircle2->Draw(pDC, dmDrag);
			delete pTempCircle2;
			delete pTempLine2;
 
			g_pView->ReleaseDC(pDC); // 释放设备环境指针			

			m_RightBottum = curPos; // 将当前位置设置为直线终点，以备下一次鼠标移动时用
			break;
		}
	}
	return 0;
}
// 单击鼠标右键取消当前的操作
int	CCreateEllipse::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_RightBottum; // 获得鼠标所在的前一个位置
		MLines*		pTempLine = new MLines(m_LeftTop, prePos);
		MEllipse*	pTempCircle = new MEllipse(m_LeftTop, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		pTempCircle->Draw(pDC, dmDrag); 
		delete pTempCircle;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("请输入椭圆的左上点：") ;
	return 0;
}
// 调用Cancel 函数取消本次操作
int CCreateEllipse::Cancel()
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_RightBottum; // 获得鼠标所在的前一个位置
		MLines*		pTempLine = new MLines(m_LeftTop, prePos);
		MEllipse*	pTempCircle = new MEllipse(m_LeftTop, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		pTempCircle->Draw(pDC, dmDrag); 
		delete pTempCircle;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0 ;
}

