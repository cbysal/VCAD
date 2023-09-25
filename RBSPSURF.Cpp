#include "stdafx.h"
#include <stdio.h>
/*
    Subroutine to generate a B-spline open knot vector with multiplicity
    equal to the order at the ends.
	
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/

extern void knot(int n, int c, int x[]);
/*{
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
*/
/*  Subroutine to generate B-spline basis functions for open knot vectors

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: basis.c
	Language: C
	Subroutines called: none
	Book reference: p. 279

    c        = order of the B-spline basis function
    d        = first term of the basis function recursion relation
    e        = second term of the basis function recursion relation
    npts     = number of defining polygon vertices
    n[]      = array containing the basis functions
               n[1] contains the basis function associated with B1 etc.
    nplusc   = constant -- npts + c -- maximum number of knot values
    t        = parameter value
    temp[]   = temporary array
    x[]      = knot vector
*/	

void basis(int c, double t, int npts, int x[], double n[])
{
	int nplusc;
	int i,k;
	double d,e;
	double temp[36];

	nplusc = npts + c;

    /* calculate the first order basis functions n[i][1]	*/
	for (i = 1; i<= nplusc-1; i++)
	{
    	if (( t >= x[i]) && (t < x[i+1]))
			temp[i] = 1;
	    else
			temp[i] = 0;
	}

	/* calculate the higher order basis functions */
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

	if (t == (double)x[nplusc])		/*    pick up last point	*/
 		temp[npts] = 1;

    /* put in n array	*/
	for (i = 1; i <= npts; i++) 
    	n[i] = temp[i];
}

/*  Subroutine to calculate the sum of the nonrational basis functions (see \eq {6--87}).

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: sumrbas.c
	Language: C
	Subroutines called: none
	Book reference: Chapter 7, Section 7.1, Alg. p.309
	
    b[]         = array containing the polygon net points
                    b[1] = x-component
                    b[2] = y-component
                    b[3] = z-component
                    b[4] = homogeneous coordinate weighting factor        
                    Note: Bi,j = b[] has dimensions of n*m x 4 with j varying fastest
                     The polygon net is n x m
    mbasis[,]  = array containing the nonrational basis functions for one value of w
    mpts       = the number of polygon vertices in w direction
    nbasis[,]  = array containing the nonrational basis functions for one value of u
    npts       = the number of polygon vertices in u direction
    sum        = sum of the basis functions
*/
double sumrbas(double b[], double nbasis[], double mbasis[], int npts, int mpts)
{

	int i,j,jbas,j1;
	double sum;
    /* calculate the sum */
	sum = 0;
	for (i = 1; i <= npts; i++)
	{
		if (nbasis[i] != 0.)
		{
			jbas = 4*mpts*(i-1);
		    for (j = 1; j <= mpts; j++)
			{
		    	if (mbasis[j] != 0.)
				{
		    	    j1 = jbas + 4*(j-1) + 4;
	    	    	sum = sum + b[j1]*nbasis[i]*mbasis[j];
				}
			}
		}
	}
	return(sum);
}

/*  Subroutine to calculate a Cartesian product rational B-spline
    surface using open uniform knot vectors (see Eq. (7.1)).

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: rbspsurf.c
	Language: C
	Subroutines called: knot.c, basis.c sumrbas.c
	Book reference: Chapter 7, Section 7.1, Alg. p. 308

    b[]         = array containing the polygon net points
                  b[1] = x-component
                  b[2] = y-component
                  b[3] = z-component
                  b[4] = h-component
                  Note: Bi,j = b[] has dimensions of n*m*3 with j varying fastest
                      The polygon net is n x m
    k           = order in the u direction
    l           = order in the w direction
    mbasis[]    = array containing the nonrational basis functions for one value of w (see Eq. (3.2))
    mpts        = the number of polygon vertices in w direction
    nbasis[]    = array containing the nonrational basis functions for one value of u (see Eq. (3.2))
    npts        = the number of polygon vertices in u direction
    p1          = number of parametric lines in the u direction
    p2          = number of parametric lines in the w direction
    q[]         = array containing the resulting surface
                  q[1] = x-component
                  q[2] = y-component
                  q[3] = z-component
                       for a fixed value of u the next m elements contain
                       the values for the curve q[u[sub i],w] q has dimensions
                       of p1*p2*3. The display surface is p1 x p2
*/

void rbspsurf(double b[], int k, int l, int npts, int mpts, int p1, int p2, double q[])
{

/*   allows for 20 data points with basis function of order 5 */

    int i,j,j1,jbas;
    int icount;
    int uinc,winc;
    int nplusc,mplusc;
    int x[30],y[30];
    int temp;

    double nbasis[30],mbasis[30];
    double pbasis;
    double h;
    double sum;
    double u,w;
    double stepu,stepw;
        
	/*    zero and redimension the arrays */
    nplusc = npts + k;
    mplusc = mpts + l;    

    for (i = 1; i <= nplusc; i++)
        x[i] = 0;
    for (i = 1; i <= mplusc; i++)
        y[i] = 0;
    for (i = 1; i <= npts; i++)
        nbasis[i] = 0.;
    for (i = 1; i <= mpts; i++)
        mbasis[i] = 0.;

    temp = 3*(p1*p2);

    for (i = 1; i <= 3*p1*p2; i++)
        q[i] = 0.;

	/*   generate the open uniform knot vectors */
    knot(npts,k,x);       /*  calculate u knot vector */
    knot(mpts,l,y);       /*  calculate w knot vector */
    icount = 1;

	/*    calculate the points on the B-spline surface */
    stepu = (double)x[nplusc]/(double)(p1-1);
    stepw = (double)y[mplusc]/(double)(p2-1);
    u = 0.;
    for (uinc = 1; uinc <= p1; uinc++)
	{
        if ((double)x[nplusc] - u < 5e-6)
            u = (double)x[nplusc];

        basis(k,u,npts,x,nbasis);    /* basis function for this value of u */
        w = 0.;
        for (winc = 1; winc <= p2; winc++)
		{
            if ((double)y[mplusc] - w < 5e-6)
                w = (double)y[mplusc];
           
            basis(l,w,mpts,y,mbasis);    /* basis function for this value of w */
            sum = sumrbas(b,nbasis,mbasis,npts,mpts);

            for (i = 1; i <= npts; i++)
			{
                if (nbasis[i] != 0.)
				{
					jbas = 4*mpts*(i-1);
					for (j = 1; j <= mpts; j++)
					{
						if (mbasis[j] != 0.)
						{
							j1 = jbas +4*(j-1) + 1;
							pbasis = b[j1+3]*nbasis[i]*mbasis[j]/sum;
							q[icount] = q[icount]+b[j1]*pbasis;  /* calculate surface point */
							q[icount+1] = q[icount+1]+b[j1+1]*pbasis;
							q[icount+2] = q[icount+2]+b[j1+2]*pbasis;
						}
					}
                }
            }
            icount = icount + 3;
            w = w + stepw;
        }
        u = u + stepu;
    }
}
