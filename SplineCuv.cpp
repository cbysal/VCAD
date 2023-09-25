// CurveFun.cpp: implementation of the CCurveFun class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sysmath.h"

#include "math.h"

#define nrColumns 1

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------- GetContrl ----------------------------
// B-Spline: Use th algorithm as P172,
//		   suppose  Q(0)=Q(1), Q(n)=Q(n-1)
//	   So we can get the control point Q(i) from value point P(i).

#define MAXDETA 0.00001
// Notice: this subroutine is used for opened curve.
void GetBS3OpenControl(Vector2 *ValuePoint,Vector2 *ControlPoint,int PointNum)
{
	if(PointNum<4) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/6xQ(i-1)+2/3xQ(i)+1/6xQ(i+1) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=PointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[1];
	ControlPoint[PointNum+1]=ControlPoint[PointNum];
	
	// Step 2:Caculate deta(i)
	Vector2 deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta(0)=0.0;  deta(1)=0.0;
		maxdeta(0)=0.0;  maxdeta(1)=0.0;
		for(i=1;i<=PointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-(ControlPoint[i-1]+ControlPoint[i+1])*0.5)*0.5;
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[1];
		ControlPoint[PointNum+1]=ControlPoint[PointNum];
	}while((~maxdeta)>MAXDETA);
}

// Notice: this subroutine is used for closed curve.
void GetBS3ClosedControl(Vector2 *ValuePoint,Vector2 *ControlPoint,int PointNum)
{
	if(PointNum<4) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/6xQ(i-1)+2/3xQ(i)+1/6xQ(i+1) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=PointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[PointNum];
	ControlPoint[PointNum+1]=ControlPoint[1];
	//ControlPoint[PointNum+2]=ControlPoint[PointNum-1];
	ControlPoint[PointNum+2]=ControlPoint[2];
	
	// Step 2:Caculate deta(i)
	Vector2 deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta(0)=0.0;  deta(1)=0.0;
		maxdeta(0)=0.0;  maxdeta(1)=0.0;
		for(i=1;i<=PointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-(ControlPoint[i-1]+ControlPoint[i+1])*0.5)*0.5;
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[PointNum];
		ControlPoint[PointNum+1]=ControlPoint[1];
		//ControlPoint[PointNum+2]=ControlPoint[PointNum-1];
		ControlPoint[PointNum+2]=ControlPoint[2];
	}while((~maxdeta)>MAXDETA);
}





//------------------------------- GetContrl ----------------------------
// B-Spline2: Use th algorithm as P172,
//		   suppose  Q(0)=Q(1), Q(n)=Q(n-1)
//	   So we can get the control point Q(i) from value point P(i).

// Notice: this subroutine is used for opened curve.
void GetBS2OpenControl(Vector2 *ValuePoint,Vector2 *ControlPoint,int PointNum)
{
	if(PointNum<3) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/2xQ(i-1)+1/2xQ(i) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=PointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[1];
	// ControlPoint[PointNum+1]=ControlPoint[PointNum];
	
	// Step 2:Caculate deta(i)
	Vector2 deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta(0)=0.0;  deta(1)=0.0;
		maxdeta(0)=0.0;  maxdeta(1)=0.0;
		for(i=1;i<=PointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-ControlPoint[i-1]);
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[1];
		//ControlPoint[PointNum+1]=ControlPoint[PointNum];
	}while((~maxdeta)>MAXDETA);
}

// Notice: this subroutine is used for closed curve.
void GetBS2ClosedControl(Vector2 *ValuePoint,Vector2 *ControlPoint,int PointNum)
{
	if(PointNum<3) return;
	int i;
	
	// Use Gauss-Seidel algorithm to get value of equation:
	//     1/2xQ(i-1)+1/2xQ(i) = P(i)
	
	// Step 1:Initialization
	for(i=1;i<=PointNum;i++) ControlPoint[i]=ValuePoint[i-1];
	ControlPoint[0]=ControlPoint[PointNum];
	ControlPoint[PointNum+1]=ControlPoint[1];
	//ControlPoint[PointNum+2]=ControlPoint[PointNum-1];
	ControlPoint[PointNum+2]=ControlPoint[2];
	
	// Step 2:Caculate deta(i)
	Vector2 deta(0.0,0.0),maxdeta(0.0,0.0);
	do
	{
		deta(0)=0.0;  deta(1)=0.0;
		maxdeta(0)=0.0;  maxdeta(1)=0.0;
		for(i=1;i<=PointNum;i++)
		{
			deta=ValuePoint[i-1]-ControlPoint[i]+
				(ValuePoint[i-1]-ControlPoint[i-1]);
			ControlPoint[i]=ControlPoint[i]+deta;
			if((~deta)>(~maxdeta)) maxdeta=deta;
		}
		ControlPoint[0]=ControlPoint[PointNum];
		ControlPoint[PointNum+1]=ControlPoint[1];
		//ControlPoint[PointNum+2]=ControlPoint[PointNum-1];
		ControlPoint[PointNum+2]=ControlPoint[2];
	}while((~maxdeta)>MAXDETA);
}










//----------------------------------------------------------
//Filename  : Vector2.cpp
//Function  : Implement member function of class Vector2
//Date      : 1994.6
//Programmer: Yong Chen
//Note      : this class is only for 2 dimensions
//----------------------------------------------------------
Vector2::Vector2(double Value1,double Value2)
{
	values=new double [nrColumns];
	if(values==NULL)
	{ //printf("Error in allocate memory!");
		return;
	}
    values[0]=Value1;  values[1]=Value2;
}

Vector2::Vector2(void)
{
	values=new double [nrColumns];
	if(values==NULL)
	{ //printf("Error in allocate memory!");
		return;
	}
    values[0]=0.0;  values[1]=0.0;
}

Vector2::Vector2(const Vector2 &aVector2)
{
	values=new double [nrColumns];
	if(values==NULL)
	{ //printf("Error in allocate memory!");
		return;
	}
    for(int j=0; j<nrColumns;j++)
		values[j]=aVector2.values[j];
}

double &Vector2::operator() (int aColumn) const
{
    if(aColumn<0 || aColumn>=nrColumns || values==NULL)
    {
		//printf("column out of range or values is NULL!\n");
		return values[0];
    }
    return values[aColumn];
	
}

Vector2 &Vector2::operator= (const Vector2 &aVector2)
{
    for(int j=0; j<nrColumns; j++)
		values[j]=aVector2.values[j];
	return *this;
}

Vector2 Vector2::operator+ (const Vector2 &aVector2) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for addition\n");
	//	  exit(1);
	//	  }
	Vector2 result(*this);
	for(int j=0; j<nrColumns; j++)
		result.values[j]+=aVector2.values[j];
	return result;
}

Vector2 Vector2::operator- (const Vector2 &aVector2) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for subtraction\n");
	//	  exit(1);
	//	  }
	Vector2 result(*this);
	for(int j=0; j<nrColumns; j++)
		result.values[j]-=aVector2.values[j];
	return result;
}

double Vector2::operator* (const Vector2 &aVector2) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for multiply\n");
	//	  exit(1);
	//	  }
	double result=0.0;
	for(int j=0; j<nrColumns; j++)
		result+=values[j]*aVector2.values[j];
	return result;
}

Vector2 Vector2::operator* (const double num) const
{
	Vector2 result(0.0,0.0);
	for(int j=0;j<nrColumns;j++)
		result.values[j]=values[j]*num;
	return result;
}

Vector2 Vector2::operator/ (const double num) const
{
	//	if(fabs(num)<0.00001) { cout<<"Error in divide"; exit(1); }
	Vector2 result(0.0,0.0);
	for(int j=0;j<nrColumns;j++)
		result.values[j]=values[j]/num;
	return result;
}

double Vector2::operator~ (void) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for multiply\n");
	//	  exit(1);
	//	  }
	double result=0.0;
	for(int i=0;i<nrColumns;i++)
		result+=values[i]*values[i];
	return (sqrt(fabs(result)));
}

double Vector2::operator& (const Vector2 &aVector2) const
{
	//	if(nrColumns!=aVector2.nrColumns)
	//	  {  printf("Vector2 sizes incompatible for multiply\n");
	//	  exit(1);
	//	  }
	double result=0.0;
	for(int i=0;i<nrColumns;i++)
		result+=(values[i]-aVector2.values[i])*(values[i]-aVector2.values[i]);
	return (sqrt(fabs(result)));
}

void Vector2::Print(void) const
{
	//	printf("The Vector2 is:\n");
	//	if(values==NULL) printf("NULL\n");
	//	else
	//	{
	//	  for (int j=0;j<nrColumns;j++)
	//		 printf("%8.5f  ",values[j]);
	//	  printf("\n");
	//	}
}

//------------------------------BSPLINE 3----------------------
// Notice: The input must be 4 control points to draw the bspline.
void BSpline3(Vector2 *ControlPoint,Vector2 *OutPutPoint,int StartPoint,int CurveAccuracy,Vector2 *ValuePoint)
{
	double constant=1.0/6.0;
	Vector2 B[4];
	//	Vector2 OutPutPoint[DIFFERNUM+1];
	
	/* Caculate the constant for t*t*t --b[0] */
	B[0]=ControlPoint[StartPoint]*(-1.0*constant)+ControlPoint[StartPoint+1]*3.0*constant;
	B[0]=B[0]+ControlPoint[StartPoint+2]*(-3.0*constant)+ControlPoint[StartPoint+3]*constant;
	
	/* Caculate the constant for t*t --b[1]*/
	B[1]=ControlPoint[StartPoint+0]*0.5-ControlPoint[StartPoint+1]+ControlPoint[StartPoint+2]*0.5;
	
	/* Caculate the constant for t --b[2]*/
	B[2]=ControlPoint[StartPoint+0]*(-0.5)+ControlPoint[StartPoint+2]*0.5;
	
	/* Caculate the constant for 1 --b[3] */
	B[3]=ControlPoint[StartPoint+0]*constant+ControlPoint[StartPoint+1]*4.0*constant;
	B[3]=B[3]+ControlPoint[StartPoint+2]*constant;
	
	for(int i=0;i<=CurveAccuracy;i++)
	{
		double t1=i*1.0/CurveAccuracy;
		double t2=t1*t1;
		double t3=t2*t1;
		
		OutPutPoint[i]=B[0]*t3+B[1]*t2+B[2]*t1+B[3];
	}
	if( ValuePoint )
	{
		OutPutPoint[0] = ValuePoint[0];
		OutPutPoint[CurveAccuracy] = ValuePoint[1];
	}
}


//------------------------------BSPLINE 2----------------------
// Notice: The input must be 3 control points to draw the bspline.
void BSpline2(Vector2 *ControlPoint,Vector2 *OutPutPoint,int StartPoint,int CurveAccuracy,Vector2 *ValuePoint)
{
	//double constant=1.0/2.0;
	Vector2 B[3];
	//	Vector2 OutPutPoint[DIFFERNUM+1];
	
	/* Caculate the constant for t*t --b[0] */
	B[0]=ControlPoint[StartPoint]*0.5-ControlPoint[StartPoint+1]
		+ControlPoint[StartPoint+2]*0.5;
	
	/* Caculate the constant for t --b[1]*/
	B[1]=ControlPoint[StartPoint+0]*(-1)+ControlPoint[StartPoint+1];
	
	/* Caculate the constant for 1 --b[2]*/
	B[2]=ControlPoint[StartPoint+0]*0.5+ControlPoint[StartPoint+1]*0.5;
	
	for(int i=0;i<=CurveAccuracy;i++)
	{
		double t1=i*1.0/CurveAccuracy;
		double t2=t1*t1;
		
		OutPutPoint[i]=B[0]*t2+B[1]*t1+B[2];
	}
	if( ValuePoint )
	{
		OutPutPoint[0] = ValuePoint[0];
		OutPutPoint[CurveAccuracy] = ValuePoint[1];
	}
}



