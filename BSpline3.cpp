#include "stdafx.h"
#include <stdio.h>
#include "base.h"
#include "math.h"
/*
    Subroutine to generate a B-spline open knot vector with multiplicity
    equal to the order at the ends.
	
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/

void knot(int n, int c, int x[])
{
	int nplusc,nplus2,i;

	nplusc = n + c;
	nplus2 = n + 2;

	x[1] = 0;
	for (i = 2; i <= nplusc; i++)
	{
	    if ( (i > c) && (i < nplus2) )
			x[i] = x[i-1] + 1;
	    else
			x[i] = x[i-1];
	}
}

/*
    Subroutine to generate a B-spline open uniform knot vector 	
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/
void knot1(int n, int c, int x[])
{
	int nplusc,nplus2,i;

	nplusc = n + c;

	x[1] = 0;
	for (i = 2; i <= nplusc; i++)
	   	x[i] = x[i-1] + 1;
}
/*  Subroutine to generate rational B-spline basis functions--open knot vector

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: rbais
	Language: C
	Subroutines called: none
	Book reference: Chapter 4, Sec. 4. , p 296

	c        = order of the B-spline basis function
    d        = first term of the basis function recursion relation
    e        = second term of the basis function recursion relation
	h[]	     = array containing the homogeneous weights
    npts     = number of defining polygon vertices
    nplusc   = constant -- npts + c -- maximum number of knot values
    r[]      = array containing the rationalbasis functions
               r[1] contains the basis function associated with B1 etc.
    t        = parameter value
    temp[]   = temporary array
    x[]      = knot vector
*/	

void rbasis(int c, double t, int npts, int x[] , double h[], double r[])
{
	int nplusc;
	int i,j,k;
	double d,e;
	double sum;
	double temp[36];

	nplusc = npts + c;

    //calculate the first order nonrational basis functions n[i]
	for (i = 1; i<= nplusc-1; i++)
	{
    	if (( t >= x[i]) && (t < x[i+1]))
			temp[i] = 1;
	    else
			temp[i] = 0;
	}

    //calculate the higher order nonrational basis functions
	for (k = 2; k <= c; k++)
	{
    	for (i = 1; i <= nplusc-k; i++)
		{
        	if (temp[i] != 0)    /* if the lower order basis function is zero skip the calculation */
           		d = ((t-x[i])*temp[i])/(x[i+k-1]-x[i]);
	        else
				d = 0;
    	    if (temp[i+1] != 0)     /* if the lower order basis function is zero skip the calculation */
        		e = ((x[i+k]-t)*temp[i+1])/(x[i+k]-x[i+1]);
	        else
    			e = 0;
    	    temp[i] = d + e;
		}
	}

	if (t == (double)x[nplusc])	//  pick up last point
 		temp[npts] = 1;

    //calculate sum for denominator of rational basis functions
	sum = 0.;
	for (i = 1; i <= npts; i++)
	    sum = sum + temp[i]*h[i];

    //form rational basis functions and put in r vector
	for (i = 1; i <= npts; i++)
	{
    	if (sum != 0)
        	r[i] = (temp[i]*h[i])/(sum);
		else
			r[i] = 0;
	}
}

/*  Subroutine to generate a rational B-spline curve using an open knot vector with multiplicity
    equal to the order at the ends.

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: rbspline.c
	Language: C
	Subroutines called: knot.c, rbasis.c, fmtmul.c
	Book reference: Chapter 4, Alg. p. 297

    b[]         : array containing the defining polygon vertices
                  b[1] contains the x-component of the vertex
                  b[2] contains the y-component of the vertex
                  b[3] contains the z-component of the vertex
	h[]			: array containing the homogeneous weighting factors 
    k           : order of the B-spline basis function = degree + 1
    nbasis      : array containing the basis functions for a single value of t
    nplusc      : number of knot values
    npts        : number of defining polygon vertices
    p[,]        : array containing the curve points
                  p[1] contains the x-component of the point
                  p[2] contains the y-component of the point
                  p[3] contains the z-component of the point
    p1          : number of points to be calculated on the curve
    t           : parameter value 0 <= t <= npts - k + 1
    x[]         : array containing the knot vector
*/

void rbspline(int npts, int k, int p1, double b[], double h[], double p[])
{
	int i,j,icount,jcount;
	int i1;
	int x[30];		/* allows for 20 data points with basis function of order 5 */
	int nplusc;

	double step;
	double t;
	double nbasis[20];
	double temp;


	nplusc = npts + k;

	//zero and redimension the knot vector and the basis array
	for(i = 0; i <= npts; i++)
		nbasis[i] = 0.;

	for(i = 0; i <= nplusc; i++)
		x[i] = 0.;
	
    //generate the uniform open knot vector
	knot(npts,k,x);

	icount = 0;

    //calculate the points on the rational B-spline curve
	t = 0;
	step = ((double)x[nplusc])/((double)(p1-1));

	for (i1 = 1; i1<= p1; i1++)
	{

		if ((double)x[nplusc] - t < 5e-6)
			t = (double)x[nplusc];
	
	    rbasis(k,t,npts,x,h,nbasis);      /* generate the basis function for this value of t */

		for (j = 1; j <= 3; j++)
		{      /* generate a point on the curve */
			jcount = j;
			p[icount+j] = 0.;

			for (i = 1; i <= npts; i++)
			{ /* Do local matrix multiplication */
				temp = nbasis[i]*b[jcount];
			    p[icount + j] = p[icount + j] + temp;
				jcount = jcount + 3;
			}
		}
    	icount = icount + 3;
		t = t + step;
	}
}

//generate a B-spline curve open with uniform knot vector
/*    b[]         : array containing the defining polygon vertices
                  b[1] contains the x-component of the vertex
                  b[2] contains the y-component of the vertex
                  b[3] contains the z-component of the vertex
	h[]			: array containing the homogeneous weighting factors 
    k           : order of the B-spline basis function = degree + 1
    nbasis      : array containing the basis functions for a single value of t
    nplusc      : number of knot values
    npts        : number of defining polygon vertices
    p[,]        : array containing the curve points
                  p[1] contains the x-component of the point
                  p[2] contains the y-component of the point
                  p[3] contains the z-component of the point
    p1          : number of points to be calculated on the curve
    t           : parameter value 0 <= t <= npts - k + 1
    x[]         : array containing the knot vector
*/
void rbspline1(int npts, int k, int p1, double b[], double h[], double p[])
{
	int i,j,icount,jcount;
	int i1;
	int x[30];		/* allows for 20 data points with basis function of order 5 */
	int nplusc;

	double step;
	double t;
	double nbasis[20];
	double temp;


	nplusc = npts + k;

	//zero and redimension the knot vector and the basis array
	for(i = 0; i <= npts; i++)
		nbasis[i] = 0.;

	for(i = 0; i <= nplusc; i++)
		x[i] = 0.;
	
    //generate the uniform open knot vector
//	knot1(npts,k,x);
	//draw circle
	x[1]=x[2]=x[3]=0;  x[4]=x[5]=1;  x[6]=x[7]=2;  x[8]=x[9]=3;  x[10]=x[11]=x[12]=4;

	icount = 0;

    //calculate the points on the rational B-spline curve
	t = (double)x[k];
	step = ((double)x[nplusc-k+1] - (double)x[k])/((double)(p1-1));

	for (i1 = 1; i1<= p1; i1++)
	{

		if ((double)x[nplusc-k+1] - t < 5e-6)
			t = (double)x[nplusc-k+1];
	
	    rbasis(k,t,npts,x,h,nbasis);      /* generate the basis function for this value of t */

		for (j = 1; j <= 3; j++)
		{      /* generate a point on the curve */
			jcount = j;
			p[icount+j] = 0.;

			for (i = 1; i <= npts; i++)
			{ /* Do local matrix multiplication */
				temp = nbasis[i]*b[jcount];
			    p[icount + j] = p[icount + j] + temp;
				jcount = jcount + 3;
			}
		}
    	icount = icount + 3;
		t = t + step;
	}
}

//Draw Circle
void DrawCircle()
{
		//draw RB-Spline curve
	int i;
	int npts,k,p1;

	double b[31];  /*control points: allows for up to 10  control vertices */
	double h[11];  /*knot points: allows for up to 10  control vertices */
	double p[301]; /*through points: allows for up to 100 points on curve */
/*
	npts = 5;
	k = 4;     // forth order, change for other orders : = degree+1
	p1 = 31;   // through points on curve 

	for (i = 1; i <= 3*npts; i++)
		b[i] = 0.;
	for (i=1; i <= npts; i++)
		h[i] = 1.0;     //all knot weight factors = 1
 	for (i = 1; i <= 3*p1; i++)
		p[i] = 0.;
	b[1]=100; 	b[2]=100; 	b[3]=200;   	b[4]=200; 	b[5]=300;	b[6]=200;
	b[7]=350; 	b[8]=100; 	b[9]=200; 	    b[10]=500;	b[11]=300; 	b[12]=200;
	b[13]=600;	b[14]=100;	b[15]=200;
*/
	//draw circle
	npts = 9;
	k = 3;     // third order, change for other orders : = degree+1
	p1 = 31;   // points number on curve 

	for (i = 1; i <= 3*npts; i++)
		b[i] = 0.;
	for (i=1; i <= npts; i++)
		h[i] = 1.0;     //all knot weight factors = 1
	h[2]=h[4]=h[6]=h[8]=0.707;
 	for (i = 1; i <= 3*p1; i++)
		p[i] = 0.;
	b[1]=400; 	b[2]=200; 	b[3]=200; 	b[4]=400; 	b[5]=300;	b[6]=200;
	b[7]=300; 	b[8]=300; 	b[9]=200; 	b[10]=200;	b[11]=300; 	b[12]=200;
	b[13]=200;	b[14]=200;	b[15]=200;  b[16]=200;	b[17]=100;	b[18]=200;
	b[19]=300;	b[20]=100;	b[21]=200;  b[22]=400;	b[23]=100;	b[24]=200;
	b[25]=400;	b[26]=200;	b[27]=200;
	
//	rbspline(npts,k,p1,b,h,p);
	rbspline1(npts,k,p1,b,h,p);
}


//均匀三次B样条曲线                                          (n-3)*num个点
void Draw_BSpline3(int n, Position *pts, int num, Position * ptOut)
{
	double A,B,C,D,t;

	for (int i=0; i<=n-4; i++)
	{
		for (int j=0; j<num; j++)
		{
			t = j*1.0/(num-1);
			A = pow((1-t), 3) * 1.0 / 6;
			B = (3* pow(t,3) - 6*pow(t,2) + 4) * 1 /6;
			C = (-3*pow(t,3) + 3 * pow(t,2) + 3*t + 1) * 1 / 6;
			D = pow(t,3) * 1.0/6;

			ptOut[i*num + j].x = A * pts[i].x + B*pts[i+1].x + C*pts[i+2].x + D*pts[i+3].x;
			ptOut[i*num + j].y = A * pts[i].y + B*pts[i+1].y + C*pts[i+2].y + D*pts[i+3].y;
		}
	}
}