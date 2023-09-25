// VCadDoc.cpp : implementation of the CVCadDoc class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "Entity.h"
#include "VCadView.h"
#include "Dxf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCadDoc

IMPLEMENT_DYNCREATE(CVCadDoc, CDocument)

BEGIN_MESSAGE_MAP(CVCadDoc, CDocument)
	//{{AFX_MSG_MAP(CVCadDoc)
	ON_COMMAND(IDM_SAVE_DXF, OnSaveDxf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCadDoc construction/destruction

CVCadDoc::CVCadDoc()
{
	// TODO: add one-time construction code here
	g_pDoc = this;
	m_pPmtEntity = NULL; // 设置处于提示状态的元素为空
	m_nStep = 0;
}

CVCadDoc::~CVCadDoc()
{
	
}

BOOL CVCadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVCadDoc serialization
void CVCadDoc::Serialize(CArchive& ar)
{
	extern void BitmapSerialize(CArchive& ar);	//将用于预显的位图写入文件
	extern void GenBitmap();	//将客户窗口内的图形存为位图

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		GenBitmap();    //生成预显用的位图 
	}
	else
	{
		// TODO: add loading code here
	}
    BitmapSerialize(ar);//将生成的预显用的位图存盘 By www
	m_EntityList.Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CVCadDoc diagnostics

#ifdef _DEBUG
void CVCadDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVCadDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVCadDoc commands
void CVCadDoc::Draw(CDC *pDC)
{
	// 绘制链表中的图元
	POSITION	pos = m_EntityList.GetHeadPosition();
	while(pos!=NULL)
	{
		MEntity*	pEntity = (MEntity *) m_EntityList.GetNext(pos);
		/////////////////////////////////////////////////////////
		if(pEntity->m_nOperationNum <= g_pView->m_nCurrentOperation) {
			MEntity * pEntShow = pEntity->GetShowEnt();
			if(pEntShow && pEntShow->m_nOperationNum >= 0 )
				pEntShow->Draw(pDC, dmNormal);
		}
	}
	// 绘制选择集中的图元
	for( int i = 0 ; i < m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*) m_selectArray[i] ;
		if(pSelEntity->m_nOperationNum > 0 && pSelEntity->m_nOperationNum <= g_pView->m_nOperations)
	        pSelEntity->Draw(pDC, dmSelect) ;
	}
}
// 清空选择集
void CVCadDoc::RemoveAllSelected()
{
	// 首先将选择集中的元素绘制为正常状态，然后清空选择集
	CDC*	pDC = g_pView->GetDC();
	for( int i = 0 ; i < m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*) m_selectArray[i] ;
        pSelEntity->Draw(pDC, dmNormal) ;
	}
	m_selectArray.RemoveAll() ;
	g_pView->ReleaseDC(pDC);
}
// 判断某一图元对象是否被选中
BOOL CVCadDoc::IsSelected(MEntity* pEntity)
{
	// 判断图元对象是否已经在选择集中
	if( pEntity )
	{
		for( int i = 0 ; i < m_selectArray.GetSize() ; i++ )
		{
			if( pEntity == (MEntity*)m_selectArray[i] )
				return TRUE ;
		}
	}
	return FALSE;
}
void CVCadDoc::OnMouseMove(UINT nFlags, const Position& pos)
{
	if(m_EntityList.GetCount() == 0)
		return;
	::Prompt("拾取一个图元或按Ctrl键拾取多个图元");
	BOOL	bPicked = FALSE;
	MEntity*	pickedEntity = NULL;
	// 遍历图元链表，判断是否存在图元被拾取到
	POSITION	position = m_EntityList.GetHeadPosition();
	while(position != NULL){
		MEntity*	pEntity = (MEntity *)m_EntityList.GetNext(position);
		pEntity = pEntity->GetShowEnt();
		if(pEntity && pEntity->m_nOperationNum > 0) {
			// 得到当前状态的拾取半径
			double curRadius = PICK_RADIUS / g_pView->GetScale(); 
			if( pEntity->Pick(pos, curRadius) ){ // 当某一个图元被拾取到后，即退出循环
				bPicked = TRUE;
				pickedEntity = pEntity;
				break;
			}
		}
	}
	CDC*	pDC = g_pView->GetDC(); // 得到视的设备环境指针
	if( bPicked ){ // 如果某个图元被拾取到
		// 如果以前存在提示图元
		// 那么将提示图元对象恢复到正常绘制状态，并设置为空
		if( m_pPmtEntity ){			
			m_pPmtEntity->Draw( pDC, dmNormal ); 
			m_pPmtEntity = NULL;
		}
		// 将拾取到的图元设置为提示图元
		m_pPmtEntity = pickedEntity ; 
		// 如果提示图元不在选择集中，那么将它高亮显示
		if( ! IsSelected(m_pPmtEntity) ){ 
			//设置光标状态；
			m_pPmtEntity->LoadPmtCursor();
			m_pPmtEntity->Draw( pDC, dmPrompt );
		}
		// 如果提示图元已存在于选择集中，那么将它恢复为空
		else 
			m_pPmtEntity = NULL;
	}
	else{ // 如果没有图元被拾取到
		// 如果以前存在提示图元
		// 那么将提示图元对象恢复到正常绘制状态，并设置为空
		if( m_pPmtEntity ){ 
			m_pPmtEntity->Draw( pDC, dmNormal ); 
			m_pPmtEntity = NULL;
		}
		
		//窗口选择时拖动
		if(m_nStep == 1) {//
/*
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

				Position	prePos, curPos;
			g_pView->ScreentoWorld(m_desPos, prePos); // 获得鼠标所在的前一个位置
			curPos = pos;

*/
			
/*
			// 创建临时对象擦除上一条橡皮线
			Position stPos;
			g_pView->ScreentoWorld(m_basePos, stPos);
			CText*	pTempRect = new CText(stPos, prePos, "");
			if(!bRefresh) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
				pTempRect->Draw(pDC, dmDrag);
			delete pTempRect;
			// 创建临时对象，根据当前位置绘制一条橡皮线
			CText*	pTempRect2 = new CText(stPos, curPos, "");
			pTempRect2->Draw(pDC, dmDrag);
			delete pTempRect2;
*/
			CPoint cPos;
			g_pView->WorldtoScreen(pos, cPos);
			int		n = GetROP2(pDC->GetSafeHdc()); // 得到原来的绘图模式
			CPen	pen; 
			::SetDrawEnvir(pDC, dmDrag, &pen);
			
			CBrush * pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPen* pOldPen = pDC->SelectObject(&pen); // 得到原来的画笔
			pDC->Rectangle(m_basePos.x, m_basePos.y, m_desPos.x, m_desPos.y);
			pDC->SelectObject(pOldPen); // 恢复原来的画笔 
			
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 1, RGB(192,192,0)) ;
			pOldPen = pDC->SelectObject(&pen); // 得到原来的画笔
			pDC->Rectangle(m_basePos.x, m_basePos.y, cPos.x, cPos.y);

			pDC->SelectObject(pOldPen); // 恢复原来的画笔 
			pDC->SelectObject(pOldBrush);
			pen.DeleteObject();
			pDC->SetROP2(n); // 恢复原来的绘图模式
 
			g_pView->WorldtoScreen(pos, m_desPos); // 将当前位置设置为直线终点，以备下一次鼠标移动时用
	}
   }
	g_pView->ReleaseDC(pDC); // 释放视的设备环境指针
}

void CVCadDoc::OnLButtonDown(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC() ;// 得到视的设备环境指针
	// 如果存在提示图元，那么当按下鼠标左键时，将该图元加入到选择集中
	if(m_pPmtEntity){ 
		if( !(nFlags & MK_CONTROL) ) // 如果没有按下Ctrl键，则首先清空选择集
			RemoveAllSelected();
		m_pPmtEntity->Draw(pDC,dmSelect); // 将图元绘制为选中状态
		m_selectArray.Add(m_pPmtEntity);  // 将图元放入选择集中
	}
	else{
//		if( !(nFlags & MK_CONTROL) )// 如果没有按下Ctrl键，则清空选择集
//			RemoveAllSelected();

		//实现窗口选择
		if(m_nStep == 0) {
			m_nStep++;
			g_pView->WorldtoScreen(pos, m_basePos);
			g_pView->WorldtoScreen(pos, m_desPos);
		}
 		else //(m_nStep == 1)
			{
/*
				// 创建临时对象擦除上一条橡皮线
				Position	prePos;
				g_pView->ScreentoWorld(m_desPos, prePos); // 获得鼠标所在的前一个位置
				Position stPos;
				g_pView->ScreentoWorld(m_basePos, stPos);
				CText*	pTempRect = new CText(stPos, prePos, "");
				if(1) // 当视窗没有被刷新时，重画原来的橡皮线使其被擦除
					pTempRect->Draw(pDC, dmDrag);
				delete pTempRect;
*/

			CPoint cPos;
			g_pView->WorldtoScreen(pos, cPos);
			int		n = GetROP2(pDC->GetSafeHdc()); // 得到原来的绘图模式
			CPen	pen; 
			::SetDrawEnvir(pDC, dmDrag, &pen);
			
			CBrush * pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPen* pOldPen = pDC->SelectObject(&pen); // 得到原来的画笔
			pDC->Rectangle(m_basePos.x, m_basePos.y, m_desPos.x, m_desPos.y);
			pDC->SelectObject(pOldPen); // 恢复原来的画笔 
			
			pDC->SelectObject(pOldPen); // 恢复原来的画笔 
			pDC->SelectObject(pOldBrush);
			pen.DeleteObject();
			pDC->SetROP2(n); // 恢复原来的绘图模式


			    CPoint point;
				g_pView->WorldtoScreen(pos, point);
				CRgn pickRgn;
				pickRgn.CreateRectRgn(m_basePos.x, m_basePos.y, point.x, point.y);
				
				// 遍历图元链表，判断是否存在图元被拾取到
				POSITION	position = m_EntityList.GetHeadPosition();
				while(position != NULL){
					MEntity*	pEntity = (MEntity *)m_EntityList.GetNext(position);
					pEntity = pEntity->GetShowEnt();
					if(pEntity == NULL || pEntity->m_nOperationNum == 0)
						continue;
	
					_box2d box;
					pEntity->GetBox(&box);
					CPoint cLTPos, cRBPos;
					g_pView->WorldtoScreen(box.min, cLTPos);
					g_pView->WorldtoScreen(box.max, cRBPos);

					CRect entRect(cLTPos, cRBPos);
					if(pickRgn.RectInRegion(entRect)) {
						pEntity->Draw(pDC,dmSelect); // 将图元绘制为选中状态
						m_selectArray.Add(pEntity);  // 将图元放入选择集中
					}
				}
				pickRgn.DeleteObject();
				m_nStep = 0;
		}
	}
	m_pPmtEntity = NULL; // 将提示图元对象设置为空
	g_pView->ReleaseDC(pDC);// 释放视的设备环境指针
} 

void CVCadDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
    // 清空选择集
	m_selectArray.RemoveAll();
	m_clpArray.RemoveAll();
	// 清除图元链表中的图元对象
	POSITION	pos = m_EntityList.GetHeadPosition();
	while(pos!=NULL)
	{
		MEntity*	pEntity = (MEntity *) m_EntityList.GetNext(pos);
		//delete pEntity ;
		pEntity->Remove();
	}
	m_EntityList.RemoveAll() ;
	
	CDocument::DeleteContents();
}

void CVCadDoc::OnSaveDxf() 
{
	// TODO: Add your command handler code here
	char szFilter[] = "DXF文件(*.DXF)|*.DXF||";
	CFileDialog	dlg(FALSE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	
	if(dlg.DoModal() == IDOK){
		CString filename = dlg.GetPathName();
		if(dlg.GetFileExt() == "")
			filename += ".dxf";
		CDxf	dxf;
		if(!dxf.OutputDxf(this, filename)){
			AfxMessageBox("保存为DXF文件时失败");
			return;
		}
	}  
}

void CVCadDoc::SetModifiedFlag(BOOL bModified)
{
	CDocument::SetModifiedFlag(bModified);
	//图形发生改变,操作数加1
	if(bModified) {
		//删除大于m_nCur操作的m_Opes的操作
		POSITION	position = m_EntityList.GetHeadPosition();
		while(position != NULL){
			MEntity*	pEntity = (MEntity *)m_EntityList.GetNext(position);
			MEntity *   pShowEntity = pEntity->GetCurrentEnt();
			if(pShowEntity != NULL) {
				MEntity * pNext = pShowEntity->next;
				while(pNext) {
					MEntity * pTmp = pNext;
					pNext = pNext->next;
					delete pTmp;
				}
				pShowEntity->next = NULL;
			}
			else {//
				//delete pEntity;
				//POSITION tmp = position;
				//m_EntityList.GetNext(position);
				//m_EntityList.RemoveAt(tmp);
				pEntity->m_nOperationNum = 0;
			}
		}

		g_pView->m_nCurrentOperation++;
		g_pView->m_nOperations = g_pView->m_nCurrentOperation;
	}
}