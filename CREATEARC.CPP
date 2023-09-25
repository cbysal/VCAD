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


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/*	
 *	CLASS	CCreateArc
 */
CCreateArc::CCreateArc()
	: m_center(0,0), m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // 初始化操作步为 0	
}

CCreateArc::~CCreateArc()
{
}

int CCreateArc::GetType()
{
	return ctCreateArc;
}

int	CCreateArc::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // 每次单击鼠标左键时操作步加 1 
	switch(m_nStep)
	{
		case 1:
		{
			m_center = pos;
			::Prompt("请输入圆弧的起始点：") ;
			break;
		}
		case 2:
		{
			m_begin = m_end = pos ;
			::Prompt("请输入圆弧的终点：") ;
			break;
		}
		case 3:
		{
 			CDC*	pDC = g_pView->GetDC(); // a pointer to internal dc 
			
			// 擦除在拖动状态时显示的橡皮线
			MLines*	pTempLine1 = new MLines(m_center, m_begin);
			MLines*	pTempLine2 = new MLines(m_center, m_end);
			CArc*	pTempArc = new CArc(m_center,m_begin, m_end);
			pTempLine1->Draw(pDC, dmDrag);
			pTempLine2->Draw(pDC, dmDrag);
			pTempArc->Draw(pDC, dmDrag);
			delete pTempLine1;
			delete pTempLine2;
			delete pTempArc;

			m_end = pos ;

			CArc*	pNewArc = new CArc(m_center,m_begin, m_end);			
			pNewArc->Draw(pDC,dmNormal);
			g_pDoc->m_EntityList.AddTail(pNewArc);
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;			
			pNewArc->m_nOperationNum = g_pView->m_nCurrentOperation;
			
			g_pView->ReleaseDC(pDC); // don't forget this
			
			m_nStep = 0; 
			::Prompt("请输入圆弧的中心点：") ;
			break;
		}
		
	}
	return 0;
}

int	CCreateArc::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_ARC));
	
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
			::Prompt("请输入圆的中心点：") ;
			break;
		case 1:
			break;
		case 2:
		{
			Position	prePos, curPos;
			prePos = m_end ;
			curPos = pos;
			
			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
			
			// 创建临时对象擦除上一次的橡皮线
			MLines*	pTempLine1 = new MLines(m_center, prePos);
			CArc*	pTempArc1 = new CArc(m_center,m_begin, prePos);
			if(!bRefresh){ // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine1->Draw(pDC, dmDrag);
				pTempArc1->Draw(pDC, dmDrag);
			}
			delete pTempLine1;
			delete pTempArc1;

			// 创建临时对象，根据当前位置绘制橡皮线
			MLines*	pTempLine2 = new MLines(m_center, curPos);
			CArc*	pTempArc2 = new CArc(m_center,m_begin, curPos);
			if(!bRefresh){ // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine2->Draw(pDC, dmDrag);
				pTempArc2->Draw(pDC, dmDrag);
			}
			delete pTempLine2;
			delete pTempArc2;

			g_pView->ReleaseDC(pDC); // 释放设备环境指针			

			m_end = curPos; // 将当前位置设置为直线终点，以备下一次鼠标移动时用			

			break;
		}
	}
	return 0;
}

int	CCreateArc::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// 如果当前的操作步为 2 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 2){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
	
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*		pTempLine1 = new MLines(m_center, m_begin);
		MLines*		pTempLine2 = new MLines(m_center, prePos);
		CArc*		pTempArc = new CArc(m_center, m_begin, prePos);
		pTempLine1->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		pTempLine2->Draw(pDC, dmDrag); 
		pTempArc->Draw(pDC, dmDrag); 
		delete		pTempLine1;
		delete		pTempLine2;
		delete		pTempArc;
		
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("请输入圆弧的中心点：") ;

	return 0;
}

// 调用Cancel 函数取消本次操作
int CCreateArc::Cancel()
{
	// 如果当前的操作步为 2 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 2){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*		pTempLine1 = new MLines(m_center, m_begin);
		MLines*		pTempLine2 = new MLines(m_center, prePos);
		CArc*		pTempArc = new CArc(m_center, m_begin, prePos);
		pTempLine1->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		pTempLine2->Draw(pDC, dmDrag); 
		pTempArc->Draw(pDC, dmDrag); 
		delete		pTempLine1;
		delete		pTempLine2;
		delete		pTempArc;

		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	
	return 0 ;
}
