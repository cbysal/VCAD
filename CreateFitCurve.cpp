#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"
#include "sysmath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateFitCurve::CCreateFitCurve(UINT nCmd)
	: m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // 初始化操作步为 0
	m_pPositions = NULL;
	m_nCmd = nCmd;
}

CCreateFitCurve::~CCreateFitCurve()
{
	if(m_pPositions) {
		delete [] m_pPositions;
		m_pPositions = NULL;
	}
}

int CCreateFitCurve::GetType()
{
	return ctCreateLine; 
}

int	CCreateFitCurve::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // 每次单击鼠标左键时操作步加 1 

	int  i;
	//保存点集
	Position * pNew = new Position[m_nStep];
	for( i=0; i<m_nStep-1; i++)
		pNew[i] = m_pPositions[i];
	pNew[i] = pos;
	if(m_nStep > 1 && m_pPositions)
		delete [] m_pPositions;
	m_pPositions = pNew;

	switch(m_nStep) // 根据操作步执行相应的操作
	{
		case 1: 
		{
			m_begin = m_end = pos;

			::Prompt("请输入直线的下一点：");
			break;
		}
		default:
		{
			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 

			// 擦除在拖动状态时显示的最后一条线
			//MLines*	pTempLine = new MLines(m_begin,m_end); 
			//pTempLine->Draw(pDC, dmDrag);
			//delete pTempLine;

			// 如果在按鼠标左键的过程中同时按下了Shift键，
			//	那么根据鼠标单击位置绘制水平线或竖直线
			if( nFlags & MK_SHIFT ){ 
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx) <= fabs(dy)) // 如果鼠标单击位置在X方向靠近起点
					m_end.Set(m_begin.x, pos.y); // 那么终点的x坐标与起点的相同
				else
					m_end.Set(pos.x,m_begin.y);
			}
			else
				m_end = pos; // 如果未按下Shift键, 则终点为鼠标单击位置			
			
			m_begin = pos;
			break;
		}		
	}

	return 0;
}


//拟合主程序，由点集合得到 二次函数的系数
void GetABC(int num, Position * pts,  double &a, double &b, double & c)
{

	double A11, A12, A13, B1;
	double A21, A22, A23, B2;
	double A31, A32, A33, B3;

	int i =0;
	//计算矩阵系数
	A11 = 0;
	for (i=0; i<num; i++)
	{
		A11 += 1;//pts[i].x ^ 0;
	}

	A12 = 0;
	for (i=0; i<num; i++)
	{
		A12 += pts[i].x;
	}

	A13 = 0;
	for (i=0; i<num; i++)
	{
		A13 += pts[i].x*pts[i].x;
	}

	A21 = A12;
	A22 = A13;
	
	A23 = 0;
	for (i=0; i<num; i++)
	{
		A23 += pts[i].x*pts[i].x*pts[i].x;
	}

	A31 = A22;
	A32 = A23;
	
	A33 = 0;
	for (i=0; i<num; i++)
	{
		A33 += pts[i].x*pts[i].x*pts[i].x*pts[i].x;
	}

	B1 = 0;
	for (i=0; i<num; i++)
	{
		B1 += 1*pts[i].y;
	}

	B2 = 0;
	for (i=0; i<num; i++)
	{
		B2 += pts[i].x*pts[i].y;
	}

	B3 = 0;
	for (i=0; i<num; i++)
	{
		B3 += pts[i].x*pts[i].x*pts[i].y;
	}


	//求解方程组，三元一次方程组
	double a11 = A21 - A11*A23/A13;
	double a12 = A22 - A12*A23/A13;
	double b1 = B2 - A23*B1/A13;
	double a21 = A31 - A11*A33/A13;
	double a22 = A32 - A12*A33/A13;
	double b2 = B3 - A33*B1/A13;

    a = (b2-b1*a22/a12) / (a21 - a11*a22/a12);
	b = (b1 - a11*a) / a12;

	double x0 = 0;
	x0 = a11*a + a12*b - b1;
	x0 = a21*a + a22*b - b2;

	c = (B1 - A11*a - A12*b)/A13;

	x0 = A11*a + A12*b + A13*c - B1;
	x0 = A21 * a + A22* b + A23*c - B2;
	x0 = A31*a + A32 * b + A33*c - B3;

	return;
}


//由二次函数方程得到一系列的点
void GeneratePoints(double a, double b,double c,int num, Position *pts)
{
	double xmin = pts[0].x;
	double xmax = pts[0].y;

	double delta = (xmax - xmin) / (num-1);
	for (int i=0; i<num; i++)
	{
		double x = xmin + delta * i;
		double y = c*x*x + b*x + a;

		pts[i].x = x;
		pts[i].y = y;
	}
}

#define MAXDETA 0.00001

void GetBS3OpenControl(Position *ValuePoint, Position *ControlPoint,int ValuePointNum)
{
	if(ValuePointNum<2) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/6xQ(i-1)+2/3xQ(i)+1/6xQ(i+1) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=ValuePointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[1];
	ControlPoint[ValuePointNum+1]=ControlPoint[ValuePointNum];
	
	// Step 2:Caculate deta(i)
	Position deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta.x=0.0;  deta.y=0.0;
		maxdeta.x=0.0;  maxdeta.y=0.0;
		for(i=1;i<=ValuePointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-(ControlPoint[i-1]+ControlPoint[i+1])*0.5)*0.5;
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[1];
		ControlPoint[ValuePointNum+1]=ControlPoint[ValuePointNum];
	}while((~maxdeta)>MAXDETA);
}


extern void bezier_n(int n, Position * pts,    int m, Position* pt);
extern void Draw_BSpline3(int n, Position *pts, int num, Position * ptOut);



//删除生成的临时直线段
void DeletemLines(CObList &newEnts)
{
    POSITION pos = newEnts.GetHeadPosition();
	while (pos)
	{
		MEntity* pEnt = (MEntity*)newEnts.GetNext(pos);

		delete pEnt;
	}

	newEnts.RemoveAll();
}

//将图形实体中的多段线分割成直线段
void DivideMLines(CObList &oldEnts, CObList &newEnts)
{
    POSITION pos = oldEnts.GetHeadPosition();
	while (pos)
	{
		MEntity* pEnt = (MEntity*) oldEnts.GetNext(pos);

		if (pEnt->GetType() == etLine)
		{
			int num = ((MLines*)pEnt)->GetNumOfPoints();
			//Position * pts = ((MLines*)pEnt)->GetPoints();
			std::vector<Position> pts = ((MLines*)pEnt)->GetPoints();

			for (int i=0; i<num-1; i++)
			{
				std::vector<Position> p2ts;// = new Position[2];
				p2ts.push_back(pts[i]);
				p2ts.push_back(pts[i+1]);

				MLines * pLine = new MLines(p2ts);
				newEnts.AddTail(pLine);
			}
		}
	}	
}

//是否为同一点
BOOL IsSamePoint(Position pt1, Position pt2)
{
	if (fabs(pt1.x - pt2.x) < 1E-8 && fabs(pt1.y - pt2.y) < 1E-8 )
	{
		return TRUE;
	}

	return FALSE;
}

//点pt是否为实体pEnt的断点
BOOL IsEndPoint(Position pt, MEntity * pEnt)
{
	ASSERT(pEnt->GetType() == etLine);

	MLines * pLine = (MLines*)pEnt;

	if (IsSamePoint(pt, pLine->GetBeginPos()) || IsSamePoint(pt, pLine->GetEndPos()))
	{
		return TRUE;
	}

	return FALSE;
}

//点是否在环上
BOOL PointInLoop(Position pt, CList<Position, Position>& ptLoop)
{
	POSITION pos = ptLoop.GetHeadPosition();
	while (pos)
	{
		Position pt1 = ptLoop.GetNext(pos);
		if (IsSamePoint(pt, pt1))
		{
			return TRUE;
		}
	}

	return FALSE;
}

double GetAngle(double x0, double y0, double x1, double y1, double x2, double y2)
// 按顺时针方向求角POQ: P(x1,y1), O(x0,y0), Q(x2,y2)
// 返回值在 (0 -- 2 Pi) 之间，单位为弧度
{	
	#define Pi 3.1415926535897932
	double x, y, r, alfa;
	x=(x1-x0)*(x2-x0)+(y1-y0)*(y2-y0);
	y=-(x1-x0)*(y2-y0)+(x2-x0)*(y1-y0);
	r=sqrt(x*x+y*y);
	alfa=acos(x/r);
	if(y<=0.0) alfa=2*Pi-alfa;

	return alfa;
}

double GetDistance(Position p1, Position p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

//求交点
int GetInterPoint(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double &x, double &y )
{	// 有解，返回1；无解，返回0；
	double delta;
	delta = -(x2-x3)*(y0-y1)+(x0-x1)*(y2-y3);

	if(fabs(delta) < 1E-8)	// 先解决直线平行
	{
		if( fabs((y1-y0)*(x2-x0)-(y2-y0)*(x1-x0)) < 1E-8 )	// 共线
		{	if ( (x2-x0)*(x1-x2) >= 0.0 && (y2-y0)*(y1-y2) >= 1E-10 ) 
			{	x=x2; y=y2; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else if ( (x3-x0)*(x1-x3) >= 0.0 && (y3-y0)*(y1-y3) >= 1E-10 ) 
			{	x=x3; y=y3; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else if ( (x0-x2)*(x3-x0) >= 0.0 && (y0-y2)*(y3-y0) >= 1E-10 ) 
			{	x=x3; y=y3; 
				//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
				return 1;}
			else return 0;
		}
		else return 0;
	}
	else
	{
		x = ((x2-x3)*(x0*y1-x1*y0)+(x0-x1)*(x3*y2-x2*y3))/delta;
		y = ((y2-y3)*(x0*y1-x1*y0)+(y0-y1)*(x3*y2-x2*y3))/delta;
		//printf(" x, y, delta = %g, %g,     %g", x, y, delta);
		if ( (x-x2)*(x3-x) >= -1E-10 && (x-x0)*(x1-x) >= -1E-10 ) return 1;
		else return 0;
	}	
}

//当前的起点pStart, 终点pEnd, 实体pEnt,在实体集mLines中寻找下一个实体pEnt，及其起点和终点
//找到返回True，开环时返回为false
BOOL GetNextPoints(Position &pStart, Position &pEnd, CObList &mLines, MEntity *& pEnt)
{
	BOOL bReturn = FALSE;
	//得到第一个交点
	Position pInter;
	//交点处的实体集, 去除自己
	CObList interLines;
	double dis0 = 1E10; 
	POSITION pos = mLines.GetHeadPosition();
	while (pos)
	{
		MLines * pLine = (MLines*)mLines.GetNext(pos);
		if (pLine == pEnt)
		{
			continue;
		}
		
		Position pSt = pLine->GetBeginPos();
		Position pEd = pLine->GetEndPos();

		Position xy;
		if (GetInterPoint(pStart.x, pStart.y, pEnd.x, pEnd.y, pSt.x, pSt.y, pEd.x, pEd.y, xy.x, xy.y))
		{
			if (IsSamePoint(xy, pStart))
			{
				continue;
			}

			
			bReturn = TRUE;

			double dis = GetDistance(pStart, xy);

			if (fabs(dis - dis0) < 1E-8) //同交点
			{
				interLines.AddTail(pLine);
				pInter = xy;
			}
			else if (dis < dis0)
			{
				interLines.RemoveAll();
				interLines.AddTail(pLine);
				pInter = xy;

				dis0 = dis;
			}
		}
	}

	//得到interLines所有起点、终点，交点夹角最小的
    pos = interLines.GetHeadPosition();

	double alfa = 1E10;
	while (pos)
	{
		MLines * pLine = (MLines*)interLines.GetNext(pos);

		Position ptStart = pLine->GetBeginPos();
		if (!IsSamePoint(ptStart, pInter))
		{
			double interAngle = GetAngle(pInter.x, pInter.y, pStart.x, pStart.y, ptStart.x, ptStart.y);

			if (interAngle < alfa)
			{
				pEnd = ptStart;
				pEnt = pLine;
				alfa = interAngle;
			}
		}

		Position ptEnd = pLine->GetEndPos();
		if (!IsSamePoint(ptEnd, pInter))
		{
			double interAngle = GetAngle(pInter.x, pInter.y, pStart.x, pStart.y, ptEnd.x, ptEnd.y);

			if (interAngle < alfa)
			{
				pEnd = ptEnd;
				pEnt = pLine;
				alfa = interAngle;
			}
		}
	}

	//交点pInter为下一个起点
	pStart = pInter;

	return bReturn;
}


//鼠标左键双击创建多线
int CCreateFitCurve::OnLButtonDblClk(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针 
	
	// 擦除在拖动状态时显示的最后一条线
	MLines*	pTempLine = new MLines(m_begin,m_end); 
	pTempLine->Draw(pDC, dmDrag);
	delete pTempLine;
	
	// 如果在按鼠标左键的过程中同时按下了Shift键，
	//	那么根据鼠标单击位置绘制水平线或竖直线
	if( nFlags & MK_SHIFT ){ 
		double dx = pos.x - m_begin.x;
		double dy = pos.y - m_begin.y;
		if(fabs(dx) <= fabs(dy)) // 如果鼠标单击位置在X方向靠近起点
			m_end.Set(m_begin.x, pos.y); // 那么终点的x坐标与起点的相同
		else
			m_end.Set(pos.x,m_begin.y);
	}
	else
		m_end = pos; // 如果未按下Shift键, 则终点为鼠标单击位置			
	

//////////////////////////////////////////////////////////////////////////
	//区域识别, 陈德智
	if (m_nCmd == 32816)
	{
		//鼠标拾取内点
		Position ptInner = m_end;

		//首先向右做水平射线，求到第一个交点
		Position pEnd0 = ptInner;
		pEnd0.x = 1E3;

		//将直线多短分割成简单的实体
		CObList lines;
		DivideMLines(g_pDoc->m_EntityList, lines);

		//在第一个交点处，获得第一个实体，记录起点
		Position ptFirst = ptInner;
		MEntity * pEnt = NULL;
		ASSERT( GetNextPoints(ptFirst, pEnd0, lines, pEnt) );

		CList<Position, Position> ptLoop;
		if (IsEndPoint(ptFirst, pEnt))
		{
			ptLoop.AddTail(ptFirst);
		}

		//根据起点到实体的终点为方向
		//回到起点时结束
		while (1)
		{
			CCircle*	pTempCircle = new CCircle(ptFirst, 4); 
			pTempCircle->Draw(pDC,dmNormal); // 绘制多边形
			g_pDoc->m_EntityList.AddTail(pTempCircle); // 将多边形指针添加到图元链表
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pTempCircle->m_nOperationNum = g_pView->m_nCurrentOperation;	

			pTempCircle = new CCircle(pEnd0, 2); 
			pTempCircle->Draw(pDC,dmNormal); // 绘制多边形
			g_pDoc->m_EntityList.AddTail(pTempCircle); // 将多边形指针添加到图元链表
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pTempCircle->m_nOperationNum = g_pView->m_nCurrentOperation;	

			BOOL bLoop = GetNextPoints(ptFirst, pEnd0, lines, pEnt);
			if (PointInLoop(ptFirst, ptLoop))
			{
				//成环了，退出
				break;
			}

			if (!bLoop)
			{
				AfxMessageBox("开环！");
				DeletemLines(lines);
				
				return 0;
			}
			else {
				ptLoop.AddTail(ptFirst);
			}
		}

		//显示点的序列
		int num = ptLoop.GetCount();
		Position * pPositions = new Position[num];
		POSITION pos = ptLoop.GetHeadPosition();
		int i = 0;
		while(pos)
		{
			Position pt = ptLoop.GetNext(pos);
			pPositions[i++] = pt;
		}
		MPolygon*	pNewLine = new MPolygon(num, pPositions);// 根据起点和终点创建多边形
		pNewLine->Draw(pDC,dmNormal); // 绘制多边形
		g_pDoc->m_EntityList.AddTail(pNewLine); // 将多边形指针添加到图元链表
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;	

		DeletemLines(lines);
	}

//////////////////////////////////////////////////////////////////////////
//绘制控制多边形;
	if(m_nStep >= 2) {
		MLines*	pNewLine = new MLines(m_nStep,m_pPositions);// 根据起点和终点创建直线
		pNewLine->SetColor(RGB(0,0,0));
		pNewLine->SetWidth(1);
		pNewLine->Draw(pDC,dmNormal); // 绘制直线
		g_pDoc->m_EntityList.AddTail(pNewLine); // 将直线指针添加到图元链表
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;

//////////////////////////////////////////////////////////////////////////
//最小二乘法
		int num = 0;
		Position * pts = NULL;
	
		if (m_nCmd == 32810)
		{
			num = 100;
			pts = new Position[num];

			//二次拟合曲线 y = c*x^2 + b*x + a
			double a, b, c;
			GetABC(m_nStep, m_pPositions, a, b, c);

			//从起点到终点离散100个点
			pts[0].x = m_pPositions[0].x;
			pts[0].y = m_pPositions[m_nStep-1].x;
			GeneratePoints(a, b, c, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
		//Bezier
		else if (m_nCmd == 32811)
		{
			num = 100;
			pts = new Position[num];
			bezier_n(m_nStep-1, m_pPositions, num, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline
		else if (m_nCmd == 32812 && m_nStep >= 4)
		{
			num = 100;
			num = num * (m_nStep-3);

			pts = new Position[num];

			Draw_BSpline3(m_nStep, m_pPositions, 100, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline反算
		else if (m_nCmd == 32815) //
		{
			//计算控制顶点
			Position * vCtlPts = new Position[m_nStep + 2];
			GetBS3OpenControl(m_pPositions, vCtlPts, m_nStep);


			num = 100;
			num = num * (m_nStep-1);

			pts = new Position[num];

			Draw_BSpline3(m_nStep+2, vCtlPts, 100, pts);
		}
	

		//创建拟和曲线
		if (num >= 2)
		{
			pNewLine = new MLines(num, pts);// 根据起点和终点创建直线
			pNewLine->Draw(pDC,dmNormal); // 绘制直线
			g_pDoc->m_EntityList.AddTail(pNewLine); // 将直线指针添加到图元链表
			g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
			pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;
		}
		if (pts) delete[]pts;
	}
	
	g_pView->ReleaseDC(pDC); // 释放设备环境指针
	
	m_nStep = 0;  // 将操作步重置为 0
	::Prompt("请输入直线的起点：");
	
	return 0;
}

int	CCreateFitCurve::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_LINE));

	// 用一静态变量nPreRefresh记录进入OnMouseMove状态时的刷新次数
	static	int nPreRefresh = g_nRefresh; 
	// 布尔变量bRefresh说明在OnMouseMove过程中视窗是否被刷新
	BOOL	bRefresh = FALSE; 
	// nCurRefresh用于记录当前的刷新次数
	int		nCurRefresh = g_nRefresh; 
	// 如果nCurRefresh和nPreRefresh不相等，说明视窗曾被刷新过
	if(nCurRefresh != nPreRefresh){ 
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh; 
	}

	switch(m_nStep)
	{
		case 0:
			::Prompt("请输入直线的起点：");
			break;
		default:
		{

			Position	prePos, curPos;
			prePos = m_end; // 获得鼠标所在的前一个位置
			
			// 如果在按鼠标左键的过程中同时按下了Shift键，
			//	那么根据鼠标单击位置绘制水平线或竖直线
			if( nFlags & MK_SHIFT ){
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx)>=fabs(dy))
					curPos.Set(pos.x,m_begin.y);
				else
					curPos.Set(m_begin.x, pos.y);
			}
			else
				curPos = pos;

			CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
			
			// 创建临时对象擦除上一条橡皮线
			MLines*	pTempLine1 = new MLines(m_begin, prePos);
			if(!bRefresh) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// 创建临时对象，根据当前位置绘制一条橡皮线
			MLines*	pTempLine2 = new MLines(m_begin, curPos);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
 

	if(m_nStep >= 2) {
//////////////////////////////////////////////////////////////////////////
//最小二乘法
		int num = 0;
		Position * ptsOld = NULL;
		Position * pts = NULL;

		Position * vPtsPre = new Position[m_nStep+1];
		Position * vPtsCur = new Position[m_nStep+1];
		for (int k=0; k<m_nStep; k++)
		{
			vPtsCur[k] = m_pPositions[k];
			vPtsPre[k] = m_pPositions[k];
		}
		vPtsPre[m_nStep] = prePos;
		vPtsCur[m_nStep] = curPos;
	
		if (m_nCmd == 32810)
		{
			num = 100;
			ptsOld = new Position[num];
			pts = new Position[num];

			//二次拟合曲线 y = c*x^2 + b*x + a
			double a, b, c, a2, b2, c2;
			GetABC(m_nStep+1, vPtsPre, a, b, c);
			GetABC(m_nStep+1, vPtsCur, a2, b2, c2);

			//从起点到终点离散100个点
			ptsOld[0].x = vPtsPre[0].x;
			ptsOld[0].y = vPtsPre[m_nStep].x;
			pts[0].x = vPtsCur[0].x;
			pts[0].y = vPtsCur[m_nStep].x;
			GeneratePoints(a, b, c, 100, ptsOld);
			GeneratePoints(a2, b2, c2, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
		//Bezier
/*		else if (m_nCmd == 32811)
		{
			num = 100;
			pts = new Position[num];
			bezier_n(m_nStep-1, m_pPositions, num, pts);
		}
//////////////////////////////////////////////////////////////////////////
		//BSpline
		else if (m_nCmd == 32812 && m_nStep >= 4)
		{
			num = 100;
			num = num * (m_nStep-3);

			pts = new Position[num];

			Draw_BSpline3(m_nStep, m_pPositions, 100, pts);
		}

//////////////////////////////////////////////////////////////////////////
		//BSpline反算
		else if (m_nCmd == 32815) //
		{
			//计算控制顶点
			Position * vCtlPts = new Position[m_nStep + 2];
			GetBS3OpenControl(m_pPositions, vCtlPts, m_nStep);


			num = 100;
			num = num * (m_nStep-1);

			pts = new Position[num];

			Draw_BSpline3(m_nStep+2, vCtlPts, 100, pts);
		}
	
*/
		//创建拟和曲线
		if (num >= 2)
		{
			// 创建临时对象擦除上一条橡皮线
			MLines*	pTempLine1 = new MLines(num, ptsOld);
			if(!bRefresh) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// 创建临时对象，根据当前位置绘制一条橡皮线
			MLines*	pTempLine2 = new MLines(num, pts);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
		}

		delete []vPtsCur;
		delete []vPtsPre;
	}


			g_pView->ReleaseDC(pDC); // 释放设备环境指针			

			m_end = curPos; // 将当前位置设置为直线终点，以备下一次鼠标移动时用
			break;
		}
	}
	return 0;
}

// 单击鼠标右键取消当前的操作
int	CCreateFitCurve::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("请输入直线的起点：");
	return 0;
}
// 调用Cancel 函数取消本次操作
int CCreateFitCurve::Cancel()
{
	// 如果当前的操作步为 1 ，那么要在结束本次操作前擦除上次鼠标移动时绘制的橡皮线
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // 得到设备环境指针
		Position	prePos = m_end; // 获得鼠标所在的前一个位置
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // 擦除上一次绘制的橡皮线
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // 释放设备环境指针
	}
	m_nStep = 0; // 将操作步重置为 0 
	::Prompt("就绪"); // 等待提示新类型的命令操作
	return 0 ;
}

