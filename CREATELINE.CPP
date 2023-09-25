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

CCreateLine::CCreateLine()
	: m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // 初始化操作步为 0
	//m_pPositions = NULL;
}

CCreateLine::~CCreateLine()
{
	//if(m_pPositions) {
	//	delete [] m_pPositions;
	//	m_pPositions = NULL;
	//}

	m_pPositions.clear();
}

int CCreateLine::GetType()
{
	return ctCreateLine; 
}

int	CCreateLine::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // 每次单击鼠标左键时操作步加 1 


	switch(m_nStep) // 根据操作步执行相应的操作
	{
		case 1: 
		{
			m_begin = m_end = pos;

			::Prompt("请输入直线的下一点：");
			break;
		}
		default:
		{
			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 

			// 擦除在拖动状态时显示的最后一条线
// 			MLines*	pTempLine = new MLines(m_begin,m_end); 
// 			pTempLine->Draw(pDC, dmDrag);
// 			delete pTempLine;

			// 如果在按鼠标左键的过程中同时按下了Shift键，
			//	那么根据鼠标单击位置绘制水平线或竖直线
			if( nFlags & MK_SHIFT ){ 
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx) <= fabs(dy)) // 如果鼠标单击位置在X方向靠近起点
					m_end.Set(m_begin.x, pos.y); // 那么终点的x坐标与起点的相同
				else
					m_end.Set(pos.x,m_begin.y);
			}
			else if (m_bCaptured)
			{
				m_end = m_ptCapture;
			}
			else {
				m_end = pos; // 如果未按下Shift键, 则终点为鼠标单击位置			
			}
			
			m_begin = m_end;
			break;
		}	
		
	}
		//保存点集
	//int i;
	//Position * pNew = new Position[m_nStep];
	//for( i=0; i<m_nStep-1; i++)
	//	pNew[i] = m_pPositions[i];
	//pNew[i] = m_end;
	//if(m_nStep > 1 && m_pPositions)
	//	delete [] m_pPositions;
	//m_pPositions = pNew;
	m_pPositions.push_back(m_end);

	return 0;
}

//鼠标左键双击创建多线
int CCreateLine::OnLButtonDblClk(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 
	
	// 擦除在拖动状态时显示的最后一条线
	MLines*	pTempLine = new MLines(m_begin,m_end); 
	pTempLine->Draw(pDC, dmDrag);
	delete pTempLine;
	
	// 如果在按鼠标左键的过程中同时按下了Shift键，
	//	那么根据鼠标单击位置绘制水平线或竖直线
	if( nFlags & MK_SHIFT ){ 
		double dx = pos.x - m_begin.x;
		double dy = pos.y - m_begin.y;
		if(fabs(dx) <= fabs(dy)) // 如果鼠标单击位置在X方向靠近起点
			m_end.Set(m_begin.x, pos.y); // 那么终点的x坐标与起点的相同
		else
			m_end.Set(pos.x,m_begin.y);
	}
	else
		m_end = pos; // 如果未按下Shift键, 则终点为鼠标单击位置			
	
	if(m_nStep >= 2) {
		MLines*	pNewLine = new MLines(m_pPositions);// 根据起点和终点创建直线
		pNewLine->Draw(pDC,dmNormal); // 绘制直线
		g_pDoc->m_EntityList.AddTail(pNewLine); // 将直线指针添加到图元链表
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;
		m_pPositions.clear();
	}
	
	g_pView->ReleaseDC(pDC); // 释放设备环境指针
	
	m_nStep = 0;  // 将操作步重置为 0
	::Prompt("请输入直线的起点：");
	
	return 0;
}

int	CCreateLine::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_LINE));

	CCommand::OnMouseMove(nFlags,  pos);

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
			::Prompt("请输入直线的起点：");
			break;
		default:
		{
			Position	prePos, curPos;
			prePos = m_end; // 获得鼠标所在的前一个位置
			
			// 如果在按鼠标左键的过程中同时按下了Shift键，
			//	那么根据鼠标单击位置绘制水平线或竖直线
			if( nFlags & MK_SHIFT ){
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx)>=fabs(dy))
					curPos.Set(pos.x,m_begin.y);
				else
					curPos.Set(m_begin.x, pos.y);
			}
			else if (m_bCaptured)
			{
				curPos = m_ptCapture;
			}
			else {
				curPos = pos; // 如果未按下Shift键, 则终点为鼠标单击位置			
			}

			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
			
			// 创建临时对象擦除上一条橡皮线
			MLines*	pTempLine1 = new MLines(m_begin, prePos);
			if(!bRefresh) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// 创建临时对象，根据当前位置绘制一条橡皮线
			MLines*	pTempLine2 = new MLines(m_begin, curPos);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
 
			g_pView->ReleaseDC(pDC); // 释放设备环境指针			

			m_end = curPos; // 将当前位置设置为直线终点，以备下一次鼠标移动时用
			break;
		}
	}

	return 0;
}

// 单击鼠标右键取消当前的操作
int	CCreateLine::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("请输入直线的起点：");
	return 0;
}
// 调用Cancel 函数取消本次操作
int CCreateLine::Cancel()
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0 ;
}

