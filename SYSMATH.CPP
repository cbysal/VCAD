#include "stdafx.h"
#include "sysmath.h"

/*************************************************************/
/* Vector function.                                          */
/*************************************************************/
Vector::Vector()
{
   _xpos = 0;
   _ypos = 0;
   _zpos = 0;
}
/*-----------------------------------------------------------*/
Vector::Vector(double x,double y,double z)
{
   _xpos = x;
   _ypos = y;
   _zpos = z;
}
/*-----------------------------------------------------------*/
Vector::Vector(locate xyz)
{
   _xpos = xyz.x;
   _ypos = xyz.y;
   _zpos = xyz.z;
}
/*-----------------------------------------------------------*/
Vector::Vector(const Vector &other)
{
   _xpos = other.x();
   _ypos = other.y();
   _zpos = other.z();
}
/*-----------------------------------------------------------*/
Vector::~Vector()
{
}
/*-----------------------------------------------------------*/
double Vector::x() const
{
   return _xpos;
}
/*-----------------------------------------------------------*/
double Vector::y() const
{
   return _ypos;
}
/*-----------------------------------------------------------*/
 double Vector::z() const
{
   return _zpos;
}
/*-----------------------------------------------------------*/
Vector& Vector::x(double x)
{
   _xpos = x;
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::y(double y)
{
   _ypos = y;
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::z(double z)
{
   _zpos = z;
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::xyz(double x, double y, double z)
{
   _xpos = x;
   _ypos = y;
   _zpos = z;
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::operator = (const Vector &other)
{
   _xpos = other.x();
   _ypos = other.y();
   _zpos = other.z();
   return *this;
}
/*-----------------------------------------------------------*/
locate Vector::operator () ()
{
   locate p;
   p.x = _xpos;
   p.y = _ypos;
   p.z = _zpos;
   return p;
}
/*-----------------------------------------------------------*/
void Vector::operator () (locate p)
{
   _xpos = p.x;
   _ypos = p.y;
   _zpos = p.z;
}
/*-----------------------------------------------------------*/
Vector Vector::operator + (const Vector &other) const
{
   return Vector(_xpos + other.x(),_ypos + other.y(),_zpos + other.z());
}
/*-----------------------------------------------------------*/
Vector Vector::operator - (const Vector &other) const
{
   return Vector(_xpos - other.x(),_ypos - other.y(),_zpos - other.z());
}
/*-----------------------------------------------------------*/
Vector Vector::operator * (double scale) const
{
   return Vector(_xpos * scale,_ypos * scale,_zpos * scale);
}
/*-----------------------------------------------------------*/
Vector Vector::operator / (double scale) const
{
   return Vector(_xpos / scale,_ypos / scale,_zpos / scale);
}
/*-----------------------------------------------------------*/
Vector& Vector::operator -= (const Vector &point)
{
   _xpos -= point.x();
   _ypos -= point.y();
   _zpos -= point.z();
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::operator += (const Vector &point)
{
   _xpos += point.x();
   _ypos += point.y();
   _zpos += point.z();
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::operator *= (double scale)
{
   _xpos *= scale;
   _ypos *= scale;
   _zpos *= scale;
   return *this;
}
/*-----------------------------------------------------------*/
Vector& Vector::operator /= (double scale)
{
   _xpos /= scale;
   _ypos /= scale;
   _zpos /= scale;
   return *this;
}
/*-----------------------------------------------------------*/
double Vector::dot(Vector &point) const
{
   return _xpos * point.x() + _ypos * point.y() + _zpos * point.z();
}
/*-----------------------------------------------------------*/
Vector Vector::mul(Vector &point) const
{
   double xx,yy,zz;
   xx = _ypos * point.z() - _zpos * point.y();
   yy = _zpos * point.x() - _xpos * point.z();
   zz = _xpos * point.y() - _ypos * point.x();
   return Vector(xx,yy,zz);
}
/*-----------------------------------------------------------*/
void Vector::rotate(double ag1,double ag2,double ag3)//rot-order:X-axis(ag1),Y-axis(ag2),Z-axis(ag3)
{
   double dx,dy,dz;
   double pi = M_PI / 180.0;
   double cr1=cos(ag1*pi),sr1=sin(ag1*pi);
   double cr2=cos(ag2*pi),sr2=sin(ag2*pi);
   double cr3=cos(ag3*pi),sr3=sin(ag3*pi);

   dx = cr2*cr3*_xpos + cr2*sr3*_ypos - sr2*_zpos;
   dy = (sr1*sr2*cr3-cr1*sr3)*_xpos + (sr1*sr2*sr3+cr1*cr3)*_ypos + sr1*cr2*_zpos;
   dz = (cr1*sr2*cr3-sr1*sr3)*_xpos + (cr1*sr2*sr3-sr1*cr3)*_ypos + cr1*cr2*_zpos;
   _xpos = dx;  _ypos = dy;  _zpos = dz;
}
/*-----------------------------------------------------------*/
double Vector::length()
{
   double len;
   len = _xpos * _xpos + _ypos * _ypos + _zpos * _zpos;
   len = sqrt(len);
   return len;
}
/*-----------------------------------------------------------*/
void Vector::unit()
{
   double len;
   len = length();
   if(len < 1.0e-15)  return;
   _xpos /= len;
   _ypos /= len;
   _zpos /= len;
}
/*-----------------------------------------------------------*/

/*************************************************************/
/* Matrix function.                                          */
/*************************************************************/
/*-----------------------------------------------------------*/
Matrix::Matrix()
{
   _row = 0;   _col = 0, _flag = false;   _val = NULL;
}
//-----------------------------------------------
Matrix::Matrix(int row,int col)
{
   _row = row;   _col = col,  _flag = false;  _val = NULL;
   (*this).setmatrix(row,col);
}
/*-----------------------------------------------------------*/
void Matrix::setmatrix(int row,int col)
{
   int  i,j;
   if(row < 1)   row = 1;
   if(col < 1)   col = 1;
   _row = row;  _col = col;  _val = NULL;
   _val = new double *[_row];
   if(_val == NULL)       return;
   for(i =0; i < _row; i++)    _val[i] = NULL;
   for(i =0; i < _row; i++)
   {
     _val[i] = new double [_col];
     if(_val[i] == NULL)  return ;
   }
   for (i =0; i < _row; i++)
     for(j =0; j < _col; j++)
       _val[i][j] = (double)0.0;
   _flag = true;
}
//---------------------------------------------
Matrix::~Matrix()
{
   if(_val == NULL)  return;
     for(int i = 0; i < _row; i++)
     {
	if(_val[i])
	  { delete _val[i]; _val[i] = NULL;}
     }
     delete _val;  _val = NULL;
}
/*-----------------------------------------------------------*/
Matrix& Matrix::operator = (Matrix &other)
{
   int  i,j;
   if((_row == other._row)&&(_col == other._col))
   {    for(i=0; i < _row; i++)
	   for(j=0; j < _col; j++)
	     _val[i][j] = other._val[i][j];
	_flag = true;
   }
   else     _flag = false;
   return *this;
}
/*-----------------------------------------------------------*/
Matrix&  Matrix::operator += (Matrix &other)
{
   int  i,j;
   if((_row == other._row)&&(_col == other._col))
   {
      for(i=0; i < _row; i++)
	for(j=0; j < _col; j++)
	  _val[i][j] = _val[i][j] + other._val[i][j];
   }
   else     _flag = false;
   return *this;
}
/*-----------------------------------------------------------*/
Matrix&  Matrix::operator -= (Matrix &other)
{
   int  i,j;
   if((_row == other._row)&&(_col == other._col))
   {
      for(i=0; i < _row; i++)
      for(j=0; j < _col; j++)
	_val[i][j] = _val[i][j] - other._val[i][j];
   }
   else     _flag = false;
   return *this;
}
/*-----------------------------------------------------------*/
Matrix&  Matrix::operator *= (double scale)
{
    int  i,j;
    for(i=0; i < _row; i++)
      for(j=0; j < _col; j++)
	_val[i][j] = scale * _val[i][j];
    return *this;
}
/*-----------------------------------------------------------*/
Matrix&  Matrix::operator /= (double scale)
{
   int  i,j;
   if(fabs(scale) > 1.0e-10)
   {
      for(i=0; i < _row; i++)
      for(j=0; j < _col; j++)
	_val[i][j] = _val[i][j] / scale;
   }
   else     _flag = false;
   return *this;
}
/*-----------------------------------------------------------*/
Matrix& Matrix::unit()
{
   int  i,j;
   if(_row == _col)
   {
      for(i=0; i < _row; i++)
	 for(j=0; j < _col; j++)
	   _val[i][j] = (double)0.0;
      for( i = 0; i < _row; i++)
	_val[i][i] = (double)1.0;
   }
   else     _flag = false;
   return *this;
}
/*-----------------------------------------------------------*/
Matrix& Matrix::trans(Matrix &other)
{
   int i,j;
   if((_row != other._col) || (_col != other._row))
   { _flag = false;  return *this;}
   for (i = 0; i < _row; i++)
      for (j = 0; j < _col; j++)
	 _val[i][j] = other._val[j][i];
   return *this;
}
/*-----------------------------------------------------------*/
Matrix& Matrix::mul(Matrix &aa,Matrix &bb)
{
   int i,j,k;
   double dd;

   if(_row != aa._row)
      {   _flag = false;    return *this; }
   if(_col != bb._col)
      {   _flag = false;    return *this; }
   if(aa._col != bb._row)
      {   _flag = false;    return *this; }
   for (i = 0; i < _row; i++)
   for (j = 0; j < _col; j++)
   {    dd = 0.0;
	for (k = 0; k < aa._col; k++)
	    dd = dd + aa._val[i][k]*bb._val[k][j];
	_val[i][j] = dd;
   }
   return *this;
}
/*-----------------------------------------------------------*/
Matrix& Matrix::rev(Matrix &other)
{
  int    i,j,k,ik;
  int    *sequ;
  double  temp,maxa;

  if((_row != _col)||(other._row != other._col)||(_row != other._row))
  {
     _flag = false;
     return *this;
  }
  sequ = new int [_row];
  if(! sequ)   {  _flag = false;  return *this; }
  for (i = 0; i < _row; i++)
     for (j = 0; j < _row; j++)
       _val[i][j] = other._val[i][j];      //copy metrix
  for(k = 0; k < _row; k++)
  {
     ik=k;     maxa = _val[k][k];        //maxa: maximum element
     for(i=k;i<_row;i++)
     {
	temp = _val[i][k];
	if(fabs(maxa) < fabs(temp))
	{
	   maxa=temp;   ik=i;           //ik: maximum element row number
	}
     }
     sequ[k] = ik;                     //recording the sequence of var X
     if(ik!=k)
	for(j=0; j<_row; j++)
	{
	    temp= _val[k][j];_val[k][j]= _val[ik][j];
	    _val[ik][j]= temp;
	}
     if(fabs(maxa) < 1.0e-12)
     {
	_flag=false;     delete sequ;
	return *this;
     }
     _val[k][k] = 1.0/maxa;
     for(i=0; i<_row; i++)
	if(i!=k) _val[i][k] *= (-_val[k][k]);
     for(i=0; i<_row; i++)
	if(i!=k)
	   for(j=0; j<_row; j++)
	      if(j!=k) _val[i][j] += (_val[i][k]*_val[k][j]);
     for(j=0; j<_row; j++)
	if(j!=k) _val[k][j] *= _val[k][k];
  }
  for(k=_row-2; k>=0; k--)
  {
     j = sequ[k];
     if(j!=k)
	for(i=0; i<_row; i++)
	{
	    maxa = _val[i][k];
	    _val[i][k]= _val[i][j];_val[i][j] = maxa;
	}
  }
  delete sequ;   _flag = true;
  return *this;
}

