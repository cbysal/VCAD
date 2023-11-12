#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "base.h"
#include <vector>

#define PICK_RADIUS 0.01 // 选取半径

#define ENTITY_NUM	12 // 图元的种类数
enum EEntityType // 图元的类型
{
	etUnknow = 0,
	etPoint = 1,
	etLine = 2,
	etRectangle = 3,
	etCircle = 4,
	etArc = 5,
	etBspline = 6,
	etEllipse = 7,
	etText = 8,
	etPolygon = 9,
	etBlock = 10,
	etConnect = 11,
	etComb = 12
};

enum EDrawMode // 绘图模式
{
	dmNormal = 0, // 正常状态
	dmSelect = 1, // 选中状态	
	dmPrompt = 2, // 提示状态
	dmDrag = 3, // 拖动状态
	dmInvalid = 4 // 擦除状态
};

//////////////////////////
// define API function
//
void	SetDrawEnvir(CDC* pDC, int drawMode, CPen* pPen);


class MEntity : public CObject
{
	DECLARE_SERIAL(MEntity)
protected:
	// 成员变量
	int			m_type;		// 图元类型（EEntityType）
	COLORREF	m_color;   // 图元颜色
	UINT		m_lineStyle; // 图元的线型
	int			m_lineWidth; // 图元的线宽	
public:
	int			m_nOperationNum;  //第几次操作声称的
	//BOOL		m_bErased;        //实体临时删除标志
	MEntity* next;
	MEntity* GetShowEnt();      //获得显示的实例
	MEntity* GetCurrentEnt();
	MEntity* Erase();
	void		Remove();
public:
	// 构造函数和析构函数
	MEntity();
	MEntity(const MEntity& entity);
	~MEntity();

	MEntity operator = (const MEntity& entity); // 重载等号
	virtual	MEntity* Copy() { return NULL; } // 指针拷贝

	virtual	void	Init(); // 初始化成员变量值
	virtual	int		GetType() { return m_type; }  // 返回图元的类型（EEntityType）
	COLORREF		GetColor() { return m_color; } // 返回图元颜色
	void			SetColor(COLORREF color) { m_color = color; } // 设置图元颜色
	void            SetWidth(int nWidth) { m_lineWidth = nWidth; }
	void			Append(MEntity* pEnt);

	virtual void Draw(CDC* pDC, int drawMode = dmNormal) {}; // 绘制图元对象

	// 给定一点及拾取半径，判断图元是否被选中
	virtual BOOL Pick(const Position& pos, const double pick_radius) { return FALSE; }
	// 得到对象的最小包围盒，该包围盒将被用于图元的选取和全屏显示
	virtual void GetBox(BOX2D* pBox) {}
	// 给定一个基点和一个目标点平移图元
	virtual void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE) {}
	// 给定一个基点和一个角度值旋转图元
	virtual void Rotate(const Position& basePos, const double angle) {}
	// 给定两点镜像图元，布尔变量bCopy用于确定是否删除原来位置的图元
	virtual void Mirror(const Position& pos1, const Position& pos2) {}
	// 改变光标
	virtual	void LoadPmtCursor() {}
	// Get a feature position of a entity near the giving position.
	// Input : giving a pos
	// output : TRUE or FALSE
	// Note : if true , the giving pos is reset to the feature position
	virtual BOOL GetSnapPos(Position& pos) { return FALSE; }

	virtual void Serialize(CArchive& ar); // 图元对象串行化
};

////////////////////////////
// CLASS MLines
//
class MLines : public MEntity
{
	DECLARE_SERIAL(MLines)
protected:
	Position	m_begin, m_end; // 起点和终点
	//Position *  m_pPositions;
	std::vector<Position> m_pPositions;
	//int			m_numpts;

public:
	MLines();
	MLines(const MLines& line);
	MLines(const Position& begin, const Position& end);
	MLines(std::vector<Position>& poses);
	MLines(int numpts, Position* poses);
	~MLines();

	MLines& operator = (const MLines& line);
	MEntity* Copy();

	int			GetType();
	void		Init();
	Position	GetBeginPos(); // 返回起点值
	Position	GetEndPos();   // 返回终点值

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);

	//增加接口
	int GetNumOfPoints() const {
		//return m_numpts;
		return m_pPositions.size();
	}
	std::vector<Position> GetPoints() const {
		//return m_pPositions;
		return m_pPositions;
	}
};

////////////////////////////
// CLASS MConnect
//
class MConnect : public MEntity
{
	DECLARE_SERIAL(MConnect)
protected:
	Position	m_begin, m_end; // 起点和终点
	Position* m_pPositions;
	int			m_numpts;
public:
	MConnect();
	MConnect(const MConnect& line);
	MConnect(const Position& begin, const Position& end);
	MConnect(int num, Position* pPos);
	~MConnect();

	MConnect& operator = (const MConnect& line);
	MEntity* Copy();
	void	    Manhadunize();

	int			GetType();
	void		Init();
	Position	GetBeginPos(); // 返回起点值
	Position	GetEndPos();   // 返回终点值
	void MConnect::SetBeginPos(const Position& pos);
	void MConnect::SetEndPos(const Position& pos);

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS MLines
//
class MPolygon : public MEntity
{
	DECLARE_SERIAL(MPolygon)
protected:
	Position	m_begin, m_end; // 起点和终点
	Position* m_pPositions;
	int			m_numpts;
public:
	MPolygon();
	MPolygon(const MPolygon& line);
	MPolygon(const Position& begin, const Position& end);
	MPolygon(int num, Position* pPos);
	~MPolygon();

	MPolygon& operator = (const MPolygon& line);
	MEntity* Copy();

	int			GetType();
	void		Init();
	Position	GetBeginPos(); // 返回起点值
	Position	GetEndPos();   // 返回终点值

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS MRectangle
//
class MRectangle : public MEntity
{
	DECLARE_SERIAL(MRectangle)
protected:
	Position	m_LeftTop, m_RightBottom; // 起点和终点
public:
	MRectangle();
	MRectangle(const MRectangle& rect);
	MRectangle(const Position& LeftTop, const Position& RightBottom);
	~MRectangle();

	MRectangle& operator = (const MRectangle& rect);
	MEntity* Copy();

	int			GetType();
	void		Init();
	Position	GetLeftTopPos();		// 返回左上角的值
	Position	GetRightBottomPos();	// 返回右下角的值

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS MBlock
//
class MBlock : public MEntity
{
	DECLARE_SERIAL(MBlock)
protected:
	Position	m_Import, m_Outport; // 输入和输出
	Position	m_LeftTop, m_RightBottom; // 起点和终点

	//CString		m_EntText;             //实体
	//CObList		m_EntList;             //实体连标
	CBitmap* m_Bitmap;

public:
	MBlock();
	MBlock(const MBlock& blk);
	MBlock(const Position& in, const Position& out, CBitmap* bmp);
	MBlock(const Position& in, const Position& out, const char* text);
	MBlock(const Position& in, const Position& out, CObList& ents);
	~MBlock();

	MBlock& operator = (const MBlock& rect);
	MEntity* Copy();

	int			GetType();
	void		Init();
	Position	GetImportPos();		// 返回Import的值
	Position	GetOutportPos();	// 返回Outport的值
	Position	GetLeftTopPos();		// 返回左上角的值
	Position	GetRightBottomPos();	// 返回右下角的值
	CBitmap* GetBitmap();
	void		SetBitmap(CBitmap* bmp);

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS MText
//
class MText : public MEntity
{
	DECLARE_SERIAL(MText)
protected:
	Position	m_LeftTop, m_RightBottom; // 起点和终点
	CString		m_Text;                     //文本内容
public:
	MText();
	MText(const MText& rect);
	MText(const Position& LeftTop, const Position& RightBottom, const char* text);
	~MText();

	MText& operator = (const MText& rect);
	MEntity* Copy();

	int			GetType();
	void		Init();
	Position	GetLeftTopPos();		// 返回左上角的值
	Position	GetRightBottomPos();	// 返回右下角的值
	CString		GetText();
	void		SetText(const char* text);

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

/////////////////
// Class CCircle
//
class CCircle : public MEntity
{
	DECLARE_SERIAL(CCircle)
protected:
	//member variables
	Position	m_center;
	double		m_dRadius;
public:
	// constructor and destructor
	CCircle();
	CCircle(const CCircle& circle);
	CCircle(const Position& center, const double& radius);
	CCircle(const Position& center, const Position& aroud);
	~CCircle();

	MEntity* Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int			GetType();
	void		Init(); // initialize member variables
	Position	GetCenterPos();
	double		GetRadius();

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

/////////////////
// Class MEllipse
//
class MEllipse : public MEntity
{
	DECLARE_SERIAL(MEllipse)
protected:
	//member variables
	Position	m_LeftTop;
	Position	m_RightBottom;
public:
	// constructor and destructor
	MEllipse();
	MEllipse(const MEllipse& elps);
	MEllipse(const Position& lt, const Position& rb);
	~MEllipse();

	MEntity* Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int	GetType();
	void Init(); // initialize member variables
	Position GetLTPos();
	Position GetRBPos();

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void GetBox(BOX2D* pBox);

	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& pos1, const Position& pos2);

	BOOL GetSnapPos(Position& pos);
	void LoadPmtCursor();

	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS CArc
//
class CArc : public MEntity
{
	DECLARE_SERIAL(CArc)
protected:
	//member variables
	Position	m_center;
	Position	m_begin;
	Position    m_end;

public:
	// constructor and destructor
	CArc();
	CArc(const CArc& arc);
	CArc(const Position& center, const Position& startPos, const Position& endPos);
	~CArc();

	MEntity* Copy();
	//-----------------------------------------------
	// member function
	// Attributes
	int			GetType();
	void		Init(); // initialize member variables
	Position	GetStartPos();
	Position	GetEndPos();
	Position	GetCenterPos();
	BOOL		GetSnapPos(Position& pos);

	void Draw(CDC* pDC, int drawMode = dmNormal);

	//对直线的编辑操作：拾取，平移，旋转，镜向和获得最小包围盒
	BOOL Pick(const Position& pos, const double pick_radius);
	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& FstPos, const Position& SecPos);
	void GetBox(BOX2D* pBox);

	void LoadPmtCursor();
	void Serialize(CArchive& ar);
};

////////////////////////////
// CLASS CComb
//
class CComb : public MEntity
{
	DECLARE_SERIAL(CComb)
protected:
	//member variables
	CString name;
	Position m_LeftTop, m_RightBottom;
	std::vector<MEntity *> entities;

public:
	CComb();
	CComb(const CComb& rect);
	CComb(const CString&name, const Position& m_LeftTop, const Position& m_RightBottom, const std::vector<MEntity*>& entities);
	~CComb();

	MEntity* Copy();

	int			GetType();
	CString		GetName();
	Position	GetLeftTop();
	Position	GetRightBottom();
	std::vector<MEntity*> GetEntities();
	void		Init();

	void Draw(CDC* pDC, int drawMode = dmNormal);
	BOOL Pick(const Position& pos, const double pick_radius);
	void Move(const Position& basePos, const Position& desPos, BOOL bTemp = FALSE);
	void Rotate(const Position& basePos, const double angle);
	void Mirror(const Position& FstPos, const Position& SecPos);
	void GetBox(BOX2D* pBox);
	void Serialize(CArchive& ar);
};
#endif