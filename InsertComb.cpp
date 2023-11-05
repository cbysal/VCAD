#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"
#include "Windows.h"
#include "TextInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInsertComb::CInsertComb()
	: m_LeftTop(0, 0), m_RightBottom(0, 0)
{
	m_nStep = 0; // 初始化操作步为 0
}

CInsertComb::~CInsertComb()
{
}

int CInsertComb::GetType()
{
	return ctCreateComb;
}

int	CInsertComb::OnLButtonDown(UINT nFlags, const Position& pos)
{
	CTextInputDlg dlg1, dlg2;
	dlg1.DoModal();
	dlg2.DoModal();

	CComb* oldComb = NULL;
	POSITION position = g_pDoc->m_EntityList.GetHeadPosition();
	while (position != NULL) {
		MEntity* entity = (MEntity*)g_pDoc->m_EntityList.GetAt(position);
		if (entity->GetType() == etComb) {
			CComb* comb = (CComb*)entity;
			if (!comb->GetName().Compare(dlg1.m_text))
			{
				oldComb = (CComb*)entity;
				break;
			}
		}
		g_pDoc->m_EntityList.GetNext(position);
	}
	if (oldComb == NULL) {
		char msg[256];
		sprintf(msg, "不存在组合: %s", dlg1.m_text);
		MessageBox(NULL, msg, "错误", MB_OK);
		return 0;
	}

	std::vector<MEntity*> newEntities;
	for (MEntity* entity : oldComb->GetEntities()) {
		newEntities.push_back(entity->Copy());
	}
	CComb* newComb = new CComb(dlg2.m_text, oldComb->GetLeftTop(),oldComb->GetRightBottom(),newEntities);
	newComb->Move(oldComb->GetLeftTop(), pos);
	CDC* pDC = g_pView->GetDC(); // 得到设备环境指针 
	newComb->Draw(pDC, dmNormal);
	g_pDoc->m_EntityList.AddTail(newComb); // 将指针添加到图元链表
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
	newComb->m_nOperationNum = g_pView->m_nCurrentOperation;
	g_pView->ReleaseDC(pDC); // 释放设备环境指针

	return 0;
}

int	CInsertComb::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_RECT));

	// 用一静态变量nPreRefresh记录进入OnMouseMove状态时的刷新次数
	static	int nPreRefresh = g_nRefresh;
	// 布尔变量bRefresh说明在OnMouseMove过程中视窗是否被刷新
	BOOL	bRefresh = FALSE;
	// nCurRefresh用于记录当前的刷新次数
	int		nCurRefresh = g_nRefresh;
	// 如果nCurRefresh和nPreRefresh不相等，说明视窗曾被刷新过
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
		::Prompt("请输入矩形的左上角点：");
		break;
	case 1:
	{
		Position	prePos, curPos;
		prePos = m_RightBottom; // 获得鼠标所在的前一个位置

		curPos = pos;

		CDC* pDC = g_pView->GetDC(); // 得到设备环境指针

		// 创建临时对象擦除上一条橡皮线
		MRectangle* pTempRect = new MRectangle(m_LeftTop, prePos);
		if (!bRefresh) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
			pTempRect->Draw(pDC, dmDrag);
		delete pTempRect;
		// 创建临时对象，根据当前位置绘制一条橡皮线
		MRectangle* pTempRect2 = new MRectangle(m_LeftTop, curPos);
		pTempRect2->Draw(pDC, dmDrag);
		delete pTempRect2;

		g_pView->ReleaseDC(pDC); // 释放设备环境指针			

		m_RightBottom = curPos; // 将当前位置设置为直线终点，以备下一次鼠标移动时用
		break;
	}
	}
	return 0;
}
// 单击鼠标右键取消当前的操作
int	CInsertComb::OnRButtonDown(UINT nFlags, const Position& pos)
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_RightBottom; // 获得鼠标所在的前一个位置
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("请输入矩形的左上角点：");
	return 0;
}
// 调用Cancel 函数取消本次操作
int CInsertComb::Cancel()
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_RightBottom; // 获得鼠标所在的前一个位置
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0;
}

