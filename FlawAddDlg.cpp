// FlawAddDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "resource.h"
#include "FlawAddDlg.h"
#include ".\flawadddlg.h"


// CFlawAddDlg 对话框

IMPLEMENT_DYNAMIC(CFlawAddDlg, CDialog)
CFlawAddDlg::CFlawAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlawAddDlg::IDD, pParent)
	, m_pt1_x(0)
	, m_pt1_y(0)
	, m_pt2_x(0)
	, m_pt2_y(0)
	, m_pt3_x(0)
	, m_pt3_y(0)
	, m_pt4_x(0)
	, m_pt4_y(0)
	, m_sigma(0)
	, m_miu(0)
{
}

CFlawAddDlg::~CFlawAddDlg()
{
}

void CFlawAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_pt1_x);
	DDX_Text(pDX, IDC_EDIT2, m_pt1_y);
	DDX_Text(pDX, IDC_EDIT4, m_pt2_x);
	DDX_Text(pDX, IDC_EDIT5, m_pt2_y);
	DDX_Text(pDX, IDC_EDIT6, m_pt3_x);
	DDX_Text(pDX, IDC_EDIT8, m_pt3_y);
	DDX_Text(pDX, IDC_EDIT9, m_pt4_x);
	DDX_Text(pDX, IDC_EDIT10, m_pt4_y);
	DDX_Text(pDX, IDC_EDIT7, m_sigma);
	DDX_Text(pDX, IDC_EDIT11, m_miu);
}


BEGIN_MESSAGE_MAP(CFlawAddDlg, CDialog)
END_MESSAGE_MAP()


// CFlawAddDlg 消息处理程序

BOOL CFlawAddDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
