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

#include "analitycs3d.h"
#include "algom.h"

//находит кол-во блоков заданого радиуса вдоль заданой длинны
int NBlocks3d(int len, int r)
{
	int res=0;
	double a,n,p;
	p=n=0;

	a=(2.0*r)/sqrt(3.0);
	n=len/a;
	p=n-((int)n);
	
	if (p>0.07) res=(int)n + 1;
	else res=(int)n;
	
	return res;
}

//возвращает индекс елемента с оптимальным радиусом/стоимостью
int GetOptVariant3d(const POINT* var, int nvar, int l, int h, int w)
{
	int optvar,optcost,optsq;
	int curcnt,vcnt,hcnt,wcnt,curcost,cursq;

	hcnt=NBlocks3d(h,var[0].x);
	vcnt=NBlocks3d(l,var[0].x);
	wcnt=NBlocks3d(w,var[0].x);
	curcnt=hcnt*vcnt*wcnt;
	curcost=curcnt*var[0].y;
	cursq=EfficientVolume(l,h,w,var[0].x,nvar);
	optcost=curcost;
	optsq=cursq;
	optvar=0;

	for (int i=1; i<nvar; i++)
	{
		hcnt=NBlocks3d(h,var[i].x);
		vcnt=NBlocks3d(l,var[i].x);
		wcnt=NBlocks3d(w,var[i].x);
		curcnt=hcnt*vcnt*wcnt;
		curcost=curcnt*var[i].y;
		cursq=EfficientVolume(l,h,w,var[0].x,nvar);
		
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

//возвращает указатель на массив точек координат кругов
POINT3D* GetPoints3d(int h, int l, int w, int r, int* n)
{
	int nh=NBlocks3d(h,r);
	int nl=NBlocks3d(l,r);
	int nw=NBlocks3d(w,r);
	int nn=nh*nl*nw;

	float ds=0.f;

	if (GetAlgoMode()==A_ALT) ds=0.2;
	

	POINT3D* pt=(POINT3D*)VirtualAlloc(NULL,sizeof(POINT3D)*nn, MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);

	double a=l/(double)nl;
	double b=h/(double)nh;
	double c=w/(double)nw;
	//double rr=((sqrtf((a*a)+(b*b)+(c*c)))/2.0f)/sqrt(3.0f);
	float ry=a/2;//((sqrtf((a*a)+(b*b)))/2);//sqrt(2.0);
	float rx=b/2;//((sqrtf((a*a)+(c*c)))/2);//sqrt(2.0);
	float rz=c/2;//((sqrtf((c*c)+(b*b)))/2);//sqrt(2.0);

	int ii=0;
	for(int i=0; i<nh; i++)
		for(int j=0; j<nl; j++)
			for(int k=0; k<nw; k++)
		{	
			//ii=k*nw*nl+i*nl+j
			pt[ii].x=a*j+rx+ds;
			pt[ii].y=(-1)*(b*i+ry+ds);
			pt[ii].z=c*k+rz+ds;
			ii++;
		}

	*n=nn;
	return pt;
}

bool IsFilled(OglSphere* pC, int n, int l, int h, int w)
{
	bool ok=0;

	for (int i=0; i<l; i++)
	{
		for (int j=0; j<w; j++)
		{
			for (int k=0; k<n; k++)
			{
				ok=PntCovered3d(i,j,w,pC[k].get_x(),pC[k].get_y(),pC[k].get_z(),pC[k].get_radius());
				if (ok) break;
			}
			if(!ok) break;
		}
		if(!ok) break;
	}

	return ok;
}

bool PntCovered2d(int x, int y, int ox, int oy, int r)
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

bool PntCovered3d(int x, int y, int z, int ox, int oy, int oz, int r)
{
	bool ok=false;
	if(PntCovered2d(x,z,ox,oz,r))
	if(PntCovered2d(x,y,ox,oy,r))
	if(PntCovered2d(z,y,oz,oy,r))
		ok=true;

	return ok;
}

double OvrlVolume(OglSphere* pC, int n)
{
	double s=0;
	float a=0;
	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02;
	
	for (int i=0; i<n; i++)
	{
		s+=(3.0f/4.0f)*M_PI*powf(pC[i].get_radius()*k,3.0f);
	}

	return s;
}

double EfficientVolume(OglSphere* pC, int n, int l, int h, int w)
{
	double r=0;
	int bs=l*h*w;
	double os=OvrlVolume(pC, n);
	
	r=os/(double)bs;
	return r;
}

double EfficientVolume(int l, int h, int w, int r, int n)
{
	double res=0;
	int bs=l*h*w;

	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02;

	double os=((3.0f/4.0f)*M_PI*powf(r*k,3))*n;
	
	res=os/(double)bs;
	return r;
}