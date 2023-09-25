// VCad.h : main header file for the VCAD application
//

#if !defined(AFX_VCAD_H__C161FBA4_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
#define AFX_VCAD_H__C161FBA4_7612_11D4_BF0F_0000E8A36431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVCadApp:
// See VCad.cpp for the implementation of this class
//
extern class CVCadView*	g_pView;
extern class CVCadDoc*	g_pDoc;
extern UINT	g_nRefresh;
//
extern COLORREF				g_CurColor ;
extern int					g_CurLineStyle ;
extern int					g_CurLineWidth ;
//
class CVCadApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CVCadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCadApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVCadApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCAD_H__C161FBA4_7612_11D4_BF0F_0000E8A36431__INCLUDED_)
