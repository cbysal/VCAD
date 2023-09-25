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

//////////////////////////
//	CLASS	MEllipse

IMPLEMENT_SERIAL(MEllipse, MEntity, 0)

MEllipse::MEllipse()
{
	Init();
}

MEllipse::MEllipse(const MEllipse& ellipse)
	: MEntity(ellipse)
{
	m_LeftTop = ellipse.m_LeftTop ;
	m_RightBottom = ellipse.m_RightBottom ;
}

MEllipse::MEllipse(const Position& center,const Position& around)
{
	Init();
	m_LeftTop = center ;

	m_RightBottom = around ;
}

MEllipse::~MEllipse()
{
}

MEntity*	MEllipse::Copy()
{
	MEllipse*	pEntity = new MEllipse(m_LeftTop, m_RightBottom);
	return pEntity;
}

void MEllipse::Init()
{
	MEntity::Init();
	m_type = etEllipse;
	m_LeftTop.Init();
	m_RightBottom.Init() ;
}

int	MEllipse::GetType()
{
	return etEllipse;
}

Position MEllipse::GetLTPos()
{
	return m_LeftTop;
}

Position MEllipse::GetRBPos()
{
	return m_RightBottom;
}
///////////
BOOL MEllipse::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	
	Position p; // feature position: start pt, end pt, mid pt
	
	p = m_LeftTop;
	
	if( pos.Distance(p) < PICK_RADIUS ){
		pos = p;
		ret = TRUE;
	}
	return ret;
}
///////////////

void MEllipse::Draw(CDC * pDC, int drawMode /* = dmNormal */)
{
	Position ltpos = m_LeftTop; //(m_LeftTop.x - m_dRadius, m_LeftTop.y + m_dRadius ) ;
	Position rbpos = m_RightBottom; //(m_LeftTop.x + m_dRadius, m_LeftTop.y - m_dRadius ) ;
	CPoint ssp, sep, scenp;

	g_pView->WorldtoScreen(ltpos,ssp) ;
	g_pView->WorldtoScreen(rbpos,sep) ;
//	g_pView->WorldtoScreen(m_LeftTop, scenp) ;	

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
	//pDC->SelectStockObject(NULL_BRUSH) ;
	pDC->Ellipse(ssp.x, ssp.y, sep.x, sep.y) ;
	pDC->SelectObject(pOldPen) ;
	pDC->SetROP2(n);
}

void MEllipse::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_ELLIPSE));
}

BOOL MEllipse::Pick(const Position& pos, const double pick_radius) 
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

	desBox.min[0] = sourceBox.min[0] - 50*pick_radius;
	desBox.min[1] = sourceBox.min[1] - 50*pick_radius;
	desBox.max[0] = sourceBox.max[0] + 50*pick_radius;
	desBox.max[1] = sourceBox.max[1] + 50*pick_radius;
	CRgn LargeRgn;
	CRgn LitRgn;
	Position LargeLTPos(desBox.min);
	Position LargeRBPos(desBox.max);
	CPoint p1, p2;

	g_pView->WorldtoScreen(LargeLTPos, p1) ;
	g_pView->WorldtoScreen(LargeRBPos, p2) ;
	LargeRgn.CreateEllipticRgn(p1.x, p1.y, p2.x, p2.y);

	g_pView->WorldtoScreen(m_LeftTop, p1);
	g_pView->WorldtoScreen(m_RightBottom, p2);
	LitRgn.CreateEllipticRgn(p1.x, p1.y, p2.x, p2.y);
	
	CPoint cPos;
	g_pView->WorldtoScreen(pos, cPos);
	if(LargeRgn.PtInRegion(cPos) && !LitRgn.PtInRegion(cPos)) {
		LargeRgn.DeleteObject();
		LitRgn.DeleteObject();
		return TRUE ;
	}
	LargeRgn.DeleteObject();
	LitRgn.DeleteObject();

	return FALSE ;
}

void MEllipse::Move(const Position& basePos,const Position& desPos, BOOL bTemp) 
{
	m_LeftTop = m_LeftTop.Offset(desPos - basePos);
	m_RightBottom = m_RightBottom.Offset(desPos - basePos) ;
}

void MEllipse::Rotate(const Position& basePos, const double angle)
{
	m_LeftTop = m_LeftTop.Rotate(basePos, angle) ;
	m_RightBottom = m_RightBottom.Rotate(basePos, angle) ;
}

void MEllipse::Mirror(const Position& pos1, const Position& pos2)
{
	m_LeftTop = m_LeftTop.Mirror(pos1, pos2) ;
	m_RightBottom = m_RightBottom.Mirror(pos1, pos2) ;
}
	
void MEllipse::GetBox(BOX2D* pBox)
{
	pBox->min[0] = min( m_LeftTop.x, m_RightBottom.x );
	pBox->min[1] = min( m_LeftTop.y, m_RightBottom.y );
	pBox->max[0] = max( m_LeftTop.x, m_RightBottom.x );
	pBox->max[1] = max( m_LeftTop.y, m_RightBottom.y );
}

void MEllipse::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
	m_LeftTop.Serialize(ar);
	m_RightBottom.Serialize(ar);
}