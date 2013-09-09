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

//#include <windef.h>

#pragma once

// struct for 3D point 
template<typename T>
struct POINT3D{
    T  x;
    T  y;
	T  z;
};

// struct for 3D point 
template<typename T>
struct POINT2D{
    T  x;
    T  y;
};

// video mode
enum VMODE {
	V_3D,   // 3D mode
	V_2D,   // 2D mode
	V_PROJ  // 3D mode (projections)
};

// used algorythm
enum AMODE {
	A_STD, // standart
	A_ALT  // alternative
};