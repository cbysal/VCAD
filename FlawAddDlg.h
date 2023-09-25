#pragma once


// CFlawAddDlg 对话框

class CFlawAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CFlawAddDlg)

public:
	CFlawAddDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFlawAddDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_FLAW_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_pt1_x;
	double m_pt1_y;
	double m_pt2_x;
	double m_pt2_y;
	double m_pt3_x;
	double m_pt3_y;
	double m_pt4_x;
	double m_pt4_y;
	virtual BOOL OnInitDialog();
	double m_sigma;
	double m_miu;
};
