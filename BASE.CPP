#include "stdafx.h"
#include "math.h"

#include "base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	for class Position;
//
Position::Position() 
{
	x = y = 0.;
} 

Position::Position(double ox, double oy )
{
	x = ox, y = oy;
}
	
Position::Position(const double *pnt)
{
	x = pnt[0],	y = pnt[1];
}

Position::Position(const Position& positionSrc)
{
	x = positionSrc.x,	y = positionSrc.y ;
}


Position::~Position()
{
}
	
void Position::Init()
{
	x = y = 0.;
}

void Position::Set(double ox, double oy)
{
	x = ox, y = oy;
}

void Position::Set(const double *pnt)
{
	x = pnt[0], y = pnt[1];
}

//操作符重载
double&
Position::operator[](const  int& i)
{
	if(i==0) return x;
	else if(i==1) return y;
	else return x;
}

Position::operator const double *() const       // as a point arrsy
{
	return	&x;
}

Position::operator double *()       // as a point arrsy
{
	return	&x;
}

const 	Position& 
Position::operator=(const Position& positionSrc)
{
	x=positionSrc.x, y=positionSrc.y;
	return *this;
}

BOOL	operator==(const Position& positionSrc1,const Position& positionSrc2)
{
	Position	pnt1(positionSrc1),pnt2(positionSrc2);
	if(pnt1.Distance(pnt2) < DISTANCE_ZERO*100) return TRUE;
	return FALSE;
}

Position operator+(const Position& position1,const Position& position2)
{
	Position p(position1.x+position2.x, position1.y+position2.y);
	return p;
}

Position operator-(const Position& position1,const Position& position2)
{
	Position p(position1.x-position2.x, position1.y-position2.y);
	return p;
}

Position operator*(const Position& position, const double& scale)
{
	Position p(position.x*scale, position.y*scale);
	return p;
}

double Position::operator~ (void) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for multiply\n");
	//	  exit(1);
	//	  }
	double result=0.0;
	result = x*x + y*y;
	return (sqrt(fabs(result)));
}

Position operator*(const double& scale,	const Position& position)
{
	Position p(position.x*scale, position.y*scale);
	return p;
}
const	Position&
Position::operator+=(const Position& positionSrc)
{
	x+=positionSrc.x, y+=positionSrc.y;
	return *this;
}
	
const 	Position& 
Position::operator-=(const Position& positionSrc)
{
	x-=positionSrc.x, y-=positionSrc.y;
	return *this;
}

const	Position&
Position::operator*=(const double& scale)
{
	x *= scale, y *= scale;
	return *this;	
}
//计算两个位置类对象的距离
double Position::Distance(const Position& pnt)
{
	double dis = sqrt( (x-pnt.x)*(x-pnt.x) + (y-pnt.y)*(y-pnt.y));	
	return dis;
}
//比较两个位置类对象是否相等
BOOL Position::IsSame(const Position& pointSrc)
{
	if(Distance(pointSrc) < DISTANCE_ZERO*100)	return TRUE;
	return FALSE;
}
//判断是否在包围盒内
BOOL Position::IsInBox(const BOX2D& box)
{
	if( x > box.min[0] && x < box.max[0]
		&& y > box.min[1] && y < box.max[1] )
		return TRUE;
	return FALSE;
}
//实现位置类的存储
void Position::Serialize(CArchive &ar) 
{
	if(ar.IsStoring())
		ar << x << y ;
	else
		ar >> x >> y ;
}
//位置类对象的平移操作
Position Position::Offset(const double & deltax,const double & deltay)
{
	Position	pnt(x + deltax, y + deltay);
	return pnt;	
}
//位置类对象的平移操作
Position Position::Offset(const Position& off)
{
	Position	pnt(x + off.x, y + off.y);
	return pnt;
}
//位置类对象的旋转操作
Position Position::Rotate(const Position& basePos, const double angle)
{
	Position p;
	double cosv = cos(angle) ; // cos value
	double sinv = sin(angle) ;
	double xc = x ;
	double yc = y ;
	p.x = xc * cosv - yc * sinv + (1.- cosv) * basePos.x + basePos.y * sinv ;
	p.y = sinv * xc + cosv * yc + (1.- cosv) * basePos.y - sinv * basePos.x ;
	return p;
}
//位置类对象的镜向操作
Position Position::Mirror(const Position& pos1, const Position& pos2)
{
	Position	p1(pos1), p2(pos2);
	double	length = p1.Distance(p2);
	if(length <= DISTANCE_ZERO)
		return *this;

	double angle = ::GetAngleToXAxis(pos1, pos2);
	double cos_v = cos(angle); // cos value
	double sin_v = sin(angle);
	
	double t1 = 2. * cos_v * sin_v;
	double t2 = cos_v * cos_v - sin_v * sin_v;
	
	double desX = x*t2 + y*t1 + pos1.x*(-t2) - pos1.y*t1 + pos1.x;
	double dexY = x*t1 + y*(-t2) + pos1.y*t2 - pos1.x*t1 + pos1.y;

	Position des_pos(desX, dexY);
	return des_pos;
}
///////////////////////////////////////////////////
double GetAngleToXAxis(const Position& posSrc1,const Position& posSrc2)
{
	Position pos1(posSrc1),pos2(posSrc2);
	//计算两点的距离
	double len = pos1.Distance(pos2) ;
	if(len < DISTANCE_ZERO) 
		return	0 ;
	Position dp = pos2 - pos1;
	//与x轴夹角的余弦值和正弦值
	double cosv = dp.x/len ;
	double sinv = dp.y/len ;
	//通过反余弦函数，求得角度值
	if(sinv >= 0)
		return acos(cosv) ;
	else if(sinv < 0)
		return 2.*PI-acos(cosv) ;
	return 0 ;
}