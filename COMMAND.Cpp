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

//show a circle
void CCommand::ShowCpaturePoint(const Position &pos, BOOL bShow /* = FALSE */)
{
	if (bShow)
	{
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 
		CCircle*	pTempCircle = new CCircle(pos, 3); 
		pTempCircle->Draw(pDC, dmDrag);
		delete pTempCircle;

		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
// 	else {
// 		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 
// 		CCircle*	pTempCircle = new CCircle(m_ptCapture, 3); 
// 		pTempCircle->Draw(pDC, dmDrag);
// 		delete pTempCircle;
// 
// 		g_pView->ReleaseDC(pDC); // 释放设备环境指针
// 	}
}


BOOL CCommand::GetNearKeyPoint(Position &pt)
{
	CVCadDoc *pDoc = (CVCadDoc *)g_pView->GetDocument();
	POSITION	pos = pDoc->m_EntityList.GetHeadPosition();
	while(pos!=NULL)
	{
		MEntity*	pEntity = (MEntity *) pDoc->m_EntityList.GetNext(pos);
		/////////////////////////////////////////////////////////
		if(pEntity->m_nOperationNum <= g_pView->m_nCurrentOperation) {
			MEntity * pEntShow = pEntity->GetShowEnt();
			if(pEntShow && pEntShow->m_nOperationNum >= 0 )
			{
				if(pEntity->GetSnapPos(pt))
				{
					return TRUE;
				}
			}
		}

	}	
	
	return FALSE;
}
//
int CCommand::OnMouseMove(UINT nFlags, const Position& pos)
{
	Position pt = pos;

	if(GetNearKeyPoint(pt))
	{
		if(!m_bCaptured)
		{
			ShowCpaturePoint(pt, TRUE);
		}

		m_bCaptured = TRUE;
		m_ptCapture = pt;
	}
	else if(m_bCaptured)
	{
		ShowCpaturePoint(m_ptCapture, TRUE);

		m_bCaptured = FALSE;
	}

	return 0;
}
