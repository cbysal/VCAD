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
	m_nStep = 0; // ��ʼ��������Ϊ 0
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
	m_nStep++; // ÿ�ε���������ʱ�������� 1 
	switch (m_nStep) // ���ݲ�����ִ����Ӧ�Ĳ���
	{
	case 1:
	{
		m_LeftTop = m_RightBottom = pos;
		::Prompt("��������ε����½ǵ㣺");
		break;
	}
	case 2:
	{
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
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
		pComb->Serialize(dlg.m_text);//���л�
		g_pView->Erase();
		pComb->Draw(pDC, dmNormal);
		g_pDoc->m_EntityList.AddTail(pComb); // ��ָ����ӵ�ͼԪ����
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pComb->m_nOperationNum = g_pView->m_nCurrentOperation;

		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��

		m_nStep = 0;  // ������������Ϊ 0
		::Prompt("��������ε����Ͻǵ㣺");
		// �������϶�״̬ʱ��ʾ����Ƥ��
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

	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh;
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE;
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh;
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if (nCurRefresh != nPreRefresh) {
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh;
	}

	switch (m_nStep)
	{
	case 0:
		::Prompt("��������ε����Ͻǵ㣺");
		break;
	case 1:
	{
		Position	prePos, curPos;
		prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��

		curPos = pos;

		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��

		// ������ʱ���������һ����Ƥ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, prePos);
		if (!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
			pTempRect->Draw(pDC, dmDrag);
		delete pTempRect;
		// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
		MRectangle* pTempRect2 = new MRectangle(m_LeftTop, curPos);
		pTempRect2->Draw(pDC, dmDrag);
		delete pTempRect2;

		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

		m_RightBottom = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��
		break;
	}
	}
	return 0;
}
// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CCreateComb::OnRButtonDown(UINT nFlags, const Position& pos)
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("��������ε����Ͻǵ㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CCreateComb::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if (m_nStep == 1) {
		CDC* pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_RightBottom; // ���������ڵ�ǰһ��λ��
		MRectangle* pTempRect = new MRectangle(m_LeftTop, m_RightBottom);
		pTempRect->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempRect;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0;
}

