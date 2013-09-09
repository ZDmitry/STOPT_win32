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

#pragma once

// Windows Header Files:
#include <windows.h>


class OglSphere{
public:
	OglSphere();
	OglSphere(double x, double y, double z, double r);
	void     set_radius(double r){r_=r;};
	void     set_cost(int c){c_=c;};
	double   get_radius(){return r_;};
	int      get_cost(){return c_;};
	void	 set_color(float r, float g, float b);
	void	 set_color(float r, float g, float b, float a);
	float    get_r(){return col_r;};
	float    get_g(){return col_g;};
	float    get_b(){return col_b;};
	float    get_a(){return col_alp;};
	double   get_x(){return ox_;};
	double   get_y(){return oy_;};
	double   get_z(){return oz_;};
	void     move_to(double x, double y, double z){ox_=x; oy_=y; oz_=z;};
	void     draw();
	void     draw_center();
	OglSphere&  OglSphere::operator =(const OglSphere&);
private:
	double	 r_;
	float    col_r, col_g, col_b, col_alp;
	int		 c_;
	double	 ox_, oy_, oz_;
};