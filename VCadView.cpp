// VCadView.cpp : implementation of the CVCadView class
//

#include "stdafx.h"
#include "VCad.h"
#include "math.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Base.h"
#include "Command.h"
#include "CreateCmd.h"
#include "ModifyCmd.h"
#include "ChgViewCmd.h"
#include "Entity.h"
#include "TextInputDlg.h"
#include "ECT2DModelDlg.h"
#include "GearParamDlg.h"
#include "InterDlg.h"
#include "sysmath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCadView

IMPLEMENT_DYNCREATE(CVCadView, CView)

BEGIN_MESSAGE_MAP(CVCadView, CView)
	//{{AFX_MSG_MAP(CVCadView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_PICK, OnPick)
	ON_UPDATE_COMMAND_UI(ID_PICK, OnUpdatePick)
	ON_COMMAND(ID_REDRAW, OnRedraw)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_ERASE, OnUpdateModifyErase)
	ON_COMMAND(ID_EDIT_MANHADUN, OnEditManhadun)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MANHADUN, OnUpdateEditManhadun)
	ON_COMMAND(ID_MENUITEM32807, OnMenuitem32807)
	ON_COMMAND(ID_MENUITEM32808, OnMenuitem32808)
	ON_COMMAND(ID_MENUITEM32809, OnMenuitem32809)
	ON_COMMAND(ID_MENUITEM32810, OnMenuitem32810)
	ON_COMMAND(ID_MENUITEM32811, OnMenuitem32811)
	ON_COMMAND(ID_MENUITEM32812, OnMenuitem32812)
	ON_COMMAND(ID_MENUITEM32813, OnMenuitem32813)
	ON_COMMAND(ID_MENUITEM32814, OnMenuitem32814)
	ON_COMMAND(ID_MENUITEM32815, OnMenuitem32815)
	ON_COMMAND(ID_INNERPT, OnInnerpt)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	ON_COMMAND_RANGE(ID_CREATE_LINE, 
					ID_INSERT_COMB, OnCreateEntity)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CREATE_LINE, 
					ID_INSERT_COMB, OnUpdateCreateCommand)
	ON_COMMAND_RANGE(ID_MODIFY_MOVE, 
					ID_MODIFY_ERASE, OnModifyEntity)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MODIFY_MOVE, 
					ID_MODIFY_ERASE, OnUpdateModifyCommand)
	//视图变换命令
	ON_COMMAND_RANGE(ID_VIEW_REGION,ID_VIEW_PAN, OnViewCommand)//

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCadView construction/destruction

CVCadView::CVCadView()
{
	// TODO: add construction code here
	g_pView = this;
	m_pCmd = NULL;
	// 初始化世界坐标与屏幕坐标的关系
	m_dOrgX = m_dOrgY = 0.;
	scale = 0.4;
	m_bIsPrinting = FALSE;

	m_bLBPressed = FALSE;
	m_bDragging = FALSE;
	//m_bEditable = TRUE;

	m_nStartOperateNum = 0;
	m_nOperations = 0;
	m_nCurrentOperation = 0;
}

CVCadView::~CVCadView()
{
	if( m_pCmd )
		delete m_pCmd ;
}

void CVCadView::SetStartOperateNum(int num)
{
	m_nStartOperateNum = num;
	m_nOperations = m_nCurrentOperation = num;
}

BOOL CVCadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVCadView drawing

void CVCadView::OnDraw(CDC* pDC)
{
	g_nRefresh ++; // 每次视窗被重新绘制时，刷新次数加 1
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPen	pen; 
	pen.CreatePen(PS_SOLID, 1, RGB(200,200,200)) ;
	CPen *	pOldPen = pDC->SelectObject(&pen); // 得到原来的画笔

	int i;
	for( i=0; i<150*scale + 1; i++) {
		pDC->MoveTo(0, i*10/scale);
		pDC->LineTo(2000,i*10/scale);
	}
	for(i=0; i<250*scale +1; i++) {
		pDC->MoveTo(i*10/scale, 0);
		pDC->LineTo(i*10/scale, 1200);
	}
	pDC->SelectObject(pOldPen); // 恢复原来的画笔 
	pen.DeleteObject();
	DrawCoord(pDC) ;

	// TODO: add draw code for native data here
	pDoc->Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CVCadView diagnostics

#ifdef _DEBUG
void CVCadView::AssertValid() const
{
	CView::AssertValid();
}

void CVCadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CVCadDoc* CVCadView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVCadDoc)));
	return (CVCadDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVCadView message handlers

void CVCadView::OnCreateEntity(UINT m_nID)
{

	// 如果当前状态存在命令，那么
	// 1. 取消该命令的操作
	// 2. 删除该命令对象
	// 3. 将命令指针设为空
	if( m_pCmd ){ 
		m_pCmd->Cancel();
		delete m_pCmd ;
		m_pCmd = NULL; 
	}
	// 下面根据不同的菜单命令创建不同的命令对象
	switch(m_nID)
	{
		case ID_CREATE_LINE: // 直线
		{
			m_pCmd = new CCreateLine();
			break;
		}
		case ID_CREATE_RECTANGLE: // 矩形
		{
			m_pCmd = new CCreateRect();
			break;
		}
		case ID_CREATE_CIRCLE: // 圆
		{
			m_pCmd = new CCreateCircle();
			break;
		}
		case ID_CREATE_ARC: // 圆弧
		{
			m_pCmd = new CCreateArc();
			break;
		}
		case ID_CREATE_ELLIPSE: //椭圆
		{
			m_pCmd = new CCreateEllipse();
			break;
		}
		case ID_CREATE_TEXT: // 文本
		{
			m_pCmd = new CCreateText();
			break;
		}
		case ID_CREATE_POLYGON: // 多边形
		{
			m_pCmd = new CCreatePolygon();
			break;
		}
		case ID_CREATE_BLOCK:
		{
			m_pCmd = new CCreateBlock();
			break;
		}
		case ID_CREATE_CONNECT:
		{
			m_pCmd = new CCreateConnect();
			break;
		}
		case 32810:
		{
			m_pCmd = new CCreateFitCurve(32810);//最小二乘法
			break;
		}
		case 32811:
		{
			m_pCmd = new CCreateFitCurve(32811);//Bezier
			break;
		}
		case 32812:
		{
			m_pCmd = new CCreateFitCurve(32812);//BSpline
			break;
		}
		case 32815:
		{
			m_pCmd = new CCreateFitCurve(32815);//BSpline
			break;
		}
		case 32816:
		{
			m_pCmd = new CCreateFitCurve(32816);//识别封闭区域
			break;
		}
		case ID_CREATE_COMB:
		{
			m_pCmd = new CCreateComb();
			break;
		}
		case ID_INSERT_COMB:
		{
			m_pCmd = new CInsertComb();
			break;
		}
	}
}

void CVCadView::OnUpdateCreateCommand(CCmdUI* pCmdUI)
{
	int flag = 0 ;
	switch(pCmdUI->m_nID)
	{
		case ID_CREATE_LINE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateLine) )
				flag = 1;
			break;
		}
		case ID_CREATE_RECTANGLE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateRectangle) )
				flag = 1;
			break;
		}
		case ID_CREATE_CIRCLE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateCircle) )
				flag = 1;
			break;
		}
		case ID_CREATE_ARC:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateArc) )
				flag = 1;
			break;
		}
		case ID_CREATE_ELLIPSE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateEllipse) )
				flag = 1;
			break;
		}
		case ID_CREATE_TEXT:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateText) )
				flag = 1;
			break;
		}
		case ID_CREATE_POLYGON:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreatePolygon) )
				flag = 1;
			break;
		}
		case ID_CREATE_BLOCK:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateBlock) )
				flag = 1;
			break;
		}
		case ID_CREATE_CONNECT:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctCreateConnect) )
				flag = 1;
			break;
		}
		case ID_CREATE_COMB:
		{
			if ((m_pCmd != NULL && m_pCmd->GetType() == ctCreateComb))
				flag = 1;
			break;
		}
		case ID_INSERT_COMB:
		{
			if ((m_pCmd != NULL && m_pCmd->GetType() == ctInsertComb))
				flag = 1;
			break;
		}
		default:
			break;
	}
	pCmdUI->SetCheck(flag);
}

void CVCadView::OnModifyEntity(UINT m_nID)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	// 如果当前状态存在命令，那么
	// 1. 取消该命令的操作
	// 2. 删除该命令对象
	// 3. 将命令指针设为空
	if( m_pCmd ){ 
		m_pCmd->Cancel();
		delete m_pCmd ;
		m_pCmd = NULL; 
	}

	// 判断是否有实体图元被选中
	if(pDoc->m_selectArray.GetSize() == 0)
	{
		CString	strError = _T("请首先选取图元");	
		AfxMessageBox(strError);
		return;
	}
	// 下面根据不同的菜单命令创建不同的命令对象
	switch(m_nID)
	{
		case ID_MODIFY_MOVE: 
		{// 平移
			m_pCmd = new CMove();
			break;
		}
		case ID_MODIFY_ROTATE: 
		{// 旋转
			m_pCmd = new CRotate();
			break;
		}
		case ID_MODIFY_MIRROR: 
		{// 镜像
			m_pCmd = new CMirror();
			break;
		}
		default:
			//删除
			Erase() ;
			break;
	}
}
void CVCadView::OnUpdateModifyCommand(CCmdUI* pCmdUI)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	// 判断是否有实体图元被选中
	if(pDoc->m_selectArray.GetSize() == 0)
	{
		pCmdUI->Enable(FALSE);		
		return;
	}

	int flag = 0 ;
	switch(pCmdUI->m_nID)
	{
		case ID_MODIFY_MOVE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctMove) )
				flag = 1;
			break;
		}
		case ID_MODIFY_ROTATE:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctRotate) )
				flag = 1;
			break;
		}
		case ID_MODIFY_MIRROR:
		{
			if( (m_pCmd != NULL && m_pCmd->GetType() == ctMirror) )
				flag = 1;
			break;
		}
		default:
			break;
	}
	pCmdUI->SetCheck(flag);
}

void CVCadView::WorldtoScreen(const Position& pos, CPoint& screenPt)
{
	// 获取当前客户区的大小
	CRect rect ;
	GetClientRect(&rect) ;
	// 将屏幕原点设置为客户区的中心
	int	nSOrgX = int((rect.left + rect.right) / 2 );///scale);
	int nSOrgY = int((rect.top + rect.bottom) / 2 );///scale);
	// 计算屏幕坐标值
	screenPt.x = (int)((pos.x - m_dOrgX) / scale + nSOrgX) ;
	screenPt.y = (int)(nSOrgY - (pos.y - m_dOrgY) / scale ) ;
	CDC* pDC = GetDC();
	
	if(m_bIsPrinting)
		pDC->SetMapMode(MM_LOENGLISH); 
	else
		pDC->SetMapMode(MM_TEXT); 
	pDC->LPtoDP(&screenPt);
	ReleaseDC(pDC);	
}

void CVCadView::ScreentoWorld(const CPoint& pt, Position& pos)
{
	CPoint screenPt = pt;
	CDC* pDC = GetDC();
	pDC->SetMapMode(MM_TEXT);

	pDC->DPtoLP(&screenPt);
	ReleaseDC(pDC);	
	// 获取当前客户区的大小
	CRect rect ;
	GetClientRect(&rect) ;
	// 将屏幕原点设置为客户区的中心
	int	nSOrgX = (rect.left + rect.right) / 2 ;
	int nSOrgY = (rect.top + rect.bottom) / 2 ;
	// 计算世界坐标值
	pos.x = (screenPt.x - nSOrgX) * scale + m_dOrgX;
	pos.y = (nSOrgY - screenPt.y ) * scale + m_dOrgY;
}

double CVCadView::GetScale()
{
	return scale;
}

void CVCadView::SetScale(double dScale)
{
	scale = dScale;
}

void CVCadView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//if(!m_bEditable)
	//	return;

	CDC * pDC = GetDC();
	m_basePos = m_desPos = point;

	// TODO: Add your message handler code here and/or call default
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	Position pos;
	ScreentoWorld(point, pos); // 将设备坐标转换为世界坐标

	if(m_pCmd) {
		m_pCmd->OnLButtonDown(nFlags, pos);
	}
	else
	{	
		//缺省移动操作
		if( (! (nFlags & MK_CONTROL)) && pDoc->m_selectArray.GetSize() >= 1)
			m_bLBPressed = TRUE;
		else //选择
			pDoc->OnLButtonDown(nFlags, pos);		
	}

	ReleaseDC(pDC);
	CView::OnLButtonDown(nFlags, point);
}

void CVCadView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//if(!m_bEditable)
	//	return;

	// TODO: Add your message handler code here and/or call default
	Position pos;
	// 将设备坐标转换为世界坐标
	ScreentoWorld(point, pos);

	if(m_pCmd)
		m_pCmd->OnLButtonDblClk(nFlags, pos);
	else {
		//获得文档类的指针
		CVCadDoc * pDoc = GetDocument() ;
		ASSERT_VALID(pDoc) ;
		
		pDoc->OnLButtonDown(nFlags, pos);
		
		MEntity* pSelEntity = NULL ;
		POSITION pos = NULL ;
		if(pDoc->m_selectArray.GetSize() == 1) {
			pSelEntity = (MEntity*)pDoc->m_selectArray[0] ;
			if(pSelEntity->GetType() == etText) {
				CTextInputDlg dlg;
				CString oldText = ((MText*)pSelEntity)->GetText();
				dlg.m_text = oldText;
				dlg.DoModal();
				
				((MText*)pSelEntity)->SetText(dlg.m_text);
			}
		}
		//清空选择集
		pDoc->m_selectArray.RemoveAll() ;
		//更新文档，重新绘制
		pDoc->UpdateAllViews(NULL) ;

	}
	
	CView::OnLButtonDblClk(nFlags, point);
}

void CVCadView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//if(!m_bEditable)
	//	return;

	// TODO: Add your message handler code here and/or call default
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	Position pos;
	ScreentoWorld(point, pos);// 将设备坐标转换为世界坐标

	//获得状态条的指针
	CStatusBar* pStatus=(CStatusBar*)
		AfxGetApp()->m_pMainWnd->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	if(pStatus)
	{
		char tbuf[40];
		sprintf(tbuf,"(%8.3f,%8.3f)",pos.x, pos.y);
		//在状态条的第二个窗格中输出当前鼠标的位置
		pStatus->SetPaneText(1,tbuf);
	}
	if(m_pCmd)
		m_pCmd->OnMouseMove(nFlags, pos);
	else {
		if(!m_bDragging)
			pDoc->OnMouseMove(nFlags, pos);
	}

	if(m_bLBPressed) {//拖动实体
			Position	prePos, curPos, basePos;
			ScreentoWorld(m_basePos, basePos);
			ScreentoWorld(m_desPos, prePos); // 获得上一个目标位置
			curPos = pos; // 得到当前位置
			

			CDC*	pDC = g_pView->GetDC(); 
			// 根据当前位置给出选中图元的实时位置
			int i, n;
			for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
			{
				MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
				pEntity->Draw(pDC,dmSelect);
				// 如果在操作过程中窗口没有被刷新，则要清除上一个位置上绘制的图元
				if(1) 
				{
					MEntity*	pCopyEntity1 = pEntity->Copy(); // 得到图元的拷贝
					pCopyEntity1->Move(basePos, prePos, TRUE); // 将拷贝移动到上一个位置
					pCopyEntity1->Draw(pDC,dmDrag); // 对在上一个位置对拷贝进行重画
					delete pCopyEntity1; // 删除临时拷贝
				}
				// 在当前位置上绘制图元
				MEntity*	pCopyEntity2 = pEntity->Copy();// 得到图元的拷贝
				pCopyEntity2->Move(basePos, curPos, TRUE);// 将拷贝移动到当前位置
				pCopyEntity2->Draw(pDC,dmDrag);	// 对当前位置绘制拷贝
				delete pCopyEntity2; // 删除临时拷贝 
			}
			g_pView->ReleaseDC(pDC);
			m_desPos = point; // 将目标设置为当前位置

			if(pos.Distance(prePos) > 0.5)
				m_bDragging = TRUE;
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CVCadView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//if(!m_bEditable)
	//	return;

	// TODO: Add your message handler code here and/or call default
	Position pos;
	// 将设备坐标转换为世界坐标
	ScreentoWorld(point, pos);

	if(m_pCmd)
		m_pCmd->OnRButtonDown(nFlags, pos);

	CView::OnRButtonDown(nFlags, point);
}

void CVCadView::OnPick() 
{
	// TODO: Add your command handler code here
	if(m_pCmd){

		delete m_pCmd;
		m_pCmd = NULL;
	}
}

void CVCadView::OnUpdatePick(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_pCmd == NULL ? 1 : 0) ;
}
/////////////////////////////////////////////////////////////////////////////
// CVCadView printing

BOOL CVCadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 将绘图板设置为2个标准页大小
	pInfo->SetMaxPage(2);
	return DoPreparePrinting(pInfo);
}

void CVCadView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	// 获得可打印区域（毫米值）
	int nHorzSize = pDC->GetDeviceCaps(HORZSIZE);
	int nVertSize = pDC->GetDeviceCaps(VERTSIZE);
	// 计算页面的高度和宽度（逻辑单位）  
	m_nPageWidth = (int)((double)nHorzSize / 25.4 * 100.0);
	m_nPageHeight = (int)((double)nVertSize / 25.4 * 100.0);
}

void CVCadView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CVCadView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	pDC->SetMapMode(MM_TEXT); 
	if(pDC->IsPrinting())
	{
		pDC->SetMapMode(MM_LOENGLISH); 
		int nPages = pInfo->m_nCurPage - 1;
		int x = (nPages & 1) * m_nPageWidth;
		int y = (nPages / 2) * m_nPageHeight;
		pDC->SetWindowOrg(x,y);
	}	
	CView::OnPrepareDC(pDC, pInfo);

}
void CVCadView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	ASSERT_VALID(pDC);
	pDC->SetWindowOrg(pInfo->m_rectDraw.left,
		-pInfo->m_rectDraw.top);
	
	m_bIsPrinting=TRUE;	
	// 打印页眉
	PrintPageHeader(pDC,pInfo); 
	// 打印图形
	CView::OnPrint(pDC, pInfo); 
	// 打印页脚
	PrintPageFooter(pDC,pInfo);

	m_bIsPrinting=FALSE;
}


void CVCadView::PrintPageHeader(CDC *pDC, CPrintInfo *pInfo)
{
	// 获取设备描述表中缺省的字体
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	// 创建新字体并将其选入设备描述表
	CFont newFont;
	newFont.CreateFont(-tm.tmHeight,
		0,0,0,
		FW_NORMAL,
		FALSE, FALSE,
		0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN,
		"Courier New");
	CFont* pOldFont = (CFont*)pDC->SelectObject(&newFont);

	// 页眉中的文本右对齐
	pDC->SetTextAlign(TA_RIGHT);
	// 打印页眉文本
	pDC->TextOut(pInfo->m_rectDraw.right, -25, "VCad 绘图系统");

	// 打印页眉中的水平直线
	int y = - 35 - tm.tmHeight;
	pDC->MoveTo(0,y);
	pDC->LineTo(pInfo->m_rectDraw.right,y);

	// 根据页眉占用的空间来调整可打印图形区域
	y -= 25;
	pInfo->m_rectDraw.top += y;

	// 恢复以前的字体
	pDC->SelectObject(pOldFont);
}
// 打印页脚
void CVCadView::PrintPageFooter(CDC *pDC, CPrintInfo *pInfo)
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// 由文档保存的位置或者标题和当前的页码构成页脚的文本
	// 如果文档没有被保存，则打印文档的标题名称；反之，则打印文档保存的位置
	CString strFooter = pDoc->GetPathName();
	CString strTitle = pDoc->GetTitle();
	if(strTitle == "Untitled" || strTitle == "无标题")
		strFooter = strTitle;
	
	// 文本左对齐
	pDC->SetTextAlign(TA_LEFT);
	// 打印文本
	pDC->TextOut(pInfo->m_rectDraw.left,
		pInfo->m_rectDraw.bottom + 100, strFooter);
	
	// 在页脚中添加页码信息
	CString strPage;
	strPage.Format("%s%d%s", "第", pInfo->m_nCurPage, "页");
	// 获取页码的宽度
	CSize size = pDC->GetTextExtent(strFooter);
	// 页码右对齐
	pDC->TextOut(pInfo->m_rectDraw.right - size.cx,
		pInfo->m_rectDraw.bottom + 100, strPage);

	// 绘制页脚的水平上划线
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int y = pInfo->m_rectDraw.bottom + 90 + tm.tmHeight;
	pDC->MoveTo(0, y);
	pDC->LineTo(pInfo->m_rectDraw.right, y);
}


void CVCadView::OnRedraw() 
{
	// TODO: Add your command handler code here
	g_pDoc ->UpdateAllViews(NULL) ;
}
//
void CVCadView::OnViewCommand(UINT m_nID) 
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT(pDoc) ;

	CRect rc ;
	GetClientRect(&rc) ;

	if( m_pCmd ){
		delete m_pCmd ;
		// 切记：在使用一个命令之前必须要清除上一个命令
		m_pCmd = NULL; 
	}
	switch(m_nID)
	{
		case ID_VIEW_REGION:
		{
			if( !(m_pCmd = new CZoomRgnCmd()) )
				AfxMessageBox("Failed!");
//			pDoc->SetModifiedFlag(TRUE) ;
			::Prompt("请输入移动的起始点：") ;
			break;
		}
		case ID_VIEW_ZOOMIN:
		{
			scale *= 0.618 ;
			if(scale<1e-6) scale = 1. ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;
		}
		case ID_VIEW_ZOOMOUT:
		{
			scale/=0.618 ;
			if(scale>1e+6) scale = 1. ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;	
		}
		case ID_VIEW_ZOOMALL:
		{
			ZoomAll() ;
			break;
		}
		case ID_VIEW_PAN:
		{
			if(!(m_pCmd = new CViewPanCmd()))
				AfxMessageBox("Failed!") ;
//			pDoc->SetModifiedFlag(TRUE) ;
			break;
		}
		default:
			break;
	}
	InvalidateRect(rc) ;
}
//
void CVCadView::ZoomAll()
{
	CVCadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CDC* pDC = GetDC() ;
	//映射模式要一致
	pDC->SetMapMode(MM_TEXT) ;

	CRect rc ;
	GetClientRect(&rc) ;
	pDC->DPtoLP(&rc) ;
	
	double MinX, MaxX, MinY, MaxY ;
	MinX = 1.e+6 ;
	MinY = 1.e+6 ;
	MaxX = - 1.e+6 ;
	MaxY = - 1.e+6 ;

	BOX2D pBox ;
	POSITION pos;
	pos = pDoc->m_EntityList.GetHeadPosition();
	// 如果链表中不存在任何图元，则不做任何操作直接返回
	if(pos == NULL)  
		return ;
	// 通过各图元的包围盒来计算出所有图元的包围盒
	while(pos != NULL)
	{
		MEntity* pEntity = (MEntity*)(pDoc->m_EntityList.GetNext(pos)) ;
		pEntity->GetBox(&pBox) ;
		if(MinX>pBox.min[0]) MinX = pBox.min[0] ;
		if(MinY>pBox.min[1]) MinY = pBox.min[1] ;
		if(MaxX<pBox.max[0]) MaxX = pBox.max[0] ;
		if(MaxY<pBox.max[1]) MaxY = pBox.max[1] ;
	}
	//重新计算原点
	m_dOrgX = (MaxX + MinX)/2. ;
	m_dOrgY = (MaxY + MinY)/2. ;
	//重新计算比例因子
	double scalex = fabs((MaxX - MinX)/(rc.right-rc.left)) ;
	double scaley = fabs((MaxY - MinY)/(rc.bottom-rc.top )) ;
	if(scalex>scaley)
		scale = scalex ;
	else
		scale = scaley ;

	if(fabs(scale)<1e-6||fabs(scale)>1e+6)
		scale = 1. ;
	InvalidateRect(rc) ;
}

void CVCadView::DrawCoord(CDC *pDC)
{
	CVCadDoc * pDoc = GetDocument() ;
	ASSERT_VALID(pDoc) ;

	CPoint sOrgp, sXp, sYp;
	Position dOrgPnt(0,0) ;

	WorldtoScreen(dOrgPnt,sOrgp) ;
	pDC->TextOut(sOrgp.x-20,sOrgp.y-2,"O") ;

	Position dtemppx(50,0) ;
	WorldtoScreen(dtemppx,sXp) ;
	pDC->TextOut(sXp.x+2,sXp.y-2,"X") ;

	Position dtemppy(0,50) ;
	WorldtoScreen(dtemppy,sYp) ;
	pDC->TextOut(sYp.x-20,sYp.y-20,"Y") ;

	pDC->MoveTo(sOrgp) ;
	pDC->LineTo(sXp) ;
	pDC->MoveTo(sOrgp) ;
	pDC->LineTo(sYp) ;

	//画箭头
	CBrush * pnew = new CBrush;
	pnew->CreateSysColorBrush(BLACK_BRUSH);
	CBrush* pbrsh = (CBrush*) pDC->SelectObject(pnew);
	POINT* pts = new POINT[3];
	pts[0] = sXp;
	pts[1] = sXp; pts[1].x -= 5 / scale; pts[1].y -= 3 / scale;
	pts[2] = sXp; pts[2].x -= 5 / scale; pts[2].y += 3 / scale;
	pDC->Polygon(pts, 3);

	pts[0] = sYp;
	pts[1] = sYp; pts[1].x -= 3 / scale; pts[1].y += 5 / scale;
	pts[2] = sYp; pts[2].x += 3 / scale; pts[2].y += 5 / scale;
	pDC->Polygon(pts, 3);
	pDC->SelectObject(pbrsh);

	delete pts;
	delete pnew;
}

void CVCadView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_NEXT:
		scale *= 0.618 ;
		if(scale<1e-6) scale = 1. ;
//		g_pDoc->SetModifiedFlag(TRUE) ;
		Invalidate(TRUE) ;
		break;
	case VK_PRIOR:
		scale /= 0.618 ;
		if(scale>1e+6) scale = 1. ;
//		g_pDoc->SetModifiedFlag(TRUE) ;
		Invalidate(TRUE) ;
		break;
	default:;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
//临时删除功能
void CVCadView::Erase()
{
	//获得文档类的指针
	CVCadDoc * pDoc = GetDocument() ;
	ASSERT_VALID(pDoc) ;
	
	MEntity* pSelEntity = NULL ;
	POSITION pos = NULL ;
	//在链表中查找被添加到选择集中图元
	//并将其从链表中永远删除
	if(pDoc->m_selectArray.GetSize() > 0)
	{
		g_pDoc->SetModifiedFlag(TRUE);
		m_nCurrentOperation++;
		m_nOperations = m_nCurrentOperation;
		g_pDoc->m_clpArray.RemoveAll();
	}
	
	for( int i = 0 ; i < pDoc->m_selectArray.GetSize() ; i++ )
	{
		pSelEntity = (MEntity*)pDoc->m_selectArray[i] ;
//		pos = pDoc->m_EntityList.Find(pSelEntity) ;
//		pDoc->m_EntityList.RemoveAt(pos) ;
//		delete pSelEntity ;
//		pSelEntity = NULL ;
		//pDoc->SetModifiedFlag(TRUE);
			MEntity *   pShowEntity = pSelEntity->GetCurrentEnt();
			if(pShowEntity != NULL) {
				MEntity * pNext = pShowEntity->next;
				while(pNext) {
					MEntity * pTmp = pNext;
					pNext = pNext->next;
					delete pTmp;
				}
				pShowEntity->next = NULL;
			}

		MEntity* pEraseEnt = pShowEntity->Erase();
//		pEraseEnt->m_bErased = TRUE;
		pShowEntity->next = pEraseEnt;
		pEraseEnt->m_nOperationNum = m_nCurrentOperation;
	}

	//清空选择集
	pDoc->m_selectArray.RemoveAll() ;
	//更新文档，重新绘制
	pDoc->UpdateAllViews(NULL) ;
}

void CVCadView::OnUpdateModifyErase(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	OnUpdateEditCopy(pCmdUI);
}

//	打开文件	
void CVCadView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CVCadDoc* pDocument = GetDocument();

	BOOL	ret = FALSE;

	//	如果当前文件为保存则先保存
	int	status = 0;
	ret = pDocument->IsModified();
	if(ret == TRUE) 
	{
	    status = AfxMessageBox(" 文件已修改,是否保存文件?",MB_YESNOCANCEL);
	    if(status == IDCANCEL) 
		{
			return;
	    }
	    if(status == IDYES) 
		{
			AfxGetApp()->m_pMainWnd->SendMessage(WM_COMMAND,ID_FILE_SAVE);
	    }
	}

	extern BOOL GetCADName(BOOL bIsOpen,CString& sFileName);	//	得到CAD文件名
	CString sFileName;
    if (GetCADName(TRUE,sFileName))
	{
		pDocument->DeleteContents();
//		pDocument->SetModifiedFlag();

		CFileException fe;
		CFile* pFile = pDocument->GetFile(sFileName,
			CFile::modeRead|CFile::shareDenyWrite, &fe);
		if (pFile == NULL)
		{
			pDocument->ReportSaveLoadException(sFileName, &fe,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}

		CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
		loadArchive.m_pDocument = pDocument;
		loadArchive.m_bForceFlat = TRUE;

		//	读取文件
		if (pFile->GetLength() != 0)
		{
			GetDocument()->BeginWaitCursor();
			pDocument->Serialize(loadArchive);
			GetDocument()->EndWaitCursor();
		}

		pDocument->SetTitle(sFileName);	//	设置窗口标题
		AfxGetApp()->AddToRecentFileList(sFileName);	//	添加最新文件列表

		loadArchive.Close();
		pDocument->ReleaseFile(pFile, FALSE);

		pDocument->SetModifiedFlag(FALSE);

		Invalidate();	//	刷新绘图区
	}
}

//	保存文件
void CVCadView::OnFileSave() 
{
	// TODO: Add your command handler code here
	CVCadDoc* pDoc = GetDocument();
	CFileException fe;
	
	//	如果文件未命名，则调“另存为”函数	
	CString sFileName=pDoc->GetTitle();
	sFileName.MakeLower();
	if(sFileName=="untitled" || sFileName == "无标题")
	{
		OnFileSaveAs();
		return;
	}

	//	创建文件
	CFile* pFile = new CFile(sFileName, CFile::modeCreate |
		CFile::modeWrite | CFile::shareExclusive);//, &fe);

	if (pFile == NULL)
		return;

	CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	saveArchive.m_pDocument = pDoc;
	saveArchive.m_bForceFlat = TRUE;

	//	保存文件
	pDoc->Serialize(saveArchive);	

	//设置文档更新标记
	pDoc->SetModifiedFlag(FALSE);

	saveArchive.Close();
	pDoc->ReleaseFile(pFile, FALSE);

}

//	另存文件
void CVCadView::OnFileSaveAs() 
{
	CVCadDoc* pDoc = GetDocument();

	// TODO: Add your command handler code here
	extern BOOL GetCADName(BOOL bIsOpen,CString& sFileName);	//	得到CAD文件名
	CString sFileName;
    if (GetCADName(FALSE,sFileName))
	{

		CFileException fe;
		CFile* pFile = new CFile(sFileName, CFile::modeCreate |
			CFile::modeWrite | CFile::shareExclusive);//, &fe);

		if (pFile == NULL)
			return;

		CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
		saveArchive.m_pDocument = pDoc;
		saveArchive.m_bForceFlat = TRUE;
		TRY
		{
			pDoc->Serialize(saveArchive);    

			pDoc->SetTitle(sFileName);
			AfxGetApp()->AddToRecentFileList(sFileName);

			saveArchive.Close();
			pDoc->ReleaseFile(pFile, FALSE);
		}
		CATCH_ALL(e)
		{
			pDoc->ReleaseFile(pFile, TRUE);

			TRY
			{
				pDoc->ReportSaveLoadException(sFileName, e,
					TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
			}
			END_TRY
			e->Delete();
			return;
		}
		END_CATCH_ALL

		pDoc->SetModifiedFlag(FALSE);    

    }

}

void CVCadView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//if(!m_bEditable)
	//	return;

	// TODO: Add your message handler code here and/or call default
	m_desPos = point;
	Position basePos, desPos;
	ScreentoWorld(m_basePos, basePos);
	ScreentoWorld(m_desPos, desPos);
	CDC*	pDC = g_pView->GetDC(); // 获得视类的设备环境指针

	// 将选择集中的图元移动到目标位置并进行绘制
	int i, n;
	if(m_bDragging) {
		for(n = g_pDoc->m_selectArray.GetSize(), i = 0; i < n; i++)
		{
			MEntity*	pEntity = (MEntity*)g_pDoc->m_selectArray[i];
			pEntity->Draw(pDC,dmInvalid); // 清除原来位置上的图元
			pEntity = pEntity->GetShowEnt();
			BOX2D box;
			pEntity->GetBox(&box);
			CPoint p1, p2;
			WorldtoScreen(box.min, p1);
			WorldtoScreen(box.max, p2);
			CRect rect(p1, p2);
			InvalidateRect(rect, 1);
//			pEntity->Move(basePos, desPos); // 将图元移动到目标位置
//			pEntity->Draw(pDC,dmNormal); // 在目标位置上绘制图元

			MEntity * pNewEnt = pEntity->Copy();
			pNewEnt->Move(basePos, desPos); // 将图元移动到目标位置
			pNewEnt->Draw(pDC,dmNormal); // 在目标位置上绘制图元
			g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
			pNewEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
			pEntity->Append(pNewEnt);
		}
		g_pDoc->m_selectArray.RemoveAll(); //  清空选择集
//		g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
		g_pView->ReleaseDC(pDC); // 释放视类的设备环境指针
	}

	m_bDragging  = FALSE;
	m_bLBPressed = FALSE;
	//Invalidate(1);
	CView::OnLButtonUp(nFlags, point);
}

void CVCadView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		g_pDoc->m_clpArray.Add(pSelEntity);
	}
}

void CVCadView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_selectArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);		
}

void CVCadView::OnEditCut() 
{
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		g_pDoc->m_clpArray.Add(pSelEntity);

			MEntity *   pShowEntity = pSelEntity->GetCurrentEnt();
			if(pShowEntity != NULL) {
				MEntity * pNext = pShowEntity->next;
				while(pNext) {
					MEntity * pTmp = pNext;
					pNext = pNext->next;
					delete pTmp;
				}
				pShowEntity->next = NULL;
			}

		MEntity* pEraseEnt = pShowEntity->Erase();
		pShowEntity->next = pEraseEnt;
		pEraseEnt->m_nOperationNum = m_nCurrentOperation + 1;
	}

	m_nCurrentOperation++;
	m_nOperations = m_nCurrentOperation;
	g_pDoc->m_selectArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_selectArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);	
}

void CVCadView::OnEditPaste() 
{
	CDC *pDC = GetDC();
	// TODO: Add your command handler code here
	for( int i = 0 ; i < g_pDoc->m_clpArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_clpArray[i] ;
		MEntity *   pCopyEntity = pSelEntity->GetCurrentEnt();
		ASSERT_VALID(pCopyEntity);
		MEntity * pNew = pCopyEntity->Copy();
		pNew->Move(Position(0,0), Position(10,10));
		pNew->Draw(pDC, dmNormal);
		g_pDoc->m_EntityList.AddTail(pNew); // 将指针添加到图元链表
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNew->m_nOperationNum = g_pView->m_nCurrentOperation;
	}

	ReleaseDC(pDC);
}

void CVCadView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(g_pDoc->m_clpArray.GetSize() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CVCadView::OnEditRedo() 
{
	// TODO: Add your command handler code here
	m_nCurrentOperation++;
	g_pDoc->m_selectArray.RemoveAll();
	g_pDoc->m_clpArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nOperations > m_nCurrentOperation)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);		
}

void CVCadView::OnEditUndo() 
{
	// TODO: Add your command handler code here
	m_nCurrentOperation--;
	g_pDoc->m_selectArray.RemoveAll();
	g_pDoc->m_clpArray.RemoveAll();
	Invalidate(TRUE);
}

void CVCadView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_nCurrentOperation > m_nStartOperateNum)
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}


void CVCadView::OnEditManhadun() //曼哈顿化
{
	CDC * pDC = GetDC();
	//算法：从起点开始，如果&x > &y则沿着X走；如果方向不同则不添加；如果两点同X或同Y则改变上一点的Y或X;
	//      添加终点，算法结束。
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		if(pSelEntity->GetType() == etConnect)
		{
			pSelEntity->Draw(pDC, dmInvalid);
			MEntity * pManhadun = pSelEntity->Copy();
			((MConnect*)pManhadun)->Manhadunize();
			pManhadun->Draw(pDC,dmNormal); // 在目标位置上绘制图元
			g_pDoc->SetModifiedFlag(TRUE); // 标志文档数据已被修改
			pManhadun->m_nOperationNum = g_pView->m_nCurrentOperation;
			pSelEntity->Append(pManhadun);
		}
	}
	ReleaseDC(pDC);
	g_pDoc->m_selectArray.RemoveAll();
	Invalidate(FALSE);
}

void CVCadView::OnUpdateEditManhadun(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	//如果选中的实体集中有连接，则有效；否则无效
	BOOL bValid = FALSE;
	for( int i = 0 ; i < g_pDoc->m_selectArray.GetSize() ; i++ ){
		MEntity*	pSelEntity = (MEntity*)g_pDoc->m_selectArray[i] ;
		if(pSelEntity->GetType() == etConnect)
		{
			bValid = TRUE;
			break;
		}
	}

	pCmdUI->Enable(bValid);
}


void CVCadView::AddEntity(MEntity* pEnt)
{
	g_pDoc->m_EntityList.AddTail(pEnt); 
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
	pEnt->m_nOperationNum = g_pView->m_nCurrentOperation;
}

void CVCadView::OnMenuitem32807() 
{
	// TODO: Add your command handler code here
	CECT2DModelDlg dlg(this);
	
	if(dlg.DoModal() == IDCANCEL)
	{ 
		return ;
	}
}

void CVCadView::OnMenuitem32808() 
{
	// TODO: Add your command handler code here
	//齿宽系数数表的程序化
	CGearParamDlg dlg;
	dlg.DoModal();
}




void CVCadView::OnMenuitem32809() 
{
	// 双线性插值
	CInterDlg dlg;

	dlg.DoModal();	
}

void CVCadView::OnMenuitem32810() 
{
	//最小二乘
	OnCreateEntity(32810);
}

void CVCadView::OnMenuitem32811() 
{
	//Bezier
	OnCreateEntity(32811);
}

void CVCadView::OnMenuitem32812() 
{
	//Bspline
	OnCreateEntity(32812);
}


extern void bezsurf(double b[], int n, int m, int p1, int p2, double q[]);
extern void rbspsurf(double b[], int k, int l, int npts, int mpts, int p1, int p2, double q[]);

//draw bezier surface
void CVCadView::OnMenuitem32813() 
{
	// TODO: Add your command handler code here
	int i,j;
    int n = 3;    //the order of u-direction
	int m = 4;    //the order of w-direction
    int p1 = 17;  //the subdivision number of u-direction
	int p2 = 21;  //the subdivision number of w-direction
	double q[1072];   //21*17*3+1
	//b[(4+1)*(3+1)+1]
/*	double b[61]={0, -300,200,0,    -150,200,200,   0,200,-200,  150,200,200,   300,200,0,        //row-1: 5 points
	               	 -300,80,200,   -150,80,-200,   0,80,200,    150,80,-200,   300,80,200,       //row-2: 5 points
					 -300,-80,-200, -150,-80,200,   0,-80,-200,  150,-80,200,   300,-80,-200,     //row-3: 5 points
					 -300,-200,0,   -150,-200,-200, 0,-200,200,  150,-200,-200, 300,-200,0};      //row-4: 5 points
*/
	double b[61]={0, -300,200,0,    -150,200,200,   0,200,300,   150,200,200,   300,200,0,        //row-1: 5 points
	               	 -300,80,200,   -150,80,300,    0,80,400,    150,80,300,    300,80,200,       //row-2: 5 points
					 -300,-80,200,  -150,-80,300,   0,-80,400,   150,-80,300,   300,-80,200,     //row-3: 5 points
					 -300,-200,0,   -150,-200,200,  0,-200,300,  150,-200,200,  300,-200,0};      //row-4: 5 points

	//transform around z-axis, y-axis and transfer [x,y,z]=[450,350,0]
	Vector vec(0,0,0);
	for(i=0; i<20; i++)
	{
		vec.xyz(b[i*3+1],b[i*3+2],b[i*3+3]);
		vec.rotate(70,0,40);    //rotate 20 around x-axis,   rotate 30 around z-axis
		b[i*3+1] = vec.x()+450;
		b[i*3+2] = vec.y()+150;
	}	
	
	//draw control polygon
	CDC *pDC = GetDC();
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*15+1],b[j*15+2]);
		for(i = 1; i < 5; i++)
		   pDC->LineTo(b[j*15+i*3+1],b[j*15+i*3+2]);
	}
	for(j = 0; j < 5; j++)
	{
		pDC->MoveTo(b[j*3+1],b[j*3+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[i*15+j*3+1],b[i*15+j*3+2]);
	}

	//draw bezier surface  
    bezsurf(b, n, m, p1, p2, q);
	for(j = 0; j < p1; j++)
	{
		pDC->MoveTo(q[j*p2*3+1],q[j*p2*3+2]);
		for(i = 1; i < p2; i++)
		   pDC->LineTo(q[j*p2*3+i*3+1],q[j*p2*3+i*3+2]);
	}
	for(j = 0; j < p2; j++)
	{
		pDC->MoveTo(q[j*3+1],q[j*3+2]);
		for(i = 1; i < p1; i++)
		   pDC->LineTo(q[i*p2*3+j*3+1],q[i*p2*3+j*3+2]);
	}

	ReleaseDC(pDC);
}

//Bspline surface
void CVCadView::OnMenuitem32814() 
{
	int i,j;
	int npts,mpts;
	int k,l;
	int p1,p2;

	double b[65];
	double q[1072];   //21*17*3+1

	npts = 4;
	mpts = 4;
	k = 3;
	l = 3;
	p1 = 17;
	p2 = 21;

	for (i = 1; i <= mpts*npts; i++)
		b[i] = 0.;
	for (i = 1; i <= 3*p1*p2; i++)
		q[i] = 0.;

	//	This is a standard test control polygon from Ex. 6.1, p. 184
	//	Comment out to use file input <filename>.rnp. Data is in the
	//	form x=b[1], y=b[2], z=b[3], h=b[4], etc. All h are 1.0.
	//	Thus, this is a nonrational B-spline surface and the results
	//	should be the same as for tbsurf.c
	b[1] = -150.;	b[2] = 0.;	   b[3] = 150.;	   b[4] = 1;  	
	b[5] = -15.;	b[6] = 50.;    b[7] = 50.;     b[8] = 1; 
    b[9] = -150.;   b[10] = 50.;   b[11] = -50.;   b[12] = 1;
	b[13] = -150.;	b[14] = 0.;    b[15] = -150.;  b[16] = 1; 
	b[17] = -50.;	b[18] = 50.;   b[19] = 150.;   b[20] = 1; 
	b[21] = -50.;   b[22] = 100.;  b[23] = 50.;    b[24] = 1;
	b[25] = -50.;	b[26] = 100.;  b[27] = -50.;   b[28] = 1;
	b[29] = -50.;	b[30] = 50.;   b[31] = -150.;  b[32] = 1;
    b[33] = 50.;    b[34] = 50.;   b[35] = 150.;   b[36] = 1;
	b[37] = 50.; 	b[38] = 100.;  b[39] = 50.;    b[40] = 1;
	b[41] = 50.; 	b[42] = 100.;  b[43] = -50.;   b[44] = 1;
    b[45] = 50.;    b[46] = 0.;	   b[47] = -150.;  b[48] = 1;
	b[49] = 150.;	b[50] = 0.;    b[51] = 150.;   b[52] = 1; 
	b[53] = 150.;	b[54] = 50.;   b[55] = 50.;    b[56] = 1;
 	b[57] = 150.;	b[58] = 50.;   b[59] = -50.;   b[60] = 1;
	b[61] = 150.;	b[62] = 0.;	   b[63] = -150.;  b[64] = 1;

		//transform around z-axis, y-axis and transfer [x,y,z]=[300,200,0]
	Vector vec(0,0,0);
	for(i=0; i<16; i++)
	{
		vec.xyz(b[i*4+1],b[i*4+2],b[i*4+3]);
		vec.rotate(0,60,30);    //rotate 20 around x-axis,   rotate 30 around z-axis
		b[i*4+1] = vec.x()+300;
		b[i*4+2] = vec.y()+200;
	}	

	//draw control polygon
	CDC * pDC = GetDC();
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*16+1],b[j*16+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[j*16+i*4+1],b[j*16+i*4+2]);
	}
	for(j = 0; j < 4; j++)
	{
		pDC->MoveTo(b[j*4+1],b[j*4+2]);
		for(i = 1; i < 4; i++)
		   pDC->LineTo(b[i*16+j*4+1],b[i*16+j*4+2]);
	}
	//draw RBspline surface  
	rbspsurf(b,k,l,npts,mpts,p1,p2,q);
	for(j = 0; j < p1; j++)
	{
		pDC->MoveTo(q[j*p2*3+1],q[j*p2*3+2]);
		for(i = 1; i < p2; i++)
		   pDC->LineTo(q[j*p2*3+i*3+1],q[j*p2*3+i*3+2]);
	}
	for(j = 0; j < p2; j++)
	{
		pDC->MoveTo(q[j*3+1],q[j*3+2]);
		for(i = 1; i < p1; i++)
		   pDC->LineTo(q[i*p2*3+j*3+1],q[i*p2*3+j*3+2]);
	}

	ReleaseDC(pDC);
}

//B样条反算
void CVCadView::OnMenuitem32815() 
{
	// TODO: Add your command handler code here
	OnCreateEntity(32815);
}


void CVCadView::OnInnerpt() 
{
	OnCreateEntity(32816);
	//区域识别
}