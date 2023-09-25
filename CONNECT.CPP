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

IMPLEMENT_SERIAL(MConnect, MEntity, 0)

MConnect::MConnect()
{
	Init();
}

MConnect::MConnect(const MConnect& line)
		: MEntity(line)
{
	m_begin = line.m_begin;
	m_end	 = line.m_end;
}

MConnect::MConnect(const Position& begin,const Position& end)
{
	Init();
	m_begin = begin ;
	m_end = end ;

	m_numpts = 2;
	m_pPositions = new Position[2];
	m_pPositions[0] = m_begin;
	m_pPositions[1] = m_end;
}

MConnect::MConnect(int num, Position * pPos)
{
	ASSERT(num >= 2);
	Init();
	
	m_numpts = num;
	m_pPositions = pPos;
	m_begin = m_pPositions[0];
	m_end   = m_pPositions[num - 1];
}

MConnect::~MConnect()
{
//	for(int i=0; i<m_numpts; i++)
	if(m_pPositions) {
		delete [] m_pPositions;
		m_pPositions = NULL;
	}
}

MConnect& MConnect::operator = (const MConnect& line)
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

MEntity* MConnect::Copy()
{
	//MConnect*	pEntity = new MConnect(m_begin, m_end);
	MConnect*	pEntity = new MConnect(m_numpts, m_pPositions);
	Position * ps = new Position[m_numpts];
	for(int i=0; i<m_numpts; i++)
		ps[i] = m_pPositions[i];

	pEntity->m_pPositions = ps;
	return pEntity;
}

void MConnect::Init()
{
	MEntity::Init();
	m_type = etConnect;
	m_begin.Init();
	m_end.Init();
	
	m_pPositions = NULL;

// 	for(int i=0; i<m_numpts; i++) {
// 		m_pPositions[i].Init();
// 	}
}

int	MConnect::GetType()
{
	return etConnect;
}

Position MConnect::GetBeginPos()
{
	return m_begin;
}

Position MConnect::GetEndPos()
{
	return m_end;
}

void MConnect::SetBeginPos(const Position & pos)
{
	m_begin = pos;
	m_pPositions[0] = pos;
}

void MConnect::SetEndPos(const Position & pos)
{
	m_end = pos;
	m_pPositions[m_numpts-1] = pos;
}

void MConnect::Draw(CDC * pDC, int drawMode /* = dmNormal */)
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
	pDC->MoveTo(pts[0]); // 根据屏幕坐标绘制图元
	for(int i=1; i<m_numpts; i++) {
		pDC->LineTo(pts[i]);
	}
	pDC->SelectObject(pOldPen); // 恢复原来的画笔 
	pDC->SetROP2(n); // 恢复原来的绘图模式
	delete [] pts;
}

BOOL MConnect::Pick(const Position& pos, const double pick_radius)
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

	for(int i=0; i<m_numpts-1; i++) {
		double angle = ::GetAngleToXAxis(m_pPositions[i], m_pPositions[i+1]);
		// DIST = fabs(X * cos(a) + Y * sin(a) - P)
		Position dp = objPos - m_pPositions[i];
		double dist = fabs(dp.x*cos(angle) + dp.y*sin(angle) - objPos.Distance(m_pPositions[i]));
		if(dist <= pick_radius)
			return TRUE;
	}

	return FALSE;
}

void MConnect::GetBox(BOX2D* pBox)
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

void MConnect::Move(const Position& basePos,const Position& desPos, BOOL bTemp)
{
	//m_begin = m_begin.Offset(desPos - basePos);
	//m_end = m_end.Offset(desPos - basePos);
	Position pos = basePos;
	for(int i=1; i<m_numpts-1; i++)
		if(pos.Distance(m_pPositions[i]) < 2) {
			m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
			return;
		}

	for(int i=1; i<m_numpts-1; i++)
		m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
}

void MConnect::Rotate(const Position& basePos, const double angle)
{
//	m_begin = m_begin.Rotate(basePos, angle) ;
//	m_end =m_end.Rotate(basePos, angle) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Rotate(basePos, angle);
}

void MConnect::Mirror(const Position& pos1, const Position& pos2)
{
//	m_begin = m_begin.Mirror(pos1, pos2) ;
//	m_end =m_end.Mirror(pos1, pos2) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Mirror(pos1, pos2);
}

void MConnect::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_CONNECT));
}


BOOL MConnect::GetSnapPos(Position& pos)
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

void MConnect::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
//	m_begin.Serialize(ar);
//	m_end.Serialize(ar);
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i].Serialize(ar);
}

void MConnect::Manhadunize()
{
	Position * ps = new Position[m_numpts + 1];
	ps[0] = m_pPositions[0];

	int   nPts = 1;
	int	  XYDir = -1;  //0-X, 1-Y
	for(int i=1; i<m_numpts; i++) {
		//首先判断方向X, Y
		if(fabs(m_pPositions[i].x - ps[nPts-1].x) > fabs(m_pPositions[i].y - ps[nPts-1].y)) { //沿X方向走
			//是否与起点到终点的X方向一致
			if(1) { //(m_pPositions[m_numpts-1].x- m_pPositions[0].x)*(m_pPositions[i].x - m_pPositions[i-1].x) > 0) {//一致
				ps[nPts].x = m_pPositions[i].x;
				ps[nPts].y = ps[nPts-1].y; //m_pPositions[i-1].y;
				if(XYDir == 0) {//两次同向处理
					ps[nPts-1] = ps[nPts];
				}
				else if(ps[nPts].Distance(ps[nPts-1]) < 3) { //距离太小
					ps[nPts] = ps[nPts-1];
				}
				else
					nPts++;
			}
			XYDir = 0;
		}
		else {
			//是否与起点到终点的Y方向一致
			if(1) { //(m_pPositions[m_numpts-1].y- m_pPositions[0].y)*(m_pPositions[i].y - m_pPositions[i-1].y) > 0) {//一致
				ps[nPts].x = ps[nPts-1].x; //m_pPositions[i-1].x;
				ps[nPts].y = m_pPositions[i].y;
				if(XYDir == 1) {//两次同向处理
					ps[nPts-1] = ps[nPts];
				}
				else if(ps[nPts].Distance(ps[nPts-1]) < 3) { //距离太小
					ps[nPts] = ps[nPts-1];
				}
				else
					nPts++;
			}
			XYDir = 1;
		}
	}

	//终点
	ps[nPts++] = m_pPositions[m_numpts-1];

	//删除原节点
	delete []m_pPositions;
	//重新赋值
	m_numpts = nPts;
	m_pPositions = new Position[nPts];
	for(int i = 0; i<nPts; i++)
		m_pPositions[i] = ps[i];

	delete [] ps;
}