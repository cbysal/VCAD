// Dxf.h: interface for the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_)
#define AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Entity.h"

class CDxf : public CFile  
{
private:
	CDocument*	m_pDoc; // 文档指针
	FILE*		fp;		// 输出文件指针
public:
	CDxf();
	virtual ~CDxf();
public:
	// 输出DXF文件
	BOOL	OutputDxf(CDocument* pDoc,CString filename);
private:
	// 写文件头
	BOOL	WriteHeader();
	// 写文件尾
	BOOL	WriteTail();
	// 写实体信息，该函数根据类型调用写图元信息的函数
	BOOL	WriteContent();

	// 写图元信息
	BOOL	WriteLine(MLines*	pLine);
	BOOL	WriteRectangle(MRectangle*	pRectangle);
	BOOL	WriteCircle(CCircle*	pCircle);
	BOOL	WriteArc(CArc*	pArc);
};

#endif // !defined(AFX_DXF_H__4CD92A21_9DBC_11D4_BF15_0000E8A36431__INCLUDED_)
