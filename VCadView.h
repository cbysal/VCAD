// VCadView.h : interface of the CVCadView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCADVIEW_H__C161FBAC_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
#define AFX_VCADVIEW_H__C161FBAC_7612_11D4_BF0F_0000E8A36431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Position;

class CVCadView : public CView
{
private:
	BOOL     m_bLBPressed;
	BOOL	 m_bDragging;
	CPoint	 m_basePos;
	CPoint   m_desPos;

	int		m_nStartOperateNum;
	
protected: // create from serialization only
	CVCadView();
	DECLARE_DYNCREATE(CVCadView)

// Attributes
public:
	//BOOL	  m_bEditable;
	int		  m_nOperations;
	int		  m_nCurrentOperation;
	CVCadDoc* GetDocument();
	void	  SetStartOperateNum(int num = 0);
	void      AddEntity(MEntity *);
public:	
	// 世界坐标与屏幕坐标的关系
	double	m_dOrgX, m_dOrgY; // 世界坐标的原点坐标
	double	scale; // 屏幕坐标到世界坐标的比例因子
	class CCommand* m_pCmd; 

	BOOL m_bIsPrinting;	//	是否正处在打印状态

	int m_nPageWidth,m_nPageHeight;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCadView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCadView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVCadView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPick();
	afx_msg void OnUpdatePick(CCmdUI* pCmdUI);
	afx_msg void OnRedraw();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModifyErase(CCmdUI* pCmdUI);
	afx_msg void OnEditManhadun();
	afx_msg void OnUpdateEditManhadun(CCmdUI* pCmdUI);
	afx_msg void OnMenuitem32807();
	afx_msg void OnMenuitem32808();
	afx_msg void OnMenuitem32809();
	afx_msg void OnMenuitem32810();
	afx_msg void OnMenuitem32811();
	afx_msg void OnMenuitem32812();
	afx_msg void OnMenuitem32813();
	afx_msg void OnMenuitem32814();
	afx_msg void OnMenuitem32815();
	afx_msg void OnInnerpt();
	//}}AFX_MSG
	afx_msg void OnCreateEntity(UINT m_nID);
	afx_msg void OnUpdateCreateCommand(CCmdUI* pCmdUI);
	afx_msg void OnModifyEntity(UINT m_nID);
	afx_msg void OnUpdateModifyCommand(CCmdUI* pCmdUI);
	//视图变换命令
	afx_msg void OnViewCommand(UINT m_nID);//

	DECLARE_MESSAGE_MAP()
private:
	void PrintPageFooter(CDC* pDC, CPrintInfo* pInfo);
	void PrintPageHeader(CDC* pDC, CPrintInfo* pInfo);
public:	
	// 世界坐标转换为屏幕坐标，输入：世界坐标pos， 输出：屏幕坐标screenPt
	void WorldtoScreen(const Position& pos, CPoint& screenPt); 
	// 屏幕坐标转换为世界坐标，输入：屏幕坐标screenPt， 输出：世界坐标pos
	void ScreentoWorld(const CPoint& screenPt, Position& pos);
	// 得到/设置屏幕坐标到世界坐标的比例因子
	double GetScale();
	void   SetScale(double dScale);
	// 进行全屏计算//
	void ZoomAll();//
	// 绘制坐标轴
	void DrawCoord(CDC *pDC) ;
	// 永久删除图元
	void Erase() ;
};

#ifndef _DEBUG  // debug version in VCadView.cpp
inline CVCadDoc* CVCadView::GetDocument()
   { return (CVCadDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCADVIEW_H__C161FBAC_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
