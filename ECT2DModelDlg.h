#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CVCadView;
class MEntity;
// CECT2DModelDlg 对话框

class CECT2DModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CECT2DModelDlg)

public:
	CECT2DModelDlg(CVCadView* pView, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CECT2DModelDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHENDEZHI };

private:
	CVCadView * m_pDrawView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void         AddEntity(MEntity *pEnt);
	void         GetFlaw4Pos(int i, Position &p1, Position &p2, Position &p3, Position &p4);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	double m_coil_r1;
	double m_coil_r2;
	double m_coil_h;
	double m_coil_distance;
	double m_coil_current;
	double m_coil_turn;
	double m_coil_freq;

	double m_tube_r1;
	double m_tube_r2;
	double m_tube_sigma;
	double m_tube_miu;

	BOOL m_bSupported;
	double m_support_r1;
	double m_support_pos;
	double m_support_h;
	double m_support_sigma;
	double m_support_miu;

	double m_scan_startpos;
	int    m_scan_steps;
	double m_scan_step;
	
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonAdd2();
	afx_msg void OnLbnDblclkList1();
	virtual BOOL OnInitDialog();
	CListCtrl m_flawlistCtrl;
	afx_msg void OnHdnItemdblclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
};
