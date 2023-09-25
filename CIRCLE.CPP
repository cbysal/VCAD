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
//	CLASS	CCircle

IMPLEMENT_SERIAL(CCircle, MEntity, 0)

CCircle::CCircle()
{
	Init();
}

CCircle::CCircle(const CCircle& circle)
	: MEntity(circle)
{
	m_center = circle.m_center ;
	m_dRadius = circle.m_dRadius ;
}
CCircle::CCircle(const Position& center,const double& radius)
{
	Init();
	m_center = center ;
	m_dRadius = radius ;
}
CCircle::CCircle(const Position& center,const Position& around)
{
	Init();
	m_center = center ;

	m_dRadius = m_center.Distance(around) ;
}

CCircle::~CCircle()
{
}

MEntity*	CCircle::Copy()
{
	CCircle*	pEntity = new CCircle(m_center, m_dRadius);
	return pEntity;
}

void CCircle::Init()
{
	MEntity::Init();
	m_type = etCircle;
	m_center.Init();
	m_dRadius = 0 ;
}

int	CCircle::GetType()
{
	return etCircle;
}

Position CCircle::GetCenterPos()
{
	return m_center;
}

double CCircle::GetRadius()
{
	return m_dRadius;
}
///////////
BOOL CCircle::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	
	Position p; // feature position: start pt, end pt, mid pt
	
	p = m_center;
	
	if( pos.Distance(p) < PICK_RADIUS ){
		pos = p;
		ret = TRUE;
	}
	return ret;
}
///////////////

void CCircle::Draw(CDC * pDC, int drawMode /* = dmNormal */)
{
	Position ltpos(m_center.x - m_dRadius, m_center.y + m_dRadius ) ;
	Position rbpos(m_center.x + m_dRadius, m_center.y - m_dRadius ) ;
	CPoint ssp, sep, scenp;

	g_pView->WorldtoScreen(ltpos,ssp) ;
	g_pView->WorldtoScreen(rbpos,sep) ;
	g_pView->WorldtoScreen(m_center, scenp) ;	

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
	pDC->SelectStockObject(NULL_BRUSH) ;
	pDC->Ellipse(ssp.x, ssp.y, sep.x, sep.y) ;
	pDC->SelectObject(pOldPen) ;
	pDC->SetROP2(n);
}

void CCircle::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_CIRCLE));
}

BOOL CCircle::Pick(const Position& pos, const double pick_radius) 
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

	double r = m_center.Distance(pos) ;
	if(fabs(r - m_dRadius) <= 50 * pick_radius)
		return TRUE ;
	return FALSE ;
}

void CCircle::Move(const Position& basePos,const Position& desPos, BOOL bTemp) 
{
	m_center = m_center.Offset(desPos - basePos);
}

void CCircle::Rotate(const Position& basePos, const double angle)
{
	m_center = m_center.Rotate(basePos, angle) ;
}

void CCircle::Mirror(const Position& pos1, const Position& pos2)
{
	m_center = m_center.Mirror(pos1, pos2) ;
}
	
void CCircle::GetBox(BOX2D* pBox)
{
	pBox->min[0] = ( m_center.x -  m_dRadius );
	pBox->min[1] = ( m_center.y -  m_dRadius );
	pBox->max[0] = ( m_center.x +  m_dRadius );
	pBox->max[1] = ( m_center.y +  m_dRadius );
}

void CCircle::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
	m_center.Serialize(ar);
	if(ar.IsStoring())
		ar<<m_dRadius ;
	else
		ar>>m_dRadius ;
}