#include "stdafx.h"
#include "math.h"
#include "Entity.h"
#include "MainFrm.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "Command.h"
#include "ModifyCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMirror::CMirror()
	: m_basePos(0,0), m_desPos(0,0)
{
	m_nStep = 0;// 初始化操作步为 0
}
CMirror::~CMirror()
{
}
int	CMirror::GetType()
{
	return ctMirror;
}
int	CMirror::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++ ;
	switch(m_nStep)
	{
		case 1:
			// 第一次单击鼠标左键时得到基点位置，并初步设定目标位置
			m_basePos = m_desPos = pos;
			::Prompt("请输入移动的目标点：单击鼠标右键取消") ;
			break;
		case 2:
		{
			m_desPos = pos;
			CDC*	pDC = g_pView->GetDC(); // 获得视类的设备环境指针
			// 清除鼠标移动时最后遗留下的橡皮线
			MLines*	pTempLine = new MLines(m_basePos, m_desPos);
			pTempLine->Draw(pDC, dmDrag);
			delete pTempLine;
			// 将选择集中的图元移动到目标位置并进行绘制
			int i, n;
			for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
			{
				MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
				pEntity->Draw(pDC,dmInvalid); // 清除原来位置上的图元
				//pEntity->Mirror(m_basePos, m_desPos); // 将图元移动到目标位置
				//pEntity->Draw(pDC,dmNormal); // 在目标位置上绘制图元
				MEntity * pNewEnt = pEntity->Copy();
				pNewEnt->Mirror(m_basePos, m_desPos); // 将图元移动到目标位置
				pNewEnt->Draw(pDC,dmNormal); // 在目标位置上绘制图元
				g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
				pNewEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
				pEntity->Append(pNewEnt);
			}
			g_pDoc->m_selectArray.RemoveAll(); //  清空选择集
//			g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
			g_pView->ReleaseDC(pDC); // 释放视类的设备环境指针
			m_nStep = 0; 
			break;
		}
		default:
			break;
	}
	return 0;
}
int	CMirror::OnMouseMove(UINT nFlags, const Position& pos)
{
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
			::Prompt("请输入镜像的第二点：") ;
			break;
		case 1:
		{
			Position	prePos, curPos;
			prePos = m_desPos; // 获得上一个目标位置
			curPos = pos; // 得到当前位置

			CDC*	pDC = g_pView->GetDC(); 
			// 如果在操作过程中窗口没有被刷新，则要清除上一次绘制的橡皮线
			if(!bRefresh)
			{
				MLines*	pTempLine1 = new MLines(m_basePos, prePos);
				pTempLine1->Draw(pDC, dmDrag);
				delete pTempLine1;
			}
			// 在当前位置绘制橡皮线
			MLines*	pTempLine2 = new MLines(m_basePos, curPos);
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
			// 根据当前位置给出选中图元的实时位置
			int i, n;
			for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
			{
				MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
				pEntity->Draw(pDC,dmSelect);
				// 如果在操作过程中窗口没有被刷新，则要清除上一个位置上绘制的图元
				if(!bRefresh) 
				{
					// 清除上一个位置上绘制的图元
					MEntity*	pCopyEntity1 = pEntity->Copy(); // 得到图元的拷贝
					pCopyEntity1->Mirror(m_basePos, prePos); // 将拷贝移动到上一个位置
					pCopyEntity1->Draw(pDC,dmDrag); // 对在上一个位置对拷贝进行重画
					delete pCopyEntity1; // 删除临时拷贝
				}
				// 在当前位置上绘制图元
				MEntity*	pCopyEntity2 = pEntity->Copy();// 得到图元的拷贝
				pCopyEntity2->Mirror(m_basePos, curPos);// 将拷贝移动到当前位置
				pCopyEntity2->Draw(pDC,dmDrag);	// 对当前位置绘制拷贝
				delete pCopyEntity2; // 删除临时拷贝 
			}
			g_pView->ReleaseDC(pDC);
			m_desPos = pos; // 将目标设置为当前位置
		}
		default:
			break;
	}
	return 0;
}
// 单击鼠标右键取消正在进行的操作
int	CMirror::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	Position	prePos = m_desPos; // 得到上一个鼠标位置
	if(m_nStep == 1){
		CDC*	pDC = g_pView->GetDC();
		// 清除上一个绘制的橡皮线
		MLines*	pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		
		int i, n;
		for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			// 清除上一次绘制的临时对象
			MEntity*	pCopyEntity = pEntity->Copy();
			pCopyEntity->Mirror(m_basePos, prePos);
			pCopyEntity->Draw(pDC,dmDrag);
			delete		pCopyEntity; 
			// 重新绘制选择集中的对象
			pEntity->Draw(pDC, dmSelect); // redraw the selected entity
		}
		g_pView->ReleaseDC(pDC); // don't forget this
	}
	m_nStep = 0;
	::Prompt("请输入镜像的第一点：") ;
	return 0;
}
// 调用Cancel 函数取消本次操作
int CMirror::Cancel()
{
	Position	prePos = m_desPos; // 得到上一个鼠标位置
	if(m_nStep == 1){
		CDC*	pDC = g_pView->GetDC();
		// 清除上一个绘制的橡皮线
		MLines*	pTempLine = new MLines(m_basePos, prePos);
		pTempLine->Draw(pDC, dmDrag);
		delete	pTempLine;
		
		int i, n;
		for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			// 清除上一次绘制的临时对象
			MEntity*	pCopyEntity = pEntity->Copy();
			pCopyEntity->Mirror(m_basePos, prePos);
			pCopyEntity->Draw(pDC,dmDrag);
			delete		pCopyEntity; 
			// 重新绘制选择集中的对象
			pEntity->Draw(pDC, dmSelect); // redraw the selected entity
		}
		g_pView->ReleaseDC(pDC); // don't forget this
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0;
}

