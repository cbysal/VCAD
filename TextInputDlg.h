#if !defined(AFX_TEXTINPUTDLG_H__D94D168A_C2D3_4CEA_A46D_DAA45D668D28__INCLUDED_)
#define AFX_TEXTINPUTDLG_H__D94D168A_C2D3_4CEA_A46D_DAA45D668D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextInputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextInputDlg dialog

class CTextInputDlg : public CDialog
{
// Construction
public:
	CTextInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextInputDlg)
	enum { IDD = IDD_TEXT_INPUT };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextInputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTINPUTDLG_H__D94D168A_C2D3_4CEA_A46D_DAA45D668D28__INCLUDED_)
