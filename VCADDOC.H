// VCadDoc.h : interface of the CVCadDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
#define AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxtempl.h"

class Position;
class MEntity;

class CVCadDoc : public CDocument
{
private:
	CPoint	 m_basePos;
	CPoint   m_desPos;
	int		 m_nStep;	

protected: // create from serialization only
	CVCadDoc();
	DECLARE_DYNCREATE(CVCadDoc)

// Attributes
public:
	//CPtrList< CObList, MEntity*> m_tt;
	CObList		m_EntityList;  // 图元对象链表
	CObArray	m_selectArray; // 选择集 
	CObArray	m_clpArray;    //剪贴集
	MEntity*	m_pPmtEntity;  // 将被选中的图元（高亮显示）
// Operations
public:
	void SetModifiedFlag(BOOL bModified = TRUE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCadDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCadDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVCadDoc)
	afx_msg void OnSaveDxf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void	Draw(CDC *pDC);
	void	RemoveAllSelected(); // 清空选择集
	BOOL	IsSelected(MEntity* pEntity); // 判断某一图元对象是否被选中
	void	OnMouseMove(UINT nFlags, const Position& pos) ;
	void	OnLButtonDown(UINT nFlags, const Position& pos) ; 
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCADDOC_H__C161FBAA_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
