// PropertyBar.cpp : implementation file
//

#include "stdafx.h"
#include "vcad.h"
#include "PropertyBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//CLineStylData
void CLineStyleData::DrawItem(CDC* pDC, const CRect& rc, BOOL bSelected)
{
	CRect rcDraw(rc) ;
	COLORREF	crBackground, crText ;
	if(bSelected)
	{
		crBackground = GetSysColor(COLOR_HIGHLIGHT) ;
		crText = GetSysColor(COLOR_HIGHLIGHTTEXT) ;
	}
	else
	{
		crBackground = GetSysColor(COLOR_WINDOW) ;
		crText = GetSysColor(COLOR_WINDOWTEXT) ;
	}
	pDC->SetBkColor(crBackground) ;
	ExtTextOut(pDC->GetSafeHdc(), 0,0,ETO_OPAQUE, rcDraw, NULL, 0,NULL) ;

	CPen NewPen(m_nLineStyle, 1, RGB(0,0,0)) ;
	CPen* pOldPen = pDC->SelectObject(&NewPen) ;
	pDC->MoveTo(rcDraw.left, (rcDraw.top+rcDraw.bottom )/2) ;
	pDC->LineTo(rcDraw.left+80, (rcDraw.top+rcDraw.bottom)/2) ;
	pDC->SelectObject(pOldPen) ;
	rcDraw.left+=80 + 5 ;

	pDC->SetBkColor(crBackground) ;
	pDC->SetTextColor(crText) ;
	pDC->DrawText(m_strLname, m_strLname.GetLength(), rcDraw,
			DT_SINGLELINE|DT_VCENTER|DT_LEFT) ;
}
/////////////////////////////////////////////////////////////////////////////
//
void CLineWidthData::DrawItem(CDC* pDC, const CRect& rc, BOOL bSelected)
{
	CRect rcDraw(rc) ;
	COLORREF	crBackground, crText ;
	if(bSelected)
	{
		crBackground = GetSysColor(COLOR_HIGHLIGHT) ;
		crText = GetSysColor(COLOR_HIGHLIGHTTEXT) ;
	}
	else
	{
		crBackground = GetSysColor(COLOR_WINDOW) ;
		crText = GetSysColor(COLOR_WINDOWTEXT) ;
	}
	pDC->SetBkColor(crBackground) ;
	ExtTextOut(pDC->GetSafeHdc(), 0,0,ETO_OPAQUE, rcDraw, NULL, 0,NULL) ;

	CPen NewPen(0, m_nLineWidth, RGB(0,0,0)) ;
	CPen* pOldPen = pDC->SelectObject(&NewPen) ;
	pDC->MoveTo(rcDraw.left, (rcDraw.top+rcDraw.bottom )/2) ;
	pDC->LineTo(rcDraw.left+80, (rcDraw.top+rcDraw.bottom)/2) ;
	pDC->SelectObject(pOldPen) ;
	rcDraw.left+=80 + 5 ;

	pDC->SetBkColor(crBackground) ;
	pDC->SetTextColor(crText) ;
	pDC->DrawText(m_strWname, m_strWname.GetLength(), rcDraw,
			DT_SINGLELINE|DT_VCENTER|DT_LEFT) ;
}
/////////////////////////////////////////////////////////////////////////////
//CColorData
void CLineColorData::DrawItem(CDC* pDC, const CRect& rc, BOOL bSelected)
{
	CRect rcDraw(rc) ;
	COLORREF	crBackground, crText ;
	if(bSelected)
	{
		crBackground = GetSysColor(COLOR_HIGHLIGHT) ;
		crText = GetSysColor(COLOR_HIGHLIGHTTEXT) ;
	}
	else
	{
		crBackground = GetSysColor(COLOR_WINDOW) ;
		crText = GetSysColor(COLOR_WINDOWTEXT) ;
	}
	pDC->SetBkColor(crBackground) ;
	ExtTextOut(pDC->GetSafeHdc(), 0,0,ETO_OPAQUE, rcDraw, NULL, 0,NULL) ;

	CBrush NewBrush ;
	NewBrush.CreateSolidBrush(m_rgbColor) ;
	CBrush* pOldBrush = pDC->SelectObject(&NewBrush) ;
	pDC->Rectangle(rcDraw.left+2,rcDraw.top+2,rcDraw.left+16,rcDraw.top+16) ;
	pDC->SelectObject(pOldBrush) ;
	rcDraw.left+=16+5 ;

	pDC->SetBkColor(crBackground) ;
	pDC->SetTextColor(crText) ;
	pDC->DrawText(m_strCname, m_strCname.GetLength(), rcDraw, DT_SINGLELINE|
		DT_VCENTER|DT_LEFT) ;
}
//////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPropertyBar

CPropertyBar::CPropertyBar()
{
}

CPropertyBar::~CPropertyBar()
{
}


BEGIN_MESSAGE_MAP(CPropertyBar, CToolBar)
	//{{AFX_MSG_MAP(CPropertyBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyBar message handlers

/////////////////////////////////////////////////////////////////////////////
// CLineStyleCmb

CLineStyleCmb::CLineStyleCmb()
{
}

CLineStyleCmb::~CLineStyleCmb()
{
}


BEGIN_MESSAGE_MAP(CLineStyleCmb, CComboBox)
	//{{AFX_MSG_MAP(CLineStyleCmb)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineStyleCmb message handlers

/////////////////////////////////////////////////////////////////////////////
// CLineWidthCmb

CLineWidthCmb::CLineWidthCmb()
{
}

CLineWidthCmb::~CLineWidthCmb()
{
}


BEGIN_MESSAGE_MAP(CLineWidthCmb, CComboBox)
	//{{AFX_MSG_MAP(CLineWidthCmb)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineWidthCmb message handlers

/////////////////////////////////////////////////////////////////////////////
// CLineColorCmb

CLineColorCmb::CLineColorCmb()
{
}

CLineColorCmb::~CLineColorCmb()
{
}


BEGIN_MESSAGE_MAP(CLineColorCmb, CComboBox)
	//{{AFX_MSG_MAP(CLineColorCmb)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineColorCmb message handlers

void CLineColorCmb::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your specialized code here and/or call the base class
	CLineColorData* pData = 
		(CLineColorData*)(lpDeleteItemStruct->itemData) ;
	
	ASSERT(pData) ;
	
	delete pData ;
//	CComboBox::DeleteItem(lpDeleteItemStruct);
}

void CLineColorCmb::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC) ;
	CLineColorData* pData = 
		(CLineColorData*)(lpDrawItemStruct->itemData) ;
	ASSERT(pData) ;
	CRect rc(lpDrawItemStruct->rcItem) ;
	if(lpDrawItemStruct->itemID == LB_ERR)
		return ;
	if(lpDrawItemStruct->itemAction&(ODA_DRAWENTIRE | ODA_SELECT))
		pData->DrawItem(pDC, rc, 
				lpDrawItemStruct->itemState&ODS_SELECTED) ;
}

void CLineColorCmb::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemHeight = 18 ;
}

int CLineColorCmb::AddItem(COLORREF rgbColor, char* pszName)
{
	CLineColorData* pData = new CLineColorData(rgbColor, pszName) ;
	int nRet = AddString((LPCSTR) pData) ;
	if(nRet == LB_ERR) 
		delete pData ;
	return nRet ;
}
void CLineColorCmb::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSel() ;
	if(nIndex != LB_ERR)
	{
		CLineColorData* pData = (CLineColorData*)GetItemDataPtr(nIndex) ;
		if(pData != (CLineColorData*) - 1)
			g_CurColor = pData->m_rgbColor ;
	}
}

void CLineStyleCmb::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your specialized code here and/or call the base class
	CLineStyleData* pData = 
		(CLineStyleData*)(lpDeleteItemStruct->itemData) ;
	ASSERT(pData) ;
	delete pData ;
//	CComboBox::DeleteItem(lpDeleteItemStruct);
}

void CLineStyleCmb::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC=CDC:: FromHandle(lpDrawItemStruct->hDC);
	CLineStyleData* pData=(CLineStyleData*) (lpDrawItemStruct->itemData);
	ASSERT(pData);
	CRect rc(lpDrawItemStruct->rcItem);

	if(lpDrawItemStruct->itemID==LB_ERR)
		return;

	if(lpDrawItemStruct->itemAction&(ODA_DRAWENTIRE|ODA_SELECT))
		pData->DrawItem(pDC,rc,lpDrawItemStruct->itemState&ODS_SELECTED);

}

void CLineStyleCmb::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemHeight = 18 ;
}
int CLineStyleCmb::AddItem(int nLineStyle, char* pszName) 
{
	CLineStyleData* pData = 
		new CLineStyleData(nLineStyle, pszName) ;
	int nRet = AddString((LPCSTR) pData) ;
	if(nRet ==LB_ERR)
		delete pData ;
	return nRet ;
}
void CLineStyleCmb::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSel() ;
	if(nIndex != LB_ERR)
	{
		CLineStyleData* pData = 
			(CLineStyleData* )GetItemDataPtr(nIndex) ;
		if(pData != (CLineStyleData*) -1)
			g_CurLineStyle = pData->m_nLineStyle ;
	}
}
int CLineWidthCmb::AddItem(int nLineWidth, char* pszName)
{
	CLineWidthData* pData = new CLineWidthData(nLineWidth, pszName) ;
	int nRet = AddString((LPCSTR) pData) ;
	if(nRet == LB_ERR)
		delete pData ;
	return nRet ;
}
void CLineWidthCmb::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your specialized code here and/or call the base class
	CLineWidthData* pData = 
		(CLineWidthData*) (lpDeleteItemStruct->itemData) ;
	ASSERT( pData) ;
	delete pData ;
//	CComboBox::DeleteItem(lpDeleteItemStruct);
}

void CLineWidthCmb::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC) ;
	CLineWidthData* pData = (CLineWidthData*) 
		(lpDrawItemStruct->itemData) ;
	ASSERT(pData) ;
	CRect rc(lpDrawItemStruct->rcItem) ;
	if(lpDrawItemStruct->itemID == LB_ERR)
		return ;
	if(lpDrawItemStruct->itemAction&
		(ODA_DRAWENTIRE | ODA_SELECT))
		pData->DrawItem(pDC, rc, lpDrawItemStruct->itemState&
		ODS_SELECTED) ;

}

void CLineWidthCmb::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemHeight = 18 ;
}

void CLineWidthCmb::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSel() ;
	if(nIndex != LB_ERR) 
	{
		CLineWidthData* pData = (CLineWidthData*)
			GetItemDataPtr(nIndex) ;
		if(pData != (CLineWidthData*)-1)
		{
			g_CurLineWidth = pData->m_nLineWidth ;
		}
	}
}
