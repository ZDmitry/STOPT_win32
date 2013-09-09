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
// STD
#include <stdlib.h> 
#include <iostream>
#include <windows.h>

// STL
#include <algorithm>

// STOPT
#include "str_pp.h"
#include "iodata.h"

using namespace std;

void IOSystem::saveToTxtFile(const vector<POINT2D<int>>* pPt, HWND hWnd)
{
	saveToTxtFile("points.txt", pPt, hWnd);
}

void IOSystem::saveToTxtFile(const char* file, const vector<POINT2D<int>>* pt, HWND hWnd)
{
	saveToTxtFile(file, pt, 0.f, hWnd);
}

void IOSystem::saveToTxtFile(const char* file, const vector<POINT2D<int>>* pt, double t, HWND hWnd)
{
	FILE* f=fopen(file,"w"); //points.txt
	// WARN: important to leave int i, counting should start from 0
	for (vector<POINT2D<int>>::const_iterator it = pt->begin(), int i=0; it != pt->end(); ++it, ++i)
		fprintf(f, "Point %d: X=%d, Y=%d\n", i, (*it).x, (*it).y);
	fprintf(f,"==========================================================\n");
	long x=0;
	StringSupport* ss = new StringSupport();
	ss->getWinInt(hWnd,&x);
	//if (inter)
	//	GetWinInt(hICX1Wnd,&x);
	//else 
	//	GetWinInt(hCY0Wnd,&x);
	fprintf(f,"Overall cost: %d \n",x);
	if (t!=0.f)
		fprintf(f,"Elapsed time: %f sec.",t);
	fclose(f);
}

void IOSystem::saveToTxtFile(const vector<POINT3D<double>>* pt, VMODE vm, HWND hWnd)
{
	saveToTxtFile("points.txt", pt, 0.f, 0.f, vm, hWnd);
}

void IOSystem::saveToTxtFile(const char* file, const vector<POINT3D<double>>* pt, VMODE vm, HWND hWnd)
{
	saveToTxtFile(file, pt, 0.f, 0.f, vm, hWnd);
}

void IOSystem::saveToTxtFile(const char* file, const vector<POINT3D<double>>* pt, double t, VMODE vm, HWND hWnd)
{
	saveToTxtFile(file, pt, t, 0.f, vm, hWnd);
}

void IOSystem::saveToTxtFile(const char* file, const vector<POINT3D<double>>* pt, double t, double tt, VMODE vm, HWND hWnd)
{
	int k=1;
	if (vm!=V_2D) k=(-1);

	FILE* f=fopen(file,"w");
	// WARN: important to leave int i, counting should start from 0
	for (vector<POINT3D<double>>::const_iterator it = pt->begin(), int i=0; it != pt->end(); ++it, ++i)
		fprintf(f, "Point %d: X=%f, Y=%f, Z=%f\n", i, (*it).x, (*it).y*k, (*it).z);
	fprintf(f,"==========================================================\n");
	long x=0;
	StringSupport* ss = new StringSupport();
	ss->getWinInt(hWnd,&x);
	//if (inter)
	//	GetWinInt(hICX1Wnd,&x);
	//else 
	//	GetWinInt(hCY0Wnd,&x);
	fprintf(f,"Overall count: %d \n",x);
	if (t!=0.f)
		fprintf(f,"Elapsed time: %f sec. \n",t);
	if (tt!=0.f)
		fprintf(f,"Elapsed time (1): %f sec.",tt);
	fclose(f);
}

void IOSystem::saveToEmfFile(OPENFILENAME &of, bool useBorder, const Figure* border, const vector<Figure>* pCircle)
{
	//of.lpstrFile = szFileName;
	//of.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetSaveFileName(&of))
	{
		HDC hdc=CreateEnhMetaFile(NULL,of.lpstrFile,NULL,NULL);

		if (useBorder) //bPt.x!=0)
			border->draw(hdc);
		if (pCircle != NULL)
			for_each(pCircle->begin(),pCircle->end(), [hdc](Figure f){ f.draw(hdc); });

		HENHMETAFILE hTemp=CloseEnhMetaFile(hdc);
        DeleteEnhMetaFile(hTemp);
	}
}