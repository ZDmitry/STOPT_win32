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
#include <gl\glut.h>

#pragma once

#include "ogl_shape.h"

#pragma warning(disable: 4996)

#define _USE_MATH_DEFINES
#include <math.h>

int		InitGL(GLvoid);  
BOOL	CreateGLWindow(HWND, HINSTANCE, WNDPROC, LPCSTR, int, int, int);
GLvoid	KillGLWindow(GLvoid); 
GLvoid	ReSizeGLScene(GLsizei, GLsizei);
int		DrawGLScene(GLvoid);
HWND	GetOpenGLWindowHandle();
VOID	CALLBACK oglRotate(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void	oglDraw();
void	oglToggleRotate();
bool	oglIsActive();
void	oglSetDimn(double l, double h, double w);
void	oglGetDimn(double& l, double& h, double& w);
void    oglSetMagn(float m);
double	oglGetVolume(); 
void	oglSetPtr(OglSphere* c, int size);