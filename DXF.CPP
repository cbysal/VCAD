// Dxf.cpp: implementation of the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vcad.h"
#include "VCadDoc.h"
#include "Dxf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxf::CDxf()
{
	m_pDoc = NULL;
	fp = NULL;
}

CDxf::~CDxf()
{
	if(m_pDoc)
		m_pDoc = NULL;
	if(fp)
		fp = NULL;
}
// 输出DXF文件
BOOL	CDxf::OutputDxf(CDocument* pDoc,CString filename)
{
// STEP 1 :	判断参数表中的参数是否满足要求
	// 判断给定文档类型是否我们需要需要的文档类型
	if(pDoc->IsKindOf(RUNTIME_CLASS(CVCadDoc)))
		m_pDoc = (CVCadDoc*)pDoc;
	else
		return FALSE;
	// 创建一个文件，保存DXF格式信息
	if ((fp=fopen((LPCSTR)filename,"w"))==NULL)
	{
		CString csError;
		csError.Format("%s%s出错","创建文件",filename);
		AfxMessageBox(csError);
		return FALSE;
	}
// STEP 2 : 将DXF信息写入文件中，必须按次序写文件
	if( !WriteHeader() ||	// 写文件头
		!WriteContent() ||	// 写图形信息
		!WriteTail() )		// 写文件尾
	{
		fclose(fp);
		DeleteFile(filename);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}
// 写文件头
BOOL	CDxf::WriteHeader()
{
	if(fp == NULL)
		return FALSE;
    try{
		fprintf (fp, "0\n");          
		fprintf (fp, "SECTION\n");
		fprintf (fp, "2\n");

		fprintf (fp, "ENTITIES\n");
		fprintf (fp, "0\n");
	}
	catch(...){
		AfxMessageBox("写DXF文件头时出错！");
		return FALSE;
	}

	return TRUE;
}
// 写文件尾
BOOL	CDxf::WriteTail()
{
	if(fp == NULL)
		return FALSE;
	try{
		fprintf (fp, "ENDSEC\n");
	    fprintf (fp, "0\n");
		fprintf (fp, "EOF\n");
	}
	catch(...){
		AfxMessageBox("写DXF文件尾时出错！");
		return FALSE;
	}

	return TRUE;
}
// 写实体信息，该函数根据类型调用写图元信息的函数
BOOL	CDxf::WriteContent()
{
	if( !m_pDoc || !fp)
		return FALSE;
	// 将图元链表中的图元信息写入DXF文件
	CVCadDoc*	pDoc = (CVCadDoc*)m_pDoc;
	POSITION	pos = pDoc->m_EntityList.GetHeadPosition();
	while(pos!=NULL)
	{
		MEntity*	pEntity = (MEntity *) pDoc->m_EntityList.GetNext(pos);
		if(!pEntity)
			return FALSE;
		// 根据图元的类型将图元信息写入DXF文件
		if( pEntity->IsKindOf(RUNTIME_CLASS(MLines)) ){
			MLines*	pLine = (MLines*)pEntity;
			WriteLine(pLine);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(MRectangle)) ){
			MRectangle*	pRectangle = (MRectangle*)pRectangle;
			WriteRectangle(pRectangle);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(CCircle)) ){
			CCircle*	pCircle = (CCircle*)pEntity;
			WriteCircle(pCircle);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(MLines)) ){
			CArc*	pArc = (CArc*)pEntity;
			WriteArc(pArc);
		}
		else{
			AfxMessageBox("在存储为DXF格式时，本系统可能会丢失一些图形信息！");
		}
	}
	return TRUE;
}
// 写图元信息
BOOL	CDxf::WriteLine(MLines*	pLine)
{
	if(!pLine || !fp)
		return FALSE;
//按格式保存直线的起始点和终点的坐标，Z坐标为0
	double x1,y1,x2,y2;       
	x1 = pLine->GetBeginPos().x; 
	y1 = pLine->GetEndPos().y; 
	x2 = pLine->GetBeginPos().x; 
	y2 = pLine->GetEndPos().y; 
	
	try{
		fprintf (fp, "LINE\n");
		fprintf (fp, "8\n");     
		fprintf (fp, "0\n");
		fprintf (fp, "10\n");     //10，20，30分别是起点的x、y、z坐标的组码
		fprintf (fp, "%lf\n", x1);
		fprintf (fp, "20\n");
		fprintf (fp, "%lf\n", y1);
		fprintf (fp, "30\n");
		fprintf (fp, "0.0\n");
		fprintf (fp, "11\n");       //11，21，31分别是终点的x、y、z坐标的组码
		fprintf (fp, "%lf\n", x2);
		fprintf (fp, "21\n");
		fprintf (fp, "%lf\n", y2);
		fprintf (fp, "31\n");
		fprintf (fp, "0.0\n");
		fprintf (fp, "0\n");
	}
	catch(...){
		AfxMessageBox("向DXF文件输出直线段信息时出错！");
		return FALSE;
	}
	return TRUE;
}

BOOL	CDxf::WriteRectangle(MRectangle*	pEntity)
{
	return TRUE;
}

BOOL	CDxf::WriteCircle(CCircle*	pEntity)
{
	return TRUE;
}

BOOL	CDxf::WriteArc(CArc*	pEntity)
{
	return TRUE;
}


