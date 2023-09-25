#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "Entity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////
// API function
//

// 根据绘图模式设置绘图环境：设置设备环境的模式并创建画笔
void	SetDrawEnvir(CDC*	pDC, int drawMode, CPen* pPen)
{
	int			lineStyle, lineWidth;
	COLORREF	color;
	switch(drawMode) 
	{
		case dmSelect: // 选中状态
		{
			pDC->SetROP2(R2_COPYPEN);
			lineStyle = PS_SOLID;
			lineWidth = 1;
			color = RGB(255,0,0);
			break;
		}
		case dmPrompt: // 提示状态
		{
			pDC->SetROP2(R2_COPYPEN);
			lineStyle = PS_DASH;
			lineWidth = 1;
			color = RGB(0,255,255);
			break;
		}
		case dmDrag: // 拖动状态
		{
			pDC->SetROP2(R2_XORPEN);
			lineStyle = PS_SOLID;
			lineWidth = 1;
			color = RGB(192,192,0);
			break;
		}
		case dmInvalid: // 擦除状态
		{
			pDC->SetROP2(R2_COPYPEN);
			lineStyle = PS_SOLID;
			lineWidth = 1;
			color = ::GetBkColor(*pDC); // 用背景色画
			break;
		}
		case dmNormal:   
		default:
		{
			pDC->SetROP2(R2_COPYPEN);
			lineStyle = PS_SOLID;
			lineWidth = 1;
			color = RGB(0,0,0);
			break;
		}		
	}
	pPen->CreatePen(lineStyle,lineWidth,color) ;
}

IMPLEMENT_SERIAL(MEntity, CObject, 0)	

MEntity::MEntity()
{
	m_type = etUnknow;		// EEntityType
	m_nOperationNum = 0;
	Init();
}

MEntity::MEntity(const MEntity& entity)
{
	m_color = entity.m_color;
	m_lineStyle = entity.m_lineStyle;
	m_lineWidth = entity.m_lineWidth;
	m_nOperationNum = 0;
	m_type		= entity.m_type;
}

MEntity::~MEntity()
{
}

void MEntity::Init()
{
	m_color = g_CurColor;
	m_lineStyle = g_CurLineStyle;
	m_lineWidth = g_CurLineWidth ;

	next = NULL;
//	m_bErased = FALSE;
}

MEntity MEntity::operator = (const MEntity& entity)
{
	m_color = entity.m_color;
	m_lineStyle = entity.m_lineStyle;
	m_lineWidth = entity.m_lineWidth;
	m_type		= entity.m_type;
	return *this;
}

void MEntity::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
		ar << m_color << m_lineStyle << m_lineWidth ;
	else
		ar >> m_color >> m_lineStyle >> m_lineWidth ;
}

void MEntity::Append(MEntity * pEnt)
{
	while(next) {
		next = next->next;
	}

	next = pEnt;
}

MEntity * MEntity::GetCurrentEnt()
{
		MEntity * pEntShow = this;
		if(pEntShow->m_nOperationNum > g_pView->m_nCurrentOperation)
			return NULL;

		while(pEntShow) {
			MEntity * pNext = pEntShow->next;
			if(pNext == NULL)
				return pEntShow;
			if( pNext->m_nOperationNum > g_pView->m_nCurrentOperation)  {
				return pEntShow;
			}			

			pEntShow = pNext;
		}

		return pEntShow;
}

MEntity * MEntity::GetShowEnt()
{
/*		MEntity * pEntShow = this;
		if(pEntShow->m_nOperationNum == 0)
			return NULL;

		if(pEntShow->m_nOperationNum > g_pView->m_nCurrentOperation)
			return NULL;

		MEntity * pPrev = pEntShow;
		while(pEntShow->next) {
			if( pEntShow->m_nOperationNum < g_pView->m_nCurrentOperation)  {
				pPrev = pEntShow;
				pEntShow = pEntShow->next;
			}
			else //==
				break;
		}

		if(pEntShow->m_nOperationNum == 0)
			return pPrev;
		else
			return pEntShow;
			*/
	MEntity * pEntShow = GetCurrentEnt();
	if(pEntShow==NULL)
		return NULL;
	
	if(pEntShow->GetType() == etUnknow)
		return NULL;
	else
		return pEntShow;
}

void MEntity::Remove()//清除
{
	MEntity * pNext = this;
	while (pNext) {
		MEntity * pTem = pNext;
		pNext = pNext->next;
		delete pTem;
	}
}

MEntity * MEntity::Erase()//临时删除
{
	MEntity * pShow = GetShowEnt();
	ASSERT_VALID(pShow);
	if(pShow) {
		MEntity * pNew = new MEntity();
		pShow->next = pNew;
		//pNew->m_bErased = TRUE;

		return pNew;
	}

	return NULL;
}
