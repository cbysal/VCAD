#include "stdafx.h"
#include "math.h"

#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"

#include "Entity.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////
//////////////////////////
//	CLASS	CArc

IMPLEMENT_SERIAL(CArc, MEntity, 0)

CArc::CArc()
{
	Init();
}

CArc::CArc(const CArc& arc)
{
	m_center = arc.m_center;
	m_begin  = arc.m_begin ;
	m_end    = arc.m_end ;
}

CArc::CArc(const Position& center,const Position& pos1, const Position& pos2)
{
	Init();
	m_center = center ;
	m_begin = pos1 ;
	double  radius = m_center.Distance(m_begin) ;

	double	angle1 = GetAngleToXAxis(center, pos1);	
	double  angle2 = GetAngleToXAxis(center, pos2);

	m_begin = pos1;
	m_end.x = radius * cos(angle2) + m_center.x ;
	m_end.y = radius * sin(angle2) + m_center.y ;

}

CArc::~CArc()
{
}

MEntity*	CArc::Copy()
{
	CArc*	pEntity = new CArc(m_center, m_begin, m_end);
	return pEntity;
}

void CArc::Init()
{
	MEntity::Init();

	m_type = etArc;
	m_center.Init();
	m_begin.Init() ;
	m_end.Init() ;
}

int	CArc::GetType()
{
	return etArc;
}

Position CArc::GetEndPos()
{
	return m_end;
}

Position CArc::GetStartPos()
{
	return m_begin;
}
Position CArc::GetCenterPos()
{
	return m_center ;
}
////////////////
BOOL CArc::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	
	Position p[3]; // feature position: start pt, end pt, mid pt
	p[0] = GetStartPos();
	p[1] = GetEndPos();
	p[2] = GetCenterPos();

	for( int i=0; i<3; i++ ){
		if( pos.Distance(p[i]) < PICK_RADIUS ){
			pos = p[i];
			ret = TRUE;
			break;
		}
	}
	return ret;
}
void CArc::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_ARC));
}

/////////////////////
void CArc::Draw(CDC * pDC, int drawMode /* = dmNormal */)
{
	if(m_begin.IsSame(m_end))
		return;
	double radius = m_center.Distance(m_begin) ;

	Position	offset(-radius,radius);
	Position	ltpos = m_center + offset;
	Position	rbpos = m_center - offset;

	CPoint sltp, srbp, ssp, sep ;

	g_pView->WorldtoScreen(m_end, sep ) ;
	g_pView->WorldtoScreen(m_begin, ssp) ;

	g_pView->WorldtoScreen(ltpos,sltp) ;
	g_pView->WorldtoScreen(rbpos,srbp) ;
	
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
	pDC->Arc(sltp.x,sltp.y,srbp.x,srbp.y ,ssp.x, ssp.y, sep.x, sep.y) ;

	pDC->SelectObject(pOldPen) ;
	pDC->SetROP2(n);
}

BOOL CArc::Pick(const Position& pos, const double pick_radius)
{
	Position objPos = pos;
	BOX2D sourceBox,desBox;
	GetBox(&sourceBox); // 得到圆弧的最小包围盒
	// 将最小包围盒向四周放大，得到测试包围盒
	desBox.min[0] = sourceBox.min[0] - pick_radius;
	desBox.min[1] = sourceBox.min[1] - pick_radius;
	desBox.max[0] = sourceBox.max[0] + pick_radius;
	desBox.max[1] = sourceBox.max[1] + pick_radius;
	// 判断拾取点是否在测试包围盒中，如果不是，则图元未被选中
	if( !objPos.IsInBox(desBox) )
		return FALSE;
	else{
		//计算圆弧半径
		double radius = m_center.Distance(m_begin) ;
		//计算圆弧的起始角（相对于x轴）
		double angle1 = GetAngleToXAxis(m_center, m_begin) ;
		//计算圆弧的终止角（相对于x轴）
		double angle2 = GetAngleToXAxis(m_center, m_end) ;
		//计算拾取点和圆弧中心连线与x轴的夹角
		double angle = GetAngleToXAxis(m_center, pos);
		//拾取点到圆心的距离
		double distance =  m_center.Distance(pos) ; 
		//拾取点和圆弧中心连线与x轴的夹角应该存在于
		//起始角和终止角范围之外，并且拾取点到圆心的距离
		//和圆弧半径之差的绝对值小于给定值时，返回TRUE
		if(angle1 > angle2){
			if( (!(angle < (angle1 - pick_radius)&&angle > (angle2 + pick_radius)))
				&&fabs( radius - distance) <= 50 * pick_radius)
				return TRUE ;
		}
		//否则按照正常的判断条件
		if( (angle > (angle1 - pick_radius)&&angle < (angle2 + pick_radius))
			&&fabs( radius - distance) <= 50 * pick_radius)
				return TRUE ;

		return FALSE;
	}
}

void CArc::Move(const Position& basePos,const Position& desPos, BOOL bTemp) 
{
	m_center = m_center.Offset(desPos - basePos);
	m_end = m_end.Offset(desPos - basePos);
	m_begin = m_begin.Offset(desPos - basePos) ;
}

void CArc::Rotate(const Position& basePos, const double angle)
{
	if(angle>DISTANCE_ZERO)
	{
		m_center = m_center.Rotate(basePos , angle);
		m_end = m_end.Rotate(basePos, angle);
		m_begin = m_begin.Rotate(basePos , angle) ;
	}
}

void CArc::Mirror(const Position& FstPos, const Position& SecPos)
{
	Position fp(FstPos), sp(SecPos) ;
	if(fp.Distance(sp)>DISTANCE_ZERO)
	{
//////镜象后的起始点和终止点的位置必须对调；
		m_center = m_center.Mirror(FstPos , SecPos);
		m_end = m_end.Mirror(FstPos, SecPos);
		m_begin = m_begin.Mirror(FstPos , SecPos) ;
		
		Position temp ;
		temp = m_begin ;
		m_begin = m_end ;
		m_end = temp ;
	}
}
	
void CArc::GetBox(BOX2D* pBox)
{
	double MinX, MinY, MaxX, MaxY ,tempX, tempY;
	int pb = 0 ;
	MLines TempLine1( m_center, m_begin) ;
	MLines TempLine2(m_center, m_end) ;
	
	double radius = m_center.Distance(m_begin) ;
	double StartRa = GetAngleToXAxis(m_center, m_begin) ;
	double EndRa = GetAngleToXAxis(m_center, m_end) ;
	MinX = min( m_begin.x, m_end.x );
	MinY = min( m_begin.y, m_end.y );
	MaxX = max( m_begin.x, m_end.x );
	MaxY = max( m_begin.y, m_end.y );

	if(StartRa < EndRa) pb = 1 ;
	else
	{
		MinX = min(MinX, m_center.x+radius) ;
		MinY = min(MinY, m_center.y) ;
		MaxX = max(MaxX, m_center.x+radius) ;
		MaxY = max(MaxY, m_center.y) ;
		tempX = MaxX ;
		tempY = MaxY ;
		pb = 2 ;
	}
	for(int i=0 ;i<4 ;i++)
	{
		if(pb==1 &&PI*i/2.>StartRa &&PI*i/2.<EndRa||
			pb==2 &&!(PI*i/2.>EndRa &&PI*i/2.<StartRa))

		{
			if(i==1)
			{
				tempX = m_center.x  ; tempY = m_center.y+radius ;
			}
			if(i==2)
			{
				tempX = m_center.x - radius ; tempY = m_center.y ;
			}
			if(i==3)
			{
				tempX = m_center.x ; tempY = m_center.y - radius ;
			}
			MinX = min(MinX, tempX) ;
			MinY = min(MinY, tempY) ;
			MaxX = max(MaxX, tempX) ;
			MaxY = max(MaxY, tempY) ;
		}
	}
	pBox->min[0] = MinX ;
	pBox->min[1] = MinY ;
	pBox->max[0] = MaxX ;
	pBox->max[1] = MaxY ;
}

void CArc::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
	m_center.Serialize(ar);
	m_begin.Serialize(ar) ;
	m_end.Serialize(ar) ;
}
