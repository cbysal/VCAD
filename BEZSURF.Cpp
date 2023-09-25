/*  Subroutine to calculate a \bzr surface (see Eq.(5.3))

    C code for An Introduction to NURBS
    by David F. Rogers. Copyright (C) 2000 David F. Rogers,
    All rights reserved.
    
    Name: bezsurf.c
    Language: C
    Subroutines called: none
    Book reference: p. 299
    
    b(,)  = array containing the polygon vertices
             b(,1) = x-components
             b(,2) = y-components
             b(,3) = z-components
             Note: Bi,j = b(,) has dimensions of n*m x 3 with j varying fastest
     Basis = function to calculate the Bernstein basis value
     Factrl= function to calculate the factorial of a number
     jin   = Bernstein basis function in the u direction (see Eq.(5.2))
     kjm   = Bernstein basis function in the w direction (see Eq.(5.2))
     m     = one less than the number of polygon vertices in w direction
     n     = one less than the number of polygon vertices in u direction
     Ni    = factorial function for the Bernstein basis
     p1    = number of parametric lines in the u direction
     p2    = number of parametric lines in the w direction
     q(,)  = position vectors for points on the surface
             q(,1) = x-components
             q(,2) = y-components
             q(,3) = z-components
                     for a fixed value of u the next m elements contain
                     the values for the curve q(u(sub i),w) q has dimensions
                     of p1*p2 x 3
*/

#include "stdafx.h"
#include <stdio.h>
#include <math.h>

/* function to calculate the factorial */

extern double factrl(int n);
// {
//     static int ntop=6;
//     static double a[33]={1.0,1.0,2.0,6.0,24.0,120.0,720.0}; /* fill in the first few values */
//     int j1;
// 
//     if (n < 0) printf("\nNegative factorial in routine FACTRL\n");
//     if (n > 32) printf("\nFactorial value too large in routine FACTRL\n");
// 
//     while (ntop < n) { /* use the precalulated value for n = 0....6 */
//         j1 = ntop++;
//         a[n]=a[j1]*ntop;
//     }
//     return a[n]; /* returns the value n! as a doubleing point number */
// }

/* function to calculate the factorial function for Bernstein basis */

extern double Ni(int n,int i);
// {
//     double ni;
//     ni = factrl(n)/(factrl(i)*factrl(n-i));
//     return ni;
// }

/* function to calculate the Bernstein basis */

extern double Basis(int n,int i,double t);
// {
//     double basis;
//     double ti; /* this is t^i */
//     double tni; /* this is (1 - t)^i */
// 
//     /* handle the special cases to avoid domain problem with pow */
// 
//     if (t==0. && i == 0) ti=1.0; else ti = pow(t,i);
//     if (n==i && t==1.) tni=1.0; else tni = pow((1-t),(n-i));
//     basis = Ni(n,i)*ti*tni; /* calculate Bernstein basis function */
//     return basis;
// }

/* Bezier surface subroutine */

void bezsurf(double b[], int n, int m, int p1, int p2, double q[])
{
    int i;
    int j;
    int k;
    int i1;
    int j1;
    int jbas;
    int uinc;
    int winc;
    int icount;
    int ch;

    double u;
    double w;
    double jin;
    double kjm;
    double stepu;
    double stepw;
    
    double factrl(int);
    double Ni(int,int);
    double Basis(int,int,double);

    icount = 1;
    stepu = 1.0/((double)(p1-1));
    stepw = 1.0/((double)(p2-1));

    u=0.0;
    
    for (uinc = 1; uinc <= p1; uinc++)
	{  /* for fixed u calculate various w's */
        if (1.0 - u < 5e-6) u=1.0; /* fix up the u = 1 value because of double */
        w = 0.0;
        for (winc = 1; winc <= p2; winc++)
		{
            if (1.0 - w < 5e-6) w=1.0; /* fix up the w = 1 value because of double */
            q[icount] = 0;      q[icount+1] = 0;    q[icount+2] = 0;
            for (i = 0; i <= n; i++)
			{
                jin = Basis(n,i,u); /* Bernstein basis function in the u direction (see Eq.(5.2)) */
                if (jin != 0.)
				{ /* don't bother no contribution */
                    jbas = 3*(m+1)*i; /* column index for lineal array*/
                    for (j = 0; j <= m; j++)
					{
                        kjm = Basis(m,j,w);  /* Bernstein basis function in the w direction (see Eq.(5.2)) */
                        if (kjm !=0.)
						{ /* don't bother no contribution */
                            j1 = jbas + 3*j + 1;
                            q[icount] = q[icount]+b[j1]*jin*kjm; /* calculate the surface points */
                            q[icount+1] = q[icount+1]+b[j1+1]*jin*kjm;
                            q[icount+2] = q[icount+2]+b[j1+2]*jin*kjm;
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
