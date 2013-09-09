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
#include "shape_derived.h"

#define _USE_MATH_DEFINES
#include <math.h>

#pragma once

#pragma warning(disable: 4244)

int    NBlocks(int len, int r);
POINT* GetPoints(int h, int l, int r, int* n);
POINT* GetPoints(int h, int l, int r, int sh, int* n);
int    GetOptVariant(const POINT* var, int nvar, int h, int l);
bool   IsFilled(circle* pC, int n, box* pB);
bool   PntCovered(int x, int y, int ox, int oy, int r);
double OvrlSquare(circle* pC, int n);
int    BoxSquare(box* bx);
double EfficientSquare(circle* pC, int n, box* pB);
double EfficientSquare(int l, int w, int r, int n);