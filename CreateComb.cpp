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

CCreateComb::CCreateComb()
	: m_LeftTop(0, 0), m_RightBottom(0, 0)
{
	m_nStep = 0; // 初始化操作步为 0
}

CCreateComb::~CCreateComb()
{
}

int CCreateComb::GetType()
{
	return ctCreateComb;
}

int	CCreateComb::OnLButtonDown(UINT nFlags, const Position& pos)
{
	m_nStep++; // 每次单击鼠标左键时操作步加 1 
	switch (m_nStep) // 根据操作步执行相应的操作
	{
	case 1:
	{
		m_LeftTop = m_RightBottom = pos;
		::Prompt("请输入矩形的右下角点：");
		break;
	}
	case 2:
	{
		CDC* pDC = g_pView->GetDC(); // 得到设备环境指针 
		m_RightBottom = pos;

		g_pDoc->OnLButtonDown(MK_LBUTTON, m_LeftTop);
		g_pDoc->OnLButtonDown(MK_LBUTTON, m_RightBottom);

		std::vector<MEntity*> entities;
		for (int i = 0; i < g_pDoc->m_selectArray.GetSize(); i++) {
			entities.push_back(((MEntity*)g_pDoc->m_selectArray.GetAt(i))->Copy());
		}

		CTextInputDlg dlg;
		dlg.DoModal();

		CComb* pComb = new CComb(dlg.m_text, m_LeftTop, m_RightBottom, entities);
		pComb->Serialize(dlg.m_text);//序列化
		g_pView->Erase();
		pComb->Draw(pDC, dmNormal);
		g_pDoc->m_EntityList.AddTail(pComb); // 将指针添加到图元链表
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pComb->m_nOperationNum = g_pView->m_nCurrentOperation;

		g_pView->ReleaseDC(pDC); // 释放设备环境指针

		m_nStep = 0;  // 将操作步重置为 0
		::Prompt("请输入矩形的左上角点：");
		// 擦除在拖动状态时显示的橡皮线
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag);
		delete pTempRect;
		break;
	}

	}
	return 0;
}

int	CCreateComb::OnMouseMove(UINT nFlags, const Position& pos)
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
int	CCreateComb::OnRButtonDown(UINT nFlags, const Position& pos)
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
int CCreateComb::Cancel()
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

