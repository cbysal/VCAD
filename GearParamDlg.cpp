// GearParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vcad.h"
#include "GearParamDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGearParamDlg dialog


CGearParamDlg::CGearParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGearParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGearParamDlg)
	K3 = 1;
	K4 = 1;
	PD = 0.2;
	Kb = 0.0;
	//}}AFX_DATA_INIT
}


void CGearParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGearParamDlg)
	DDX_Text(pDX, IDC_EDIT1, K3);
	DDV_MinMaxInt(pDX, K3, 1, 4);
	DDX_Text(pDX, IDC_EDIT2, K4);
	DDV_MinMaxInt(pDX, K4, 1, 2);
	DDX_Text(pDX, IDC_EDIT3, PD);
	DDV_MinMaxDouble(pDX, PD, 0.1, 1.7);
	DDX_Text(pDX, IDC_EDIT4, Kb);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGearParamDlg, CDialog)
	//{{AFX_MSG_MAP(CGearParamDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGearParamDlg message handlers
int GetIndex(double d[], double pd)
{
	int i = 0;
	while ( i<8 )
	{
		if (fabs(pd - d[i++]) < 0.1) //pd 与 d[i] 接近
		{
			break;
		}
	}

	return i;
}

void CGearParamDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(1);

   double D[8] = {0.2, 0.4, 0.6,0.8,1.0,1.2,1.4, 1.6};
   //double Data[3][2][2] = {{{0,0},{0,0}},{{0,0},{0,0}},{{0,0},{0,0}}};
   double Data[4][2][8] = {{{0, 1, 1.005, 1.03,1.05,1.07,1.09, 1.13},\
   {0,1,1.03,1.06,1.1,1.14,1.16,1.25}}, {{1,1.02,1.04,1.06,1.08,1.12,1.14,1.18},\
   {1,1.04,1.08,1.13,1.17,1.23,1.28,1.35}}, {{1.025, 1.05,1.08,1.115,1.15,1.18,1.225,1.28},\
   {1.05,1.1,1.16,1.22,1.28,1.36,1.45,1.55}}, {{1.075,1.115,1.16,1.225,0,0,0,0},\
   {1.15,1.21,1.32,1.45,0,0,0,0}}}; 
  

   int index = GetIndex(D, PD); //近似得到index，？？？
   Kb = Data[K3-1][K4-1][index-1];

   UpdateData(0);
}
