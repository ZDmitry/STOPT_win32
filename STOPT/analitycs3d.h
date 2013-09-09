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

#include <windows.h> 
#include "ogl_shape.h"

#define _USE_MATH_DEFINES
#include <math.h>

#pragma once

#pragma warning(disable: 4244)

typedef struct tagPOINT3D
{
    double  x;
    double  y;
	double  z;
} POINT3D, *PPOINT3D, NEAR *NPPOINT3D, FAR *LPPOINT3D;

int		 NBlocks3d(int len, int r);
POINT3D* GetPoints3d(int l, int h, int w, int r,  int* n);
int		 GetOptVariant3d(const POINT* var, int nvar, int l, int h, int w);
bool	 IsFilled(OglSphere* pC, int n, int l, int h, int w);
bool	 PntCovered3d(int x, int y, int z, int ox, int oy, int oz, int r);
double   OvrlVolume(OglSphere* pC, int n);
double	 EfficientVolume(OglSphere* pC, int n, int l, int h, int w);
double	 EfficientVolume(int l, int h, int w, int r, int n);