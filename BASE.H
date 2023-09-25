#ifndef _BASE_H_
#define _BASE_H_

// 如果距离小于DISTANCE_ZERO，则认为该距离为0
#ifndef DISTANCE_ZERO
#define DISTANCE_ZERO 1.e-6 
#endif
#define PI			3.14159
//定义包围盒
typedef struct _box2d BOX2D ;
struct _box2d
{
    double      min[2], max[2] ; // 定义最大和最小坐标值
} ;
//定义位置类
class Position	{
public:
	double 		x;
	double 		y;		
public:	
	Position();
	Position(double ox, double oy);
	Position(const double *pnt);
	Position(const Position& positionSrc);
	~Position();

	void 	Init(); // 初始化成员变量
	double&	  operator[](const  int& i);
	operator const double *() const;       
	operator double *();			       
	
	const 	Position& operator=(const Position& positionSrc);
	void	Set(double x, double y);
	void	Set(const double *pnt);
	
	friend	Position  operator+(const Position& position1,const Position& position2);
	friend	Position  operator-(const Position& position1,const Position& position2);
	friend	Position  operator*(const Position& position, const double& scale);
	friend	Position  operator*(const double& scale,	  const Position& position);

	const	Position& operator+=(const Position& positionSrc);
	const 	Position& operator-=(const Position& positionSrc);
	const	Position& operator*=(const double& scale);
    double operator~ (void) const;

	double	Distance(const Position& pnt);
	BOOL	IsSame(const Position& pointSrc);
	BOOL	IsInBox(const BOX2D& box);

	Position Offset(const double & deltax,const double & deltay);
	Position Offset(const Position& off);
	Position Rotate(const Position& basePos, const double angle) ;
	Position Mirror(const Position& pos1, const Position& pos2) ;

	void Serialize(CArchive &ar) ;
};

double GetAngleToXAxis(const Position& pos1,const Position& pos2);
#endif