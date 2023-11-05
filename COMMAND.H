#ifndef _Command_h_
#define _Command_h_

#include "base.h"

#ifdef __cplusplus

enum ECommandType	// 命令类
{
	ctUnknown = 0,
	//  创建对象命令类
	ctCreateLine = 1,		// 创建对象
	ctCreateRectangle = 2,	// 创建矩形
	ctCreateCircle = 3,		// 创建圆
	ctCreateArc = 4,		// 创建圆弧
	ctCreateEllipse = 5,    //创建椭圆
	ctCreateText = 6,       //创建文本
	ctCreatePolygon = 7,    //多边形 
	ctCreateBlock = 8,		//块
	ctCreateConnect = 9,    //连接
	ctCreateComb = 10,
	ctInsertComb = 11,
	ctDeleteComb = 12,
	// ...其他创建类型
	//  修改命令类
	ctMove = 14,			// 移动
	ctRotate = 15,			// 旋转
	ctMirror = 16,			// 镜像
	// ...其他创建类型
	//修改视图命令类
	ctViewPan = 20,
	ctViewZoomRgn = 21
};

class CCommand
{
protected:
	int		m_nStep; // 命令操作步
	BOOL    m_bCaptured; //捕捉
	Position m_ptCapture; //捕捉点

	void     ShowCpaturePoint(const Position& pos, BOOL bShow = FALSE);
	BOOL     GetNearKeyPoint(Position& pt);
public:
	CCommand() { m_nStep = 0; m_bCaptured = FALSE; }
	~CCommand() {}

	virtual int GetType() = 0; // 返回命令类型 ECommandType
	virtual int OnLButtonDown(UINT nFlags, const Position& pos) = 0;
	virtual int OnMouseMove(UINT nFlags, const Position& pos);
	virtual int OnRButtonDown(UINT nFlags, const Position& pos) = 0;
	virtual int OnLButtonDblClk(UINT nFlags, const Position& pos) { return 0; }
	virtual int Cancel() = 0;
};

#endif

#endif
