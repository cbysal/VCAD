#include <math.h>
#include <stdio.h>

#define  M_PI 3.1415926535897933
typedef  struct point3d_struct
{
   double x,y,z;
} locate;

//*************************************************************
//* Vector Class.
//*************************************************************
class Vector
{
public:
   double _xpos,_ypos,_zpos;
   Vector();                                 //constructure function
   Vector(double x,double y,double z);       //constructure function
   Vector(locate xyz);                       //constructure function
   Vector(const Vector &other);              //constructure function
   ~Vector();                                //destroy constructure function
   double x() const;                         //return x value
   Vector& x(double x);                      //modify x value
   double y() const;                         //return y value
   Vector& y(double y);                      //modify y value
   double z() const;                         //return z value
   Vector& z(double z);                      //modify z value
   Vector& operator = (const Vector &other);        // A = B
   locate operator () ();                           //return x,y,z value
   void operator () (locate p);                     //modify x,y,z value
   Vector& xyz(double x, double y, double z);       //modify x,y,z value
   Vector operator + (const Vector &other) const;   // A + B
   Vector operator - (const Vector &other) const;   // A - B
   Vector operator * (double scale) const;          // a * A
   Vector operator / (double scale) const;          // A / a
   Vector& operator -= (const Vector &point);       // A -= B
   Vector& operator += (const Vector &point);       // A += B
   Vector& operator *= (double scale);              // A *= a ==> A = a*A
   Vector& operator /= (double scale);              // A /= a ==> A = A/a
   double dot(Vector &point) const;                 // A.dot(B), dot multiple
   Vector mul(Vector &point) const;                 // A.mul(B), cross multiple
   void rotate(double ag1,double ag2,double ag3);   // A.rotate(a1,a2,a3)
   double length();                                 // A.length()
   void unit();                                     // A.unit()
};

class  Vector2{
  public:
    double *values;
  public:
    Vector2(double Value1,double Value2);
    Vector2(void);
    Vector2(const Vector2 &aVector);
    ~Vector2(void) {  delete values; }
    double &operator() (int aColumn) const;
    Vector2 &operator= (const Vector2 &aVector);
    Vector2 operator+ (const Vector2 &aVector) const;
    Vector2 operator- (const Vector2 &aVector) const;
    double operator* (const Vector2 &aVector) const;
    Vector2 operator* (const double num) const;
    Vector2 operator/ (const double num) const;
    // the distance of Vector
    double operator~ (void) const;
    // The distance of two Vector
    double operator& (const Vector2 &aVector) const;
    void Print (void) const;
};

//*****************************************
//define Metrix class                     *
//*****************************************
class Matrix
{
   int    _row, _col;
   double   **_val;
public:
   int  _flag;
   int  _halfwid;                        // half band width of the matrix
   Matrix();                             // A  or  A[n]
   Matrix(int row, int col);             // A(row,col) or A[n](row)
   ~Matrix();
   int isOK()   {  return _flag; };      // isOK() = 0 :result error
   double * operator [](int row) { return _val[row];};   // A[m][n]
   Matrix& operator = (Matrix& other);   // C =  A
   Matrix& operator += (Matrix &other);  // C += B      (C = C + B)
   Matrix& operator -= (Matrix &other);  // C -= B      (C = C - B)
   Matrix& operator *= (double scale);    // C *= t      (C = t * C)
   Matrix& operator /= (double scale);    // C /= t      (C = C / t)
   Matrix& unit();                       // A.unit()
   void    setmatrix(int row,int col);   // A.setmatrix(m,n)
   Matrix& rev(Matrix &other);           // C.rev(A)    result in C
   Matrix& trans(Matrix &other);         // C.tranf(A)  result in C
   Matrix& mul(Matrix &aa,Matrix &bb);   // C.mul(A,B)  result in C = A x B
};
