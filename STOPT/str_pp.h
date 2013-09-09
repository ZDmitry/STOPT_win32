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
#include <iostream>
#include <stdlib.h> 

#pragma once

#pragma warning(disable: 4996)

char*   ftoa(double d, int count);
bool    GetWinFloat(HWND src, double* i);
bool	GetWinInt(HWND src, long* i);	
char*	GetTitle(const char* s1, const char* s2, int i);
char*	GetTitle(const char* s1, int i);
char*	GetTitle(const char* s1, const char* s2);
char*	GetTitle(const char* s1);