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
IMPLEMENT_SERIAL(MBlock, MEntity, 0)

MBlock::MBlock()
{
	Init() ;
}

MBlock::MBlock(const MBlock& rect)
		: MEntity(rect)
{
	m_LeftTop = rect.m_LeftTop ;
	m_RightBottom = rect.m_RightBottom ;
	m_Import = GetImportPos();
	m_Outport = GetOutportPos();
}

MBlock::MBlock(const Position& LeftTop,const Position& RightBottom, CBitmap * bmp)
{
	Init() ;
	m_LeftTop = LeftTop ;
	m_RightBottom = RightBottom ;
	m_Bitmap = bmp;
	m_Import = GetImportPos();
	m_Outport = GetOutportPos();
}

MBlock::MBlock(const Position& LeftTop,const Position& RightBottom, const char * text)
{
	Init() ;
	m_LeftTop = LeftTop ;
	m_RightBottom = RightBottom ;
	m_Import = GetImportPos();
	m_Outport = GetOutportPos();
	//1.将text转成实体集;

	//2. 由实体集生成Bitmap
}

MBlock::~MBlock()
{
}

MEntity* MBlock::Copy()
{
	MEntity*	pEntity = new MBlock(m_LeftTop, m_RightBottom, m_Bitmap);
	m_Import = GetImportPos();
	m_Outport = GetOutportPos();

	///////////////////////?????????????????????????????????????????????
	
	return pEntity;
}

void MBlock::Init()
{
	MEntity::Init();
	m_type = etBlock;
	m_LeftTop.Init();
	m_RightBottom.Init();
}

int MBlock::GetType()
{
	return etBlock ;
}

Position MBlock::GetLeftTopPos()
{
	return m_LeftTop;
}

Position MBlock::GetRightBottomPos()
{
	return m_RightBottom;
}

Position MBlock::GetImportPos()
{
	return Position(m_LeftTop.x, (m_RightBottom.y + m_LeftTop.y)/2);
}

Position MBlock::GetOutportPos()
{
	return Position(m_RightBottom.x, (m_RightBottom.y + m_LeftTop.y)/2);
}

CBitmap * MBlock::GetBitmap()
{
	return m_Bitmap;
}

void MBlock::SetBitmap(CBitmap * bmp)
{
	m_Bitmap = bmp;
}

/////////////////
BOOL MBlock::GetSnapPos(Position& pos)
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
void MBlock::Draw(CDC * pDC, int drawMode /* = dmNormal */)
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
		pDC->Rectangle(rect);
		CPoint pLU, pLD, pLM, pRU, pRD, pRM;
		pLU.x = sltp.x;
		pLU.y = sltp.y + (srbp.y - sltp.y)*1/4;
		pLD.x = sltp.x;
		pLD.y = sltp.y + (srbp.y - sltp.y)*3/4;
		pLM.x = sltp.x + (srbp.x - sltp.x)*1/4;
		pLM.y = sltp.y + (srbp.y - sltp.y)*2/4;

		pRU.x = srbp.x - 1;
		pRU.y = pLU.y;
		pRD.x = srbp.x;
		pRD.y = pLD.y;
		pRM.x = sltp.x + (srbp.x - sltp.x)*3/4;
		pRM.y = pLM.y;

		pDC->MoveTo(pLU);
		pDC->LineTo(pLM);
		pDC->LineTo(pLD);
		pDC->MoveTo(pRU);
		pDC->LineTo(pRM);
		pDC->LineTo(pRD);
	}
	else
	{
		CRect rect(sltp.x, sltp.y, srbp.x, srbp.y);
		pDC->Rectangle(rect);
	}
		
	pDC->SelectObject(pOldPen) ;
	pDC->SetROP2(n);
}

void MBlock::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_BLOCK));
}

BOOL MBlock::Pick(const Position& pos, const double pick_radius) 
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

void MBlock::Move(const Position& basePos,const Position& desPos, BOOL bTemp) 
{
	static Position poldimp = GetImportPos();
	static Position poldout = GetOutportPos();
//	m_LeftTop = m_LeftTop.Offset(desPos - basePos);
//	m_RightBottom = m_RightBottom.Offset(desPos - basePos);
	Position bPos;
	bPos = basePos;
	if(bPos.Distance(m_LeftTop) < 5)
		m_LeftTop = m_LeftTop.Offset(desPos - basePos);
	else if(bPos.Distance(m_RightBottom) < 5)
		m_RightBottom = m_RightBottom.Offset(desPos - basePos);
	else {
		m_LeftTop = m_LeftTop.Offset(desPos - basePos);
		m_RightBottom = m_RightBottom.Offset(desPos - basePos);
	}

	if(bTemp)  //临时实体移动
		return;

	//带动Connect的Import起点和终点一起移动
	POSITION	POS = g_pDoc->m_EntityList.GetHeadPosition();
	while(POS!=NULL)
	{
		MEntity*	pEntity = (MEntity *) g_pDoc->m_EntityList.GetNext(POS);
		pEntity = pEntity->GetShowEnt();//处理连接线情况
		if(pEntity == NULL || pEntity->m_nOperationNum ==0)
			continue;

		if(pEntity->GetType() == etConnect) {
			Position ps = ((MConnect*)pEntity)->GetBeginPos();
			Position pe = ((MConnect*)pEntity)->GetEndPos();
			Position pimp = GetImportPos();
			Position pout = GetOutportPos();
			if( (ps.Distance(m_Import) < 0.1) || (ps.Distance(m_Outport) < 0.1) || (pe.Distance(m_Import) < 0.1) || (pe.Distance(m_Outport) < 0.1)) {
				CDC * pDC = g_pView->GetDC();
				BOX2D box;
				pEntity->GetBox(&box);
				CPoint p1, p2;
				g_pView->WorldtoScreen(box.min, p1);
				g_pView->WorldtoScreen(box.max, p2);
				CRect rect(p1, p2);
				g_pView->InvalidateRect(rect, 1);//刷新园区域
				
				//创建连接线的新版本
				MEntity * pNewEnt = pEntity->Copy();

				if(ps.Distance(m_Import) < 0.1) 
					((MConnect*)pNewEnt)->SetBeginPos(pimp);
				if(ps.Distance(m_Outport) < 0.1) 
					((MConnect*)pNewEnt)->SetBeginPos(pout);
				if(pe.Distance(m_Import) < 0.1) 
					((MConnect*)pNewEnt)->SetEndPos(pimp);
				if(pe.Distance(m_Outport) < 0.1) 
					((MConnect*)pNewEnt)->SetEndPos(pout);	
				pNewEnt->Draw(pDC,dmInvalid); // 清除原来位置上的图元
				pNewEnt->GetBox(&box);
				g_pView->WorldtoScreen(box.min, p1);
				g_pView->WorldtoScreen(box.max, p2);
				CRect rect2(p1, p2);
				g_pView->InvalidateRect(rect2, 1);
				pNewEnt->Draw(pDC,dmNormal); // 在目标位置上绘制图元
				g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
				pNewEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
				pEntity->Append(pNewEnt);
				
				g_pView->ReleaseDC(pDC);
			}
		}
	}
	
}

void MBlock::Rotate(const Position& basePos, const double angle)
{
	m_LeftTop = m_LeftTop.Rotate(basePos, angle) ;
	m_RightBottom = m_RightBottom.Rotate(basePos, angle) ;
}

void MBlock::Mirror(const Position& pos1, const Position& pos2)
{
	m_LeftTop = m_LeftTop.Mirror(pos1, pos2) ;
	m_RightBottom = m_RightBottom.Mirror(pos1, pos2) ;
}
	
void MBlock::GetBox(BOX2D* pBox)
{
	pBox->min[0] = min( m_LeftTop.x, m_RightBottom.x );
	pBox->min[1] = min( m_LeftTop.y, m_RightBottom.y );
	pBox->max[0] = max( m_LeftTop.x, m_RightBottom.x );
	pBox->max[1] = max( m_LeftTop.y, m_RightBottom.y );
}

void MBlock::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
	m_LeftTop.Serialize(ar);
	m_RightBottom.Serialize(ar);
}