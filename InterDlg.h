#if !defined(AFX_INTERDLG_H__41D91C9B_7AB4_4D7B_BA53_8849566FF548__INCLUDED_)
#define AFX_INTERDLG_H__41D91C9B_7AB4_4D7B_BA53_8849566FF548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InterDlg.h : header file
//	

/////////////////////////////////////////////////////////////////////////////
// CInterDlg dialog

class CInterDlg : public CDialog
{
// Construction
public:
	CInterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInterDlg)
	enum { IDD = IDD_DIALOG2 };
	double	Xa;
	double	Ya;
	double	Za;
	double	Xb;
	double	Yb;
	double	Zb;
	double	Xc;
	double	Yc;
	double	Zc;
	double	Xd;
	double	Yd;
	double	Zd;
	double	Xk;
	double	Yk;
	double	Zk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInterDlg)
	afx_msg void OnButton1();
	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeEdit9();
	afx_msg void OnChangeEdit4();
	afx_msg void OnChangeEdit13();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERDLG_H__41D91C9B_7AB4_4D7B_BA53_8849566FF548__INCLUDED_)
