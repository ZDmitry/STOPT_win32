/*
    This file is part of STOPT project.

    Copyright 2012      by Dmitry Zagnoyko <hiroshidi@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/

#include "analitycs.h"
#include "algom.h"
#include "videom.h"

//находит кол-во блоков заданого радиуса вдоль заданой длинны
int NBlocks(int len, int r)
{
	int res=0;
	double a,n,p;
	p=n=0;

	a=(2.0*r)/sqrt(2.0);
	n=len/a;
	p=n-((int)n);
	
	if (p>0.07) res=(int)n + 1;
	else res=(int)n;
	
	return res;
}

//возвращает индекс елемента с оптимальным радиусом/стоимостью
int GetOptVariant(const POINT* var, int nvar, int h, int l)
{
	int optvar,optcost,optsq;
	int curcnt,vcnt,hcnt,curcost,cursq;

	hcnt=NBlocks(h,var[0].x);
	vcnt=NBlocks(l,var[0].x);
	curcnt=hcnt*vcnt;
	curcost=curcnt*var[0].y;
	cursq=EfficientSquare(l,h,var[0].x,nvar);
	optcost=curcost;
	optsq=cursq;
	optvar=0;

	for (int i=1; i<nvar; i++)
	{
		hcnt=NBlocks(h,var[i].x);
		vcnt=NBlocks(l,var[i].x);
		curcnt=hcnt*vcnt;
		curcost=curcnt*var[i].y;
		cursq=EfficientSquare(l,h,var[0].x,nvar);
		
		if (curcost<optcost)
		{
			optcost=curcost;
			optvar=i;
		}
		if (curcost==optcost)
		{
			if(cursq<optsq)
			{
				optsq=cursq;
				optvar=i;
			}
		}
	}

	return optvar;
}

POINT* GetPoints(int h, int l, int r, int* n)
{
	return GetPoints(h, l, r, 0, n);
}
//возвращает указатель на массив точек координат кругов
POINT* GetPoints(int h, int l, int r, int sh, int* n)
{
	int nh=NBlocks(h,r);
	int nl=NBlocks(l,r);
	int nn=nh*nl;

	int ds=0;

	if (GetAlgoMode()==A_ALT) ds=2;

	POINT* pt=new POINT[nn];

	float a=l/(float)nl;
	float b=h/(float)nh;
	float rx=a/2;
	float ry=b/2;

	if (GetVideoMode()==V_2D)
	{
		float rr=((sqrtf((a*a)+(b*b)))/2)/sqrt(2.0);
		rx=ry=rr;
	}

	for(int i=0; i<nh; i++)
		for(int j=0; j<nl; j++)
		{
			pt[i*nl+j].x=a*j+rx+sh+ds;
			pt[i*nl+j].y=b*i+ry+sh+ds;
		}

	*n=nn;
	return pt;
}

bool IsFilled(circle* pC, int n, box* pB)
{
	bool ok=0;

	int l=pB->length();
	int w=pB->width();

	for (int i=0; i<l; i++)
	{
		for (int j=0; j<w; j++)
		{
			for (int k=0; k<n; k++)
			{
				ok=PntCovered(i,j,pC[k].get_x(),pC[k].get_y(),pC[k].get_radius());
				if (ok) break;
			}
			if(!ok) break;
		}
		if(!ok) break;
	}

	return ok;
}

bool PntCovered(int x, int y, int ox, int oy, int r)
{
	bool ok;
	double a,rx,ry;
	a=0;
	ok=1;

	for (int i=0; i<360; i++)
	{
		a=(M_PI/180)*i;
		rx=(ox-10)+r*sin(a);
		ry=(oy-10)-r*cos(a);


		if (i<91)
		{
			if (x>rx && y<ry) 
			{
				ok=0;
				break;
			}
		}
		else if (i<181)
		{
			if (x>rx && y>ry) 
			{
				ok=0;
				break;
			}
		}
		else if (i<271)
		{
			if (x<rx && y>ry) 
			{
				ok=0;
				break;
			}
		}
		else 
		{
			if (x<rx && y<ry) 
			{
				ok=0;
				break;
			}
		}
	}

	return ok;
}

double OvrlSquare(circle* pC, int n)
{
	double s=0;
	float a=0;
	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02f;

	for (int i=0; i<n; i++)
	{
		s+=M_PI*powf(pC[i].get_radius()*k,2.0);
	}

	return s;
}

int BoxSquare(box* bx)
{
	int s=0;

	s=bx->length()*bx->width();

	return s;
}

double EfficientSquare(circle* pC, int n, box* pB)
{
	double r=0;
	int bs=BoxSquare(pB);
	double os=OvrlSquare(pC, n);
	
	r=os/(double)bs;
	return r;
}

double EfficientSquare(int l, int w, int r, int n)
{
	double res=0;
	int bs=l*w;
	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02f;

	double os=(M_PI*powf(r*k,2))*n;
	
	res=os/(double)bs;
	return r;
}