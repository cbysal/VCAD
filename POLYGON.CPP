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

IMPLEMENT_SERIAL(MPolygon, MEntity, 0)

MPolygon::MPolygon()
{
	Init();
}

MPolygon::MPolygon(const MPolygon& line)
		: MEntity(line)
{
	m_begin = line.m_begin;
	m_end	 = line.m_end;
}

MPolygon::MPolygon(const Position& begin,const Position& end)
{
	Init();
	m_begin = begin ;
	m_end = end ;

	m_numpts = 2;
	m_pPositions = new Position[2];
	m_pPositions[0] = m_begin;
	m_pPositions[1] = m_end;
}

MPolygon::MPolygon(int num, Position * pPos)
{
	ASSERT(num >= 2);
	Init();
	
	m_numpts = num;
	m_pPositions = pPos;
	m_begin = m_pPositions[0];
	m_end   = m_pPositions[num - 1];
}

MPolygon::~MPolygon()
{
//	for(int i=0; i<m_numpts; i++)
	if(m_pPositions) {
		delete [] m_pPositions;
		m_pPositions = NULL;
	}
}

MPolygon& MPolygon::operator = (const MPolygon& line)
{
	// 处理特殊情况：line1 = line1
	if(this == &line)
		return *this;
	// 一般情形：line2 = line1
	MEntity::operator = (line); // 调用基类的重载“=”操作
	m_begin = line.m_begin;
	m_end = line.m_end;
	
	m_pPositions = line.m_pPositions;
	return *this;
}

MEntity* MPolygon::Copy()
{
	//MPolygon*	pEntity = new MPolygon(m_begin, m_end);
	MPolygon*	pEntity = new MPolygon(m_numpts, m_pPositions);
	Position * ps = new Position[m_numpts];
	for(int i=0; i<m_numpts; i++)
		ps[i] = m_pPositions[i];

	pEntity->m_pPositions = ps;
	return pEntity;
}

void MPolygon::Init()
{
	MEntity::Init();
	m_type = etPolygon;
	m_begin.Init();
	m_end.Init();
	
	m_pPositions = NULL;

// 	for(int i=0; i<m_numpts; i++) {
// 		m_pPositions[i].Init();
// 	}
}

int	MPolygon::GetType()
{
	return etPolygon;
}

Position MPolygon::GetBeginPos()
{
	return m_begin;
}

Position MPolygon::GetEndPos()
{
	return m_end;
}

void MPolygon::Draw(CDC * pDC, int drawMode /* = dmNormal */)
{
	CPoint pt_begin, pt_end; // 屏幕坐标的起点和终点
	CPoint * pts = new CPoint[m_numpts];
	
	g_pView->WorldtoScreen(m_begin,pt_begin); // 将世界坐标转化为屏幕坐标
	g_pView->WorldtoScreen(m_end,pt_end) ;
	for(int i=0; i<m_numpts; i++)
		g_pView->WorldtoScreen(m_pPositions[i], pts[i]);

	int		n = GetROP2(pDC->GetSafeHdc()); // 得到原来的绘图模式
	CPen	pen; 
	if( drawMode == dmNormal )  // 如果为正常状态的绘制，根据成员变量创建画笔
		pen.CreatePen(m_lineStyle,m_lineWidth,m_color) ;
	else // 非正常状态调用SetDrawEnvir函数设置绘图环境
		::SetDrawEnvir(pDC, drawMode, &pen);
	
	CPen* pOldPen = pDC->SelectObject(&pen); // 得到原来的画笔
	pDC->Polygon(pts, m_numpts); // 根据屏幕坐标绘制图元
	pDC->SelectObject(pOldPen); // 恢复原来的画笔 
	pDC->SetROP2(n); // 恢复原来的绘图模式
	delete [] pts;
}

BOOL MPolygon::Pick(const Position& pos, const double pick_radius)
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

	for(int i=0; i<m_numpts; i++) {
		int i1 = i+1;
		if(i == m_numpts-1)
			i1 = 0;
		double angle = ::GetAngleToXAxis(m_pPositions[i], m_pPositions[i1]);
		// DIST = fabs(X * cos(a) + Y * sin(a) - P)
		Position dp = objPos - m_pPositions[i];
		double dist = fabs(dp.x*cos(angle) + dp.y*sin(angle) - objPos.Distance(m_pPositions[i]));
		if(dist <= pick_radius)
			return TRUE;
	}

	return FALSE;
}

void MPolygon::GetBox(BOX2D* pBox)
{
	double min0 = 1E20;
	double min1 = 1E20;
	double max0 = -1E20;
	double max1 = -1E20;

	for(int i=0; i<m_numpts; i++) {
		min0 = min(min0, m_pPositions[i].x);
		min1 = min(min1, m_pPositions[i].y);
		max0 = max(max0, m_pPositions[i].x);
		max1 = max(max1, m_pPositions[i].y);
	}
	pBox->min[0] = min0; //min( m_begin.x, m_end.x );
	pBox->min[1] = min1; //min( m_begin.y, m_end.y );
	pBox->max[0] = max0; //max( m_begin.x, m_end.x );
	pBox->max[1] = max1; //max( m_begin.y, m_end.y );
}

void MPolygon::Move(const Position& basePos,const Position& desPos, BOOL bTemp)
{
	//m_begin = m_begin.Offset(desPos - basePos);
	//m_end = m_end.Offset(desPos - basePos);
	Position pos = basePos;
	for(int i=0; i<m_numpts; i++)
		if(pos.Distance(m_pPositions[i]) < 2) {
			m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
			return;
		}

		int i;
	for(i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
}

void MPolygon::Rotate(const Position& basePos, const double angle)
{
//	m_begin = m_begin.Rotate(basePos, angle) ;
//	m_end =m_end.Rotate(basePos, angle) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Rotate(basePos, angle);
}

void MPolygon::Mirror(const Position& pos1, const Position& pos2)
{
//	m_begin = m_begin.Mirror(pos1, pos2) ;
//	m_end =m_end.Mirror(pos1, pos2) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Mirror(pos1, pos2);
}

void MPolygon::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_POLYGON));
}


BOOL MPolygon::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	
/*
	Position p[3]; // feature position: start pt, end pt, mid pt
	p[0] = m_begin;
	p[1] = m_end;
	p[2] = (p[0] + p[1]) * 0.5;

	for( int i=0; i<3; i++ ){
		if( pos.Distance(p[i]) < 0.5 ){
			pos = p[i];
			ret = TRUE;
			break;
		}
	}
*/
	for( int i=0; i<m_numpts; i++ ){
		if( pos.Distance(m_pPositions[i]) < 0.5 ){
			pos = m_pPositions[i];
			ret = TRUE;
			break;
		}
	}

	return ret;
}

void MPolygon::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
//	m_begin.Serialize(ar);
//	m_end.Serialize(ar);
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i].Serialize(ar);
}

