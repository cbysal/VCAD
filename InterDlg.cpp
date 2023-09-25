// InterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcad.h"
#include "InterDlg.h"
#include "base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInterDlg dialog


CInterDlg::CInterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInterDlg)
	Xa = 0.0;
	Ya = 0.0;
	Za = 0.0;
	Xb = 0.0;
	Yb = 0.0;
	Zb = 0.0;
	Xc = 0.0;
	Yc = 0.0;
	Zc = 0.0;
	Xd = 0.0;
	Yd = 0.0;
	Zd = 0.0;
	Xk = 0.0;
	Yk = 0.0;
	Zk = 0.0;
	//}}AFX_DATA_INIT
}


void CInterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInterDlg)
	DDX_Text(pDX, IDC_EDIT1, Xa);
	DDX_Text(pDX, IDC_EDIT4, Ya);
	DDX_Text(pDX, IDC_EDIT6, Za);
	DDX_Text(pDX, IDC_EDIT9, Xb);
	DDX_Text(pDX, IDC_EDIT7, Yb);
	DDX_Text(pDX, IDC_EDIT11, Zb);
	DDX_Text(pDX, IDC_EDIT12, Xc);
	DDX_Text(pDX, IDC_EDIT13, Yc);
	DDX_Text(pDX, IDC_EDIT14, Zc);
	DDX_Text(pDX, IDC_EDIT19, Xd);
	DDX_Text(pDX, IDC_EDIT16, Yd);
	DDX_Text(pDX, IDC_EDIT17, Zd);
	DDX_Text(pDX, IDC_EDIT15, Xk);
	DDX_Text(pDX, IDC_EDIT18, Yk);
	DDX_Text(pDX, IDC_EDIT20, Zk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInterDlg, CDialog)
	//{{AFX_MSG_MAP(CInterDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT9, OnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT13, OnChangeEdit13)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterDlg message handlers
//一维线性插值
//p.x已知，求p.y
bool  GetValue_Line1(const Position p1, const Position p2, Position& p)
{
     if(p.x < p1.x || p.x > p2.x) //???
           return false;

     double A1x = (p.x - p2.x)/(p1.x - p2.x);
     double A2x = (p.x - p1.x)/(p2.x - p1.x);

	 p.y = A1x*p1.y + A2x*p2.y;
    
     return true;
}

void CInterDlg::OnButton1() 
{
	UpdateData(1);
	
	// TODO: Add your control notification handler code here
	if (Xk > Xa && Xk < Xb && Yk > Ya && Yk < Yc)
	{
		//在X方向上一维插值A,B得到E点
		Position Pa(Xa, Za);
		Position Pb(Xb, Zb);

		Position Pe;
		Pe.x = Xk;

		if (GetValue_Line1(Pa, Pb, Pe) == false)
		{
			AfxMessageBox("error");
			return;
		}

		//在X方向上一维插值C,D得到F点
		Position Pc(Xc, Zc);
		Position Pd(Xd, Zd);

		Position Pf;
		Pf.x = Xk;

		if (GetValue_Line1(Pc, Pd, Pf) == false)
		{
			AfxMessageBox("error");
			return;
		}

		//在Y方向上一维插值E,F得到点K
		Pe.x = Ya;
		Pf.x = Yc;
		Position Pk;
		Pk.x = Yk;

		if (GetValue_Line1(Pe, Pf, Pk) == false)
		{
			AfxMessageBox("error");
			return;
		}

		Zk = Pk.y;
		UpdateData(0);
	}
	else {
		AfxMessageBox("输入插值点不对！");
	}
}

void CInterDlg::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(1);

	Xc = Xa;

	UpdateData(0);
}

void CInterDlg::OnChangeEdit9() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(1);

	Xd = Xb;

	UpdateData(0);
	
}

void CInterDlg::OnChangeEdit4() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(1);

	Yb = Ya;

	UpdateData(0);
	
}

void CInterDlg::OnChangeEdit13() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(1);

	Yd = Yc;

	UpdateData(0);
	
}
