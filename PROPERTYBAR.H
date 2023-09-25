#if !defined(AFX_PROPERTYBAR_H__2BAC3A61_AD6D_11D4_B543_00001C538465__INCLUDED_)
#define AFX_PROPERTYBAR_H__2BAC3A61_AD6D_11D4_B543_00001C538465__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyBar.h : header file
//
class CLineStyleData
{
public:
	int			m_nLineStyle ;
	CString		m_strLname ;
public:
	CLineStyleData(int nLineStyle, char * pLineStyleName)
		:m_nLineStyle(nLineStyle),m_strLname(pLineStyleName)
	{};
	void DrawItem(CDC* pDC, const CRect& rc,BOOL bSelected) ;
};
///////////////////////////////////////////////////////////////
class CLineColorData
{
public:
	COLORREF	m_rgbColor ;
	CString		m_strCname ;
public:
	CLineColorData(COLORREF rgbcolor, char* pColorName)
		:m_rgbColor(rgbcolor), m_strCname(pColorName)
	{};
	void DrawItem(CDC* pDC, const CRect& rc, BOOL bSelected) ;
};
/////////////////////////////////////////////////////////////////////////////
// CLineWidthData 
class CLineWidthData
{
public:
	int		 m_nLineWidth ;
	CString  m_strWname ;
public:
	CLineWidthData(int nLineWidth, char  * pLineWidthName) 
		:m_nLineWidth(nLineWidth),m_strWname(pLineWidthName)
	{} ;
	void DrawItem(CDC* pDC, const CRect& rc, BOOL bSelected) ;
};
/////////////////////////////////////////////////////////////////////////////
// CLineStyleCmb window
class CLineStyleCmb : public CComboBox
{
// Construction
public:
	CLineStyleCmb();

// Attributes
public:

// Operations
public:
	int AddItem(int nLineStyle, char* pszName) ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineStyleCmb)
	public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLineStyleCmb();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineStyleCmb)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CLineWidthCmb window

class CLineWidthCmb : public CComboBox
{
// Construction
public:
	CLineWidthCmb();

// Attributes
public:

// Operations
public:
	int AddItem(int nLineWidth, char* pszName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineWidthCmb)
	public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLineWidthCmb();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineWidthCmb)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CLineColorCmb window

class CLineColorCmb : public CComboBox
{
// Construction
public:
	CLineColorCmb();

// Attributes
public:
	int AddItem(COLORREF rgbColor, char* pszName) ;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineColorCmb)
	public:
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLineColorCmb();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineColorCmb)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CPropertyBar window

class CPropertyBar : public CToolBar
{
// Construction
public:
	CPropertyBar();

// Attributes
public:
	CLineColorCmb	m_wndLineColorCmb ;
	CLineWidthCmb	m_wndLineWidthCmb ;
	CLineStyleCmb	m_wndLineStyleCmb ;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYBAR_H__2BAC3A61_AD6D_11D4_B543_00001C538465__INCLUDED_)
