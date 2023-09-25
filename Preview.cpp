//	本文件中的函数主要用于保存和显示预显位图	//		00-10-20

#include "stdafx.h"
#include "WinUser.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"

//	预显图形的大小
#define BMP_PREVIEW_WIDTH	200 
#define BMP_PREVIEW_HEIGHT	128

unsigned char *g_pBmpBuf = NULL;	//位图缓冲区

//	以下三个函数在外部使用
//将用于预显的位图写入文件
void BitmapSerialize(CArchive& ar);
//将客户窗口内的图形存为位图
void GenBitmap();
//	得到VCAD文件名
BOOL GetCADName(BOOL bIsOpen,CString& sFileName);

//压缩与解压缩位图的详细原理见《图像格式编程指南》（清华大学出版社）GIF图像一章
//压缩用于预显的位图 
DWORD Compressed(short *pComBuf,unsigned char *lpBuf,UINT nCount);
void pack_init(int clear,short *reqcnt,short first_ch,short *oldcode,
			   short *nextcode,short *nextlim,short *CTlink,short *CTnext,
			   unsigned char *CTfirst);
short lookup_ct(short code,unsigned char thischar,short *nextcode,
				short *CTlink,short *CTnext,unsigned char *CTfirst);
//解压缩用于预显的位图 
void DeCompressed(unsigned char *entBuffer,short *filebuf);
void unpack_init(short clear,short *nextcode,short *oldcode,
				short *reqcnt,short *nextlim,short first_ch,
				 unsigned char *CTfirst,unsigned char *CTlast,
				 short *CTlink);
void putx(short code,unsigned char *OStack,unsigned char *CTlast,
		  short *CTlink,unsigned char *entBuffer,DWORD *size);
void insert_ct(short code,short oldcode,short *nextcode,short *nextlim,
			   short *reqcnt,short *CTlink,unsigned char *CTfirst,
			   unsigned char *CTlast);

//	生成预览位图
void GenBitmap()
{
	CRect	clientRect;
	CBitmap	bitmap;
	CClientDC	dc(g_pView);
	CDC		memDC;
	DWORD	i=0,dwCount=0;
	int		x=0,y=0;
	COLORREF	color;
	
	CRect prevrect(0,0,BMP_PREVIEW_WIDTH,BMP_PREVIEW_HEIGHT);
	g_pView->GetClientRect(&clientRect);//得到客户窗口的矩形
	bitmap.CreateCompatibleBitmap(&dc,
			BMP_PREVIEW_WIDTH,BMP_PREVIEW_HEIGHT);//初始化空白位图
	memDC.CreateCompatibleDC(&dc);//创建内存设备情景对象
	memDC.SelectObject(&bitmap);//将位图选入内存设备情景对象

	memDC.SetMapMode(MM_TEXT); 

	memDC.Rectangle(&prevrect);	//	绘制边界并清除绘制区域

	//	根据当前绘图区大小和预览图形大小来计算缩放比例
	double dScale=1.0*clientRect.Width()/BMP_PREVIEW_WIDTH;
	//	缩小绘制所有元素
	  // 首先得到当前状态系统的设置
	double dOldScale = g_pView->GetScale(); // 缩放比例
	dScale *= dOldScale;
	double dOldOrgX = g_pView->m_dOrgX; // 屏幕中心点的世界坐标
	double dOldOrgY = g_pView->m_dOrgY;
      // 然后设置各状态变量，以便使得在设备环境左上方绘制图形
	g_pView->m_dOrgX = (clientRect.Width() - BMP_PREVIEW_WIDTH)/2 * dScale;
	g_pView->m_dOrgY = -(clientRect.Height() - BMP_PREVIEW_HEIGHT)/2 * dScale ;
	g_pView->SetScale(dScale);
	g_pView->OnDraw(&memDC);
	  // 最后恢复各系统状态设置
	g_pView->SetScale(dOldScale);
	g_pView->m_dOrgX = dOldOrgX;
	g_pView->m_dOrgY = dOldOrgY;
		
	dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//得到位图数据的字节数
	//	分配位图数据缓冲区
	if(g_pBmpBuf!=NULL)
		delete g_pBmpBuf;
	g_pBmpBuf=new BYTE[dwCount];			
	//	得到位图数据
	i=0;
	for(y=BMP_PREVIEW_HEIGHT-1;y>=0;y--)		
	{
		for(x=0;x<BMP_PREVIEW_WIDTH;x++)
		{
			i=(BMP_PREVIEW_HEIGHT-1-y)*BMP_PREVIEW_WIDTH+x;
			color=memDC.GetPixel(x,y);
			g_pBmpBuf[i*3]=(BYTE)((color&0x00ff0000)>>16);//B
			g_pBmpBuf[i*3+1]=(BYTE)((color&0x0000ff00)>>8);//G
			g_pBmpBuf[i*3+2]=(BYTE)(color&0x000000ff);//R
		}
	}

	memDC.SetMapMode(MM_LOENGLISH); 
}

//	将用于预显的位图数据写入文件 
void BitmapSerialize(CArchive& ar)
{
	DWORD	i=0,dwCount=0,len=0;
	dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//得到位图数据的字节数

	if(ar.IsStoring())
	{
		len=dwCount;
		ar<<len;			//压缩后的长度写入文件
		for(i=0;i<len;i++)
			ar<<g_pBmpBuf[i];	//将位图数据写入文件
		delete g_pBmpBuf;		//清除位图缓冲区
		g_pBmpBuf=NULL;
	}
	else
	{
		ar>>len;	//读入压缩位图的长度
		unsigned char buf;	//位图数据缓冲区
		for(i=0;i<len;i++)		//从文件中读入位图数据
			ar>>buf;
	}
}

//	从选择的文件中得到预显位图数据
BOOL GetFileSelect(unsigned char *entBuffer,char *filenamestring)
{
	CFile	 file;
	//打开文件
	if(!file.Open(filenamestring,CFile::modeRead)) 
		return FALSE;
	CArchive ar(&file,CArchive::load);

	DWORD dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//得到位图数据的字节数
	DWORD i,len;
	ar>>len;//文件中压缩后的位图数据长度
	for(i=0;i<len;i++)
		ar>>entBuffer[i];		//从文件中读入位图数据

	ar.Close();
	file.Close();
    return TRUE;
}

//	根据预显位图数据在指定位置显示位图
void DisplayBitmap(HDC hDC,int x,int y,unsigned char *entBuffer)
{
	BITMAPINFO bmInfo;
	bmInfo.bmiHeader.biSize=sizeof(bmInfo.bmiHeader);
	bmInfo.bmiHeader.biWidth=BMP_PREVIEW_WIDTH;
	bmInfo.bmiHeader.biHeight=BMP_PREVIEW_HEIGHT;
	bmInfo.bmiHeader.biPlanes=1;
	bmInfo.bmiHeader.biBitCount=24;
	bmInfo.bmiHeader.biCompression=BI_RGB;
	bmInfo.bmiHeader.biSizeImage=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;
	bmInfo.bmiHeader.biClrUsed=1;
	bmInfo.bmiHeader.biClrImportant=0;
	SetDIBitsToDevice(hDC,x,y,BMP_PREVIEW_WIDTH,
			BMP_PREVIEW_HEIGHT,0,0,0,BMP_PREVIEW_HEIGHT,entBuffer,
			&bmInfo,DIB_RGB_COLORS);
}

//	打开/保存文件的钩子函数
UINT APIENTRY OFNHookProc(HWND hdlg,UINT uiMsg,WPARAM wParam,LPARAM lParam)
{
	static char szFile[256]; //文件名
	static BOOL	bFolderChag;		//目录是否改变标志
	static BOOL bIsPreview = TRUE;	//是否选中预显图形
	static CRect	rectPrev=CRect(0,0,0,0);//预显矩形
	static BOOL bIsCADFlag = FALSE;	//选中的是否为CAD文件标志
	UINT	code;

	switch (uiMsg)
	{
	case WM_INITDIALOG:	//	初始化对话框
	{
		SetWindowLong(hdlg, -21, lParam);
		HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
		GetWindowRect(hWnd,&rectPrev);	//得到预显窗口矩形
		POINT point[2];
		point[0]=rectPrev.TopLeft();
		point[1]=rectPrev.BottomRight();
		ScreenToClient(hdlg,&point[0]);
		ScreenToClient(hdlg,&point[1]);
		rectPrev.left=point[0].x;
		rectPrev.right=point[1].x;
		rectPrev.top=point[0].y;
		rectPrev.bottom=point[1].y;
		DWORD dwCount=BMP_PREVIEW_WIDTH*BMP_PREVIEW_HEIGHT*3;//得到位图数据的字节数
		g_pBmpBuf=new unsigned char[dwCount];		//分配位图数据缓冲区
		if(g_pBmpBuf==NULL)
			return FALSE;
		bFolderChag=FALSE;
		bIsPreview=TRUE;
		HWND hCheck=GetDlgItem(hdlg,IDC_CHECK_PREVIEW);
		::SendMessage(hCheck,BM_SETCHECK,BST_CHECKED,0);

	}
		return FALSE;
	case WM_DESTROY:	//	对话框的销毁处理
		if(g_pBmpBuf!=NULL)
		{
			delete []g_pBmpBuf;	//删除位图数据缓冲区
			g_pBmpBuf=NULL;
		}
		bIsCADFlag=FALSE;
		return FALSE;
	case WM_PAINT:	//	绘制对话框中的预览位图
		{
			try
			{
				CWnd *pWnd;
				pWnd=CWnd::FromHandle(hdlg);
				HDC hDC=GetDC(hdlg);
				CDC *pDC;
				pDC=CDC::FromHandle(hDC);
				CPoint midpoint=rectPrev.CenterPoint();
				int nXOffset=16,nYOffset=4;
				if(bIsCADFlag==TRUE&&bIsPreview==TRUE)
				{	//选中的为合法文件且打开了预显开关
					//输出预显位图
					DisplayBitmap(hDC,midpoint.x+nXOffset-BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset-BMP_PREVIEW_HEIGHT/2,g_pBmpBuf);
				}
				else	
				{	//将预显部分涂成底色
					CBrush brush(RGB(192,192,192));
					CRect rect(midpoint.x+nXOffset-BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset-BMP_PREVIEW_HEIGHT/2,
							midpoint.x+nXOffset+BMP_PREVIEW_WIDTH/2,
							midpoint.y+nYOffset+BMP_PREVIEW_HEIGHT/2);
					pDC->FillRect(&rect,&brush);
				}
				ReleaseDC(hdlg,hDC);
			}
			catch (...)
			{
				return FALSE;
			}
			return FALSE;
		}
	case WM_COMMAND:	//	处理是否预显选项
		if(LOWORD(wParam)==IDC_CHECK_PREVIEW&&
				HIWORD(wParam)==BN_CLICKED)
		{
			bIsPreview=!bIsPreview;
			::SendMessage(hdlg,WM_PAINT,0,0);
		}
		return FALSE;
	case WM_NOTIFY:	//	处理通知消息
		code=((LPOFNOTIFY)lParam)->hdr.code;
		switch(code)
		{
		case CDN_SELCHANGE:	//	当选择不同的文件时
			if (CommDlg_OpenSave_GetFilePath(GetParent(hdlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))//得到文件名
			{
				if(bFolderChag==TRUE)//目录改变,已经在CDN_FOLDERCHANGE中进行了处理,因此返回
				{
					::SendMessage(hdlg,WM_PAINT,0,0);
					bFolderChag=FALSE;
					return FALSE;
				}
				//判断是否为合法文件
				int length=strlen(szFile);
				if(length<7)
					return FALSE;
				char ext[4];
				HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
				ext[3]='\0';
				ext[2]=szFile[length-1];
				ext[1]=szFile[length-2];
				ext[0]=szFile[length-3];
				_strlwr(ext);
				if(strcmp(ext,"cad")==0)
				{
					bIsCADFlag=TRUE;
					if(!GetFileSelect(g_pBmpBuf,szFile)) //得到位图数据
						bIsCADFlag=FALSE;
				}
				else 
					bIsCADFlag=FALSE;
				::SendMessage(hdlg,WM_PAINT,0,0);
			}
			return FALSE;
		case CDN_FOLDERCHANGE:	//	当选中的文件夹改变时
			//目录改变,如果新目录中没有CAD文件,那么就没有CDN_SELCHANGE消息发出
			//否则执行完这部分后就会执行CDN_DELCHANGE的消息处理
			if (CommDlg_OpenSave_GetFilePath(GetParent(hdlg),
				szFile, sizeof(szFile)) <= sizeof(szFile))
			{
				bFolderChag=TRUE;
				//判断是否为合法文件
				int length=strlen(szFile);
				if(length<7)
					return FALSE;
				char ext[4];
				HWND hWnd=GetDlgItem(hdlg,IDC_STATIC_FILEPREVIEW);
				ext[3]='\0';
				ext[2]=szFile[length-1];
				ext[1]=szFile[length-2];
				ext[0]=szFile[length-3];
				_strlwr(ext);
				if(strcmp(ext,"CAD")==0)
				{
					bIsCADFlag=TRUE;
					if(!GetFileSelect(g_pBmpBuf,szFile))
						bIsCADFlag=FALSE;
				}
				else bIsCADFlag=FALSE;
				::SendMessage(hdlg,WM_PAINT,0,0);
			}
			return FALSE;
		case CDN_FILEOK:	//	单击“确定”按钮
			return TRUE;
		}
	}
	return FALSE;
}

//	得到CAD文件名
BOOL GetCADName(BOOL bIsOpen,CString& sFileName)
{
	CString sFilter="图形文件";
	sFilter += "(*.CAD)|*.CAD|";	
	sFilter.Replace('|','\0');
	char	strFileName[256]="\0";
	CString sTitle;
	BOOL bRet=TRUE;

	if(bIsOpen)
		sTitle="打开文件";
	else
		sTitle="另存文件";

	OPENFILENAME ofn;

	ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = AfxGetMainWnd()->m_hWnd;
    ofn.hInstance         = AfxGetInstanceHandle();
    ofn.lpstrFilter       = sFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = strFileName;
    ofn.nMaxFile          = sizeof(strFileName);
    ofn.lpstrFileTitle    = NULL;
    ofn.nMaxFileTitle     = 0;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = sTitle;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "CAD";
	ofn.lpfnHook 		  = (LPOFNHOOKPROC)OFNHookProc;
	ofn.lpTemplateName    = MAKEINTRESOURCE(IDD_DIALOG_FILEPREVIEW);
    ofn.Flags             = OFN_ENABLETEMPLATE|//OFN_NOVALIDATE|
							 OFN_EXPLORER |OFN_ENABLEHOOK|OFN_HIDEREADONLY|
							 OFN_OVERWRITEPROMPT;

    if (bIsOpen)
		bRet=GetOpenFileName(&ofn);
	else
		bRet=GetSaveFileName(&ofn);
	
	if(bRet)
		sFileName=ofn.lpstrFile;

	return bRet;

}