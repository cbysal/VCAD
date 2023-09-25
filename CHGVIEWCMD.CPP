 #include "stdafx.h"
#include "math.h"

#include "Entity.h"

#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"

#include "Command.h"
#include "ChgViewCmd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////
/*	
 *	CLASS	CViewPanCmd
 */
CViewPanCmd::CViewPanCmd()
{
	m_nStep = 0; // 初始化操作步为 0
}

CViewPanCmd::~CViewPanCmd()
{
}

int	CViewPanCmd::GetType()
{
	return ctViewPan;
}

int	CViewPanCmd::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++ ;
	
	CRect rc ;
	g_pView->GetClientRect(&rc) ;

	switch(m_nStep)
	{
		case 1:
			m_basePos = m_desPos = pos;
			::Prompt("请输入平移画面的目标点：") ;
			break;
		case 2:
		{
			m_desPos = pos;
			double dx = m_basePos.x - m_desPos.x;
			double dy = m_basePos.y - m_desPos.y ;

			g_pView->m_dOrgX += dx ;
			g_pView->m_dOrgY += dy ;
			g_pView->Invalidate() ;	
			
//			g_pDoc->SetModifiedFlag(TRUE);

			//	设置步骤为0
			m_nStep = 0;
			break;
		}
		default:
			break;
	}
	return 0;
}

int	CViewPanCmd::OnMouseMove(UINT nFlags, const Position& pos)
{
	switch(m_nStep)
	{
		case 1:
		{
			CRect	rect;
			g_pView->GetClientRect(&rect); 
				
			Position	prePos, curPos;
			prePos = m_basePos ;//
			curPos = pos ;
			//计算位移差
			Position dp = prePos - curPos ;
			//计算平移后的世界坐标原点
			g_pView->m_dOrgX += dp.x ;
			g_pView->m_dOrgY += dp.y ;
			//由于每移动一次，坐标原点都要改变，
			//所以，起始点的位置也发生变化
			m_basePos.x = pos.x + dp.x;
			m_basePos.y = pos.y + dp.y ;
			
			g_pView->Invalidate(TRUE) ;
			break;
		}
		default:
			break;
	}
	return 0;
}

// Click right button to cancel the operation
int	CViewPanCmd::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep = 0;
	return 0;
}

int CViewPanCmd::Cancel()
{
	return 0 ;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/*	
 *	CLASS	CZoomRgnCmd
 */
CZoomRgnCmd::CZoomRgnCmd()
{
	m_nStep = 0; // 初始化操作步为 0
}

CZoomRgnCmd::~CZoomRgnCmd()
{
}

int CZoomRgnCmd::GetType()
{
	return ctViewZoomRgn;
}

int	CZoomRgnCmd::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++;
	switch(m_nStep)
	{
		case 1:
		{
			m_basePos = m_desPos = pos;
			Prompt("请输入缩放矩形的右下角点：") ;
			break;
		}
		case 2:
		{
			m_desPos = pos;
			// 获得当前的设备环境
			CDC*	pDC = g_pView->GetDC(); 
			CRect rc ;
			g_pView->GetClientRect(&rc) ;
			// 转换为逻辑坐标系
			pDC->DPtoLP(&rc) ;
			// 计算新坐标系的原点
			g_pView->m_dOrgX = (m_basePos.x + m_desPos.x)/2. ;
			g_pView->m_dOrgY = -(m_basePos.y + m_desPos.y)/2. ;
			// 计算新坐标系与设备坐标的比例因子
			double scalex = fabs((m_basePos.x - m_desPos.x)/
				(rc.right-rc.left)) ;
			double scaley = fabs((m_basePos.y - m_desPos.y)/
				(rc.bottom-rc.top )) ;
			if(scalex>scaley)
				g_pView->scale = scalex ;
			else
				g_pView->scale = scaley ;
			// 设置比例因子的取值范围
			if(fabs( float(g_pView->scale) )<1e-6||
				fabs( float(g_pView->scale))>1e+6 )
				g_pView->scale = 1. ;
			
			g_pDoc->UpdateAllViews(NULL) ;
			// 设置修改标记
//			g_pDoc->SetModifiedFlag(TRUE);
			// 释放当前的设备环境
			g_pView->ReleaseDC(pDC); 
			
			m_nStep = 0;
			break;
		}
		default:
			m_nStep = 0;
			break;		
	}
	return 0;
}

int	CZoomRgnCmd::OnMouseMove(UINT nFlags, const Position& pos)
{
	switch(m_nStep)
	{
		case 1:
		{
			Position	prePos, curPos;
			prePos = m_desPos;
			curPos = pos;
			// 获得当前的设备环境
			CDC*	pDC = g_pView->GetDC(); 
			CPoint prep, curp, bp ;
			// 转换成屏幕坐标点
			g_pView->WorldtoScreen(prePos, prep) ;
			g_pView->WorldtoScreen(curPos, curp) ;
			g_pView->WorldtoScreen(m_basePos, bp) ;
			
			pDC->SelectStockObject(NULL_BRUSH) ;
			int ndrawmode = pDC->SetROP2(R2_NOT) ;
			// 绘制被放大区域
			pDC->Rectangle(bp.x, bp.y, prep.x, prep.y) ;
			pDC->Rectangle(bp.x, bp.y, curp.x, curp.y) ;
			pDC->SetROP2(ndrawmode) ;
			// 释放当前的设备环境
			g_pView->ReleaseDC(pDC); 
			m_desPos = pos;
			break;
		}
	}
	return 0;
}

int	CZoomRgnCmd::OnRButtonDown(UINT nFlags, const Position& pos) 
{

	::ClipCursor(NULL);

	m_nStep = 0;
	return 0;
}

int CZoomRgnCmd::Cancel()
{
	return 0 ;
}