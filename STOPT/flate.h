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

#pragma once

#include "shape_derived.h"

#define _USE_MATH_DEFINES
#include <math.h>

BOOL	CreateFLATEWindow(HINSTANCE, HWND);
BOOL	KillFLATEWindow();
//BOOL	ReSizeFLATEScene(int, int);

bool	fltIsActive();
void	fltSetDimn(int l, int h, int w);
void	fltGetDimn(double& l, double& h, double& w);
int     fltGetVolume();
void    fltSetOptVar(int r);