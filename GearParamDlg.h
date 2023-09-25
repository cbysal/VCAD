#if !defined(AFX_GEARPARAMDLG_H__A23FD61D_78ED_4326_9F21_A08A67759158__INCLUDED_)
#define AFX_GEARPARAMDLG_H__A23FD61D_78ED_4326_9F21_A08A67759158__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GearParamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGearParamDlg dialog

class CGearParamDlg : public CDialog
{
// Construction
public:
	CGearParamDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGearParamDlg)
	enum { IDD = IDD_DIALOG1 };
	int		K3;
	int		K4;
	double	PD;
	double	Kb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGearParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGearParamDlg)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEARPARAMDLG_H__A23FD61D_78ED_4326_9F21_A08A67759158__INCLUDED_)
