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

/////////////
//define Rectangle class
IMPLEMENT_SERIAL(MText, MEntity, 0)

MText::MText()
{
	Init() ;
}

MText::MText(const MText& rect)
		: MEntity(rect)
{
	m_LeftTop = rect.m_LeftTop ;
	m_RightBottom = rect.m_RightBottom ;
}

MText::MText(const Position& LeftTop,const Position& RightBottom, const char * text)
{
	Init() ;
	m_LeftTop = LeftTop ;
	m_RightBottom = RightBottom ;
	m_Text = text;
}

MText::~MText()
{
}

MEntity* MText::Copy()
{
	MEntity*	pEntity = new MText(m_LeftTop, m_RightBottom, m_Text);
	return pEntity;
}

void MText::Init()
{
	MEntity::Init();
	m_type = etText;
	m_LeftTop.Init();
	m_RightBottom.Init();
}

int MText::GetType()
{
	return etText ;
}

Position MText::GetLeftTopPos()
{
	return m_LeftTop;
}

Position MText::GetRightBottomPos()
{
	return m_RightBottom;
}

CString MText::GetText()
{
	return m_Text;
}

void MText::SetText(const char * text)
{
	m_Text = text;
}

/////////////////
BOOL MText::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	
	Position temp1, temp2; // feature position: start pt, end pt, mid pt
	temp1.x = m_LeftTop.x ;
	temp1.y = m_RightBottom.y ;
	temp2.x = m_RightBottom.x ;
	temp2.y = m_LeftTop.y ;
	
	MLines  tempLine1(m_LeftTop, temp1) ;
	MLines  tempLine2(m_LeftTop, temp2) ;
	MLines  tempLine3(m_RightBottom, temp1) ;
	MLines  tempLine4(m_RightBottom, temp2) ;
	
	if(tempLine1.GetSnapPos(pos) || tempLine2.GetSnapPos(pos)
		|| tempLine3.GetSnapPos(pos) ||tempLine4.GetSnapPos(pos))
		ret = TRUE ;
	return ret;
}
///////////////////////
void MText::Draw(CDC * pDC, int drawMode /* = dmNormal */)
{

	CPoint sltp, srbp;

	g_pView->WorldtoScreen(m_LeftTop, sltp) ;
	g_pView->WorldtoScreen(m_RightBottom, srbp) ;

	int		n = GetROP2(pDC->GetSafeHdc());
	// create a pen by following rules:
	// if in normal draw mode, create a pen by its member variables 
	// if else, create a pen using global funtion "SetDrawEnvir"
	CPen	pen; 
	if( drawMode == dmNormal ) 
		pen.CreatePen(m_lineStyle,m_lineWidth,m_color) ;
	else
		::SetDrawEnvir(pDC, drawMode, &pen);

	CPen* pOldPen = pDC->SelectObject(&pen) ;
	
	if( drawMode == dmNormal )  {
		CRect rect(sltp.x, sltp.y, srbp.x, srbp.y);
		pDC->DrawText(m_Text, rect, DT_CENTER);
	}
	else
	{
		pDC->MoveTo(sltp) ;
		pDC->LineTo(sltp.x, srbp.y) ;
		pDC->LineTo(srbp);
		pDC->LineTo(srbp.x, sltp.y) ;
		pDC->LineTo(sltp) ;
	}
		
	pDC->SelectObject(pOldPen) ;
	pDC->SetROP2(n);
}

void MText::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_TEXT));
}

BOOL MText::Pick(const Position& pos, const double pick_radius) 
{
	Position objPos = pos;
	BOX2D sourceBox,desBox;
	GetBox(&sourceBox); // 得到直线段的最小包围盒
	// 将最小包围盒向四周放大，得到测试包围盒
	desBox.min[0] = sourceBox.min[0] - pick_radius;
	desBox.min[1] = sourceBox.min[1] - pick_radius;
	desBox.max[0] = sourceBox.max[0] + pick_radius;
	desBox.max[1] = sourceBox.max[1] + pick_radius;
	// 判断拾取点是否在测试包围盒中，如果不是，则图元未被选中
	if( !objPos.IsInBox(desBox) )
		return FALSE;

	// 如果选中了矩形的四条边之一，则认为矩形被选中
	Position left_bottom(m_LeftTop.x, m_RightBottom.y);
	Position right_top(m_RightBottom.x, m_LeftTop.y);
	MLines line1(m_LeftTop, left_bottom);
	MLines line2(left_bottom, m_RightBottom);
	MLines line3(m_RightBottom, right_top);
	MLines line4(right_top, m_LeftTop);
	if(line1.Pick(pos, pick_radius) || 
		line2.Pick(pos, pick_radius) || 
		line3.Pick(pos, pick_radius) || 
		line4.Pick(pos, pick_radius) )
		return TRUE;
	
	return FALSE ;
}

void MText::Move(const Position& basePos,const Position& desPos, BOOL bTemp) 
{
	m_LeftTop = m_LeftTop.Offset(desPos - basePos);
	m_RightBottom = m_RightBottom.Offset(desPos - basePos);
}

void MText::Rotate(const Position& basePos, const double angle)
{
	m_LeftTop = m_LeftTop.Rotate(basePos, angle) ;
	m_RightBottom = m_RightBottom.Rotate(basePos, angle) ;
}

void MText::Mirror(const Position& pos1, const Position& pos2)
{
	m_LeftTop = m_LeftTop.Mirror(pos1, pos2) ;
	m_RightBottom = m_RightBottom.Mirror(pos1, pos2) ;
}
	
void MText::GetBox(BOX2D* pBox)
{
	pBox->min[0] = min( m_LeftTop.x, m_RightBottom.x );
	pBox->min[1] = min( m_LeftTop.y, m_RightBottom.y );
	pBox->max[0] = max( m_LeftTop.x, m_RightBottom.x );
	pBox->max[1] = max( m_LeftTop.y, m_RightBottom.y );
}

void MText::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
	m_LeftTop.Serialize(ar);
	m_RightBottom.Serialize(ar);
}