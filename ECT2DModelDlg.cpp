// ECT2DModelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "base.h"
#include "entity.h"
#include "vcaddoc.h"
#include "VCadView.h"
#include "ECT2DModelDlg.h"
#include "FlawAdddlg.h"


// CECT2DModelDlg 对话框

IMPLEMENT_DYNAMIC(CECT2DModelDlg, CDialog)


CECT2DModelDlg::CECT2DModelDlg(CVCadView* pDrawView, CWnd* pParent /*=NULL*/)
	: CDialog(CECT2DModelDlg::IDD, pParent)
	, m_bSupported(FALSE)
	, m_coil_r1(6.5)
	, m_coil_r2(8.0)
	, m_coil_h(1.5)
	, m_coil_distance(1.5)
	, m_coil_current(1)
	, m_coil_turn(34)
	, m_coil_freq(550)

	, m_tube_r1(8.4)
	, m_tube_r2(9.5)
	, m_tube_sigma(1.03)
	, m_tube_miu(1)

	, m_support_r1(9.5)
	, m_support_pos(0)
	, m_support_h(2.0)
	, m_support_sigma(1.03)
	, m_support_miu(1)

	, m_scan_startpos(0)
	, m_scan_steps(20)
	, m_scan_step(1)
{
	m_pDrawView = pDrawView;
}

CECT2DModelDlg::~CECT2DModelDlg()
{
}

void CECT2DModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_R1, m_coil_r1);
	DDX_Text(pDX, IDC_EDIT_R2, m_coil_r2);
	DDX_Text(pDX, IDC_EDIT_H, m_coil_h);
	DDX_Text(pDX, IDC_EDIT_DIS, m_coil_distance);
	DDX_Text(pDX, IDC_EDIT_CUR, m_coil_current);
	DDX_Text(pDX, IDC_EDIT_TURN, m_coil_turn);
	DDX_Text(pDX, IDC_EDIT_FREQ, m_coil_freq);

	DDX_Text(pDX, IDC_EDIT_TR1, m_tube_r1);
	DDX_Text(pDX, IDC_EDIT_TR2, m_tube_r2);
	DDX_Text(pDX, IDC_EDIT_TSIG, m_tube_sigma);
	DDX_Text(pDX, IDC_EDIT_TMIU, m_tube_miu);

	DDX_Text(pDX, IDC_EDIT_SR1, m_support_r1);
	DDX_Text(pDX, IDC_EDIT_SPOS, m_support_pos);
	DDX_Text(pDX, IDC_EDIT_SH, m_support_h);
	DDX_Text(pDX, IDC_EDIT_SSIG, m_support_sigma);
	DDX_Text(pDX, IDC_EDIT_SMIU, m_support_miu);

	DDX_Text(pDX, IDC_EDIT_STARTPOS, m_scan_startpos);
	DDX_Text(pDX, IDC_EDIT_SCANSTEPS, m_scan_steps);
	DDX_Text(pDX, IDC_EDIT_STEP, m_scan_step);

	DDV_MinMaxDouble(pDX, m_support_h, 0, 10000);
	DDX_Check(pDX, IDC_CHECK1, m_bSupported);

	DDX_Control(pDX, IDC_LIST2, m_flawlistCtrl);
}


BEGIN_MESSAGE_MAP(CECT2DModelDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_ADD2, OnBnClickedButtonAdd2)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickList2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnNMDblclkList2)
END_MESSAGE_MAP()


// CECT2DModelDlg 消息处理程序

void CECT2DModelDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//画几何模型
	//1。画外框
	Position P1,P2,P3,P4;
	P1.x = 0;
	P1.y = -5*m_tube_r2;
	P2.x = 6*m_tube_r2;
	P2.y = P1.y;
	P3.x = P2.x;
	P3.y = - P2.y;
	P4.x = 0;
	P4.y = P3.y;

	MLines *rectOutLine1 = new MLines(P1, P2);
	AddEntity(rectOutLine1);
	rectOutLine1 = new MLines(P2, P3);
	AddEntity(rectOutLine1);
	rectOutLine1 = new MLines(P3, P4);
	AddEntity(rectOutLine1);
	rectOutLine1 = new MLines(P4, P1);
	AddEntity(rectOutLine1);

	//2. 画管子 
	Position P5,P6,P7,P8;
	P5.x = m_tube_r1;
	P5.y = P1.y;
	P6.x = m_tube_r2;
	P6.y = P5.y;
	P7.x = P6.x;
	P7.y = - P6.y;
	P8.x = P5.x;
	P8.y = P7.y;

	MRectangle * rectTube =  new MRectangle(P5, P7);
	AddEntity(rectTube);

	//如果有支撑板，画之
	if(m_bSupported)
	{
		Position P9, P10;
		P9.x = m_support_r1;
		P9.y = - m_support_h / 2 + m_support_pos;
		P10.x = P3.x;
		P10.y = m_support_h / 2 + m_support_pos;

		MRectangle * rectSupport = new MRectangle(P9, P10);
		AddEntity(rectSupport);
	}

	//3. 画线圈
	Position p1,p2,p3,p4;
	//上线圈
	p1.x = m_coil_r1;
	p1.y = m_coil_h / 2;
	p2.x = m_coil_r2;
	p2.y = p1.y;
	p3.x = p2.x;
	p3.y = p2.y + m_coil_h;
	p4.x = p1.x;
	p4.y = p3.y;
	MRectangle *rectCoil1 = new MRectangle(p1, p3);
	AddEntity(rectCoil1);

	MLines * l1, *l2;
	l1 = new MLines(p1, p3);
	AddEntity(l1);
	l2 = new MLines(p2, p4);
	AddEntity(l2);

	// 下线圈
	p1.y = - m_coil_h / 2;
	p2.y = p1.y;
	p3.x = p2.x;
	p3.y = p2.y - m_coil_h;
	p4.x = p1.x;
	p4.y = p3.y;
	MRectangle *rectCoil2 = new MRectangle(p1, p3);
	AddEntity(rectCoil2);

	l1 = new MLines(p1, p3);
	AddEntity(l1);
	l2 = new MLines(p2, p4);
	AddEntity(l2);

	//4.画缺陷
	int numOfFlaws = m_flawlistCtrl.GetItemCount();
	for(int i=0; i<numOfFlaws; i++)
	{
		GetFlaw4Pos(i, p1, p2, p3, p4);

		MLines* ln1,*ln2,*ln3,*ln4;
		ln1 = new MLines(p1, p2);
		AddEntity(ln1);
		ln2 = new MLines(p2, p3);
		AddEntity(ln2);
		ln3 = new MLines(p3, p4);
		AddEntity(ln3);
		ln4 = new MLines(p4, p1);
		AddEntity(ln4);
	}


	OnOK();
}

void CECT2DModelDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CECT2DModelDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CECT2DModelDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(1);

	if(m_bSupported)
	{
		GetDlgItem(IDC_EDIT_SR1)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_SPOS)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_SH)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_SSIG)->EnableWindow(1);
		GetDlgItem(IDC_EDIT_SMIU)->EnableWindow(1);
	}
	else
	{
		GetDlgItem(IDC_EDIT_SR1)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SPOS)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SH)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SSIG)->EnableWindow(0);
		GetDlgItem(IDC_EDIT_SMIU)->EnableWindow(0);
	}
}

void CECT2DModelDlg::OnBnClickedButtonAdd()//增加
{
	// TODO: 在此添加控件通知处理程序代码
	CFlawAddDlg dlg;

	if(dlg.DoModal() == IDOK)
	{
		CString sAdd, s1,s2,s3,s4;

		s1.Format("%6.2f,%6.2f" , dlg.m_pt1_x, dlg.m_pt1_y);
		s2.Format("%6.2f,%6.2f" , dlg.m_pt2_x, dlg.m_pt2_y);
		s3.Format("%6.2f,%6.2f" , dlg.m_pt3_x, dlg.m_pt3_y);
		s4.Format("%6.2f,%6.2f" , dlg.m_pt4_x, dlg.m_pt4_y);

		sAdd = s1;
		sAdd += s2 + s3 + s4;

		//m_flawlist.AddString(sAdd);

		int nCount = m_flawlistCtrl.GetItemCount();
		int nItem = m_flawlistCtrl.InsertItem(nCount,s1);
		m_flawlistCtrl.SetItemText(nItem, 1,s2);
		m_flawlistCtrl.SetItemText(nItem, 2,s3);
		m_flawlistCtrl.SetItemText(nItem, 3,s4);
		char stmp[10];
		sprintf(stmp, "%6.2f", dlg.m_sigma);
		m_flawlistCtrl.SetItemText(nItem, 4,stmp);
		sprintf(stmp, "%6.2f", dlg.m_miu);
		m_flawlistCtrl.SetItemText(nItem, 5,stmp);
	}
}

void CECT2DModelDlg::OnBnClickedButtonDel()//删除
{
	// TODO: 在此添加控件通知处理程序代码
/*	int nSel = m_flawlist.GetCurSel();

	if(nSel >= 0)
	{
		m_flawlist.DeleteString(nSel);
	}
	*/

	int nSel = m_flawlistCtrl.GetNextItem(-1, LVNI_SELECTED);
	m_flawlistCtrl.DeleteItem(nSel);
}

void CECT2DModelDlg::OnBnClickedButtonDelall()//全部删除
{
	// TODO: 在此添加控件通知处理程序代码
	//m_flawlist.ResetContent();
	m_flawlistCtrl.DeleteAllItems();
}

void CECT2DModelDlg::OnBnClickedButtonAdd2()//编辑
{
	// TODO: 在此添加控件通知处理程序代码
	CFlawAddDlg dlg;

/*	CString sSel, sNum;
	m_flawlist.GetText(m_flawlist.GetCurSel(), sSel);
	
	int pos;
	pos = sSel.Find(":");
	if(pos > 0)
	{
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	sSel.TrimLeft();
	dlg.m_pt1_x = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt1_y = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(":");
	if(pos > 0)
	{
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt2_x = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt2_y = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(":");
	if(pos > 0)
	{
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt3_x = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt3_y = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(":");
	if(pos > 0)
	{
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt4_x = atof((LPCSTR)sNum);

	sSel.TrimLeft();
	pos = sSel.Find(" ");
	if(pos > 0)
	{
		sNum = sSel.Left(pos);
		sSel = sSel.Right(sSel.GetLength() - pos - 1);
	}
	dlg.m_pt4_y = atof((LPCSTR)sNum);
*/

	int nSel = m_flawlistCtrl.GetNextItem(-1, LVNI_SELECTED);
	Position p1,p2,p3,p4;
	GetFlaw4Pos(nSel, p1,p2,p3,p4);

	dlg.m_pt1_x = p1.x;
	dlg.m_pt1_y = p1.y;
	dlg.m_pt2_x = p2.x;
	dlg.m_pt2_y = p2.y;
	dlg.m_pt3_x = p3.x;
	dlg.m_pt3_y = p3.y;
	dlg.m_pt4_x = p4.x;
	dlg.m_pt4_y = p4.y;

	CString	sPos = m_flawlistCtrl.GetItemText(nSel, 4);
	dlg.m_sigma = atof((LPCSTR)sPos);
	sPos = m_flawlistCtrl.GetItemText(nSel, 5);
	dlg.m_miu = atof((LPCSTR)sPos);

	if(dlg.DoModal() == IDOK)
	{
		OnBnClickedButtonDel();

		/////////////////////////////
		CString sAdd, s1,s2,s3,s4;

		s1.Format("%6.2f,%6.2f" , dlg.m_pt1_x, dlg.m_pt1_y);
		s2.Format("%6.2f,%6.2f" , dlg.m_pt2_x, dlg.m_pt2_y);
		s3.Format("%6.2f,%6.2f" , dlg.m_pt3_x, dlg.m_pt3_y);
		s4.Format("%6.2f,%6.2f" , dlg.m_pt4_x, dlg.m_pt4_y);

		sAdd = s1;
		sAdd += s2 + s3 + s4;

		int nCount = m_flawlistCtrl.GetItemCount();
		int nItem = m_flawlistCtrl.InsertItem(nCount,s1);
		m_flawlistCtrl.SetItemText(nItem, 1,s2);
		m_flawlistCtrl.SetItemText(nItem, 2,s3);
		m_flawlistCtrl.SetItemText(nItem, 3,s4);
		char stmp[10];
		sprintf(stmp, "%6.2f", dlg.m_sigma);
		m_flawlistCtrl.SetItemText(nItem, 4,stmp);
		sprintf(stmp, "%6.2f", dlg.m_miu);
		m_flawlistCtrl.SetItemText(nItem, 5,stmp);
	}
}

void CECT2DModelDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButtonAdd2();
}

BOOL CECT2DModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	///
	m_flawlistCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_flawlistCtrl.InsertColumn(0,"P1",LVCFMT_LEFT,60);
	m_flawlistCtrl.InsertColumn(1,"P2",LVCFMT_LEFT,60);
	m_flawlistCtrl.InsertColumn(2,"P3",LVCFMT_LEFT,60);
	m_flawlistCtrl.InsertColumn(3,"P4",LVCFMT_LEFT,60);
	m_flawlistCtrl.InsertColumn(4,"电导率",LVCFMT_LEFT,60);
	m_flawlistCtrl.InsertColumn(5,"磁导率",LVCFMT_LEFT,60);

	// TODO:  在此添加额外的初始化
	OnBnClickedCheck1();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


///
void CECT2DModelDlg::AddEntity(MEntity * pEnt)
{
	m_pDrawView->AddEntity(pEnt);
}


void CECT2DModelDlg::GetFlaw4Pos(int nSel, Position &p1, Position &p2, Position &p3, Position &p4)
{
	CString sPos, sX, sY;
	int pos;
	sPos = m_flawlistCtrl.GetItemText(nSel, 0);
	pos = sPos.Find(",");
	sX = sPos.Left(pos);
	sY = sPos.Right(sPos.GetLength() - pos -1);
	p1.x = atof((LPCSTR)sX);
	p1.y = atof((LPCSTR)sY);

	sPos = m_flawlistCtrl.GetItemText(nSel, 1);
	pos = sPos.Find(",");
	sX = sPos.Left(pos);
	sY = sPos.Right(sPos.GetLength() - pos -1);
	p2.x = atof((LPCSTR)sX);
	p2.y = atof((LPCSTR)sY);

	sPos = m_flawlistCtrl.GetItemText(nSel, 2);
	pos = sPos.Find(",");
	sX = sPos.Left(pos);
	sY = sPos.Right(sPos.GetLength() - pos -1);
	p3.x = atof((LPCSTR)sX);
	p3.y = atof((LPCSTR)sY);

	sPos = m_flawlistCtrl.GetItemText(nSel, 3);
	pos = sPos.Find(",");
	sX = sPos.Left(pos);
	sY = sPos.Right(sPos.GetLength() - pos -1);
	p4.x = atof((LPCSTR)sX);
	p4.y = atof((LPCSTR)sY);
}

void CECT2DModelDlg::OnHdnItemdblclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButtonAdd2();

	*pResult = 0;
}

void CECT2DModelDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedButtonAdd2();

	*pResult = 0;
}
