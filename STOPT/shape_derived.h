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

#include "shape.h"

#pragma once

class circle:public figure{
public:
	circle();
	circle(int x, int y, int r,int n);
	void     set_number(int n){n_=n;};
	void     set_radius(int r){r_=r;};
	void     set_cost(int c){c_=c;};
	int      get_number(){return n_;};
	int      get_radius(){return r_;};
	int      get_cost(){return c_;};
	COLORREF get_color(){return col_;};
	int      get_x(){return ox_;};
	int      get_y(){return oy_;};
	virtual  void move_by(int x, int y){_x=ox_=x;_y=oy_=y;};
	void     shift_by(int x, int y){_x=ox_-x;_y=oy_-y;};
	virtual  void draw(HDC)const;
	circle&  circle::operator =(const circle& r);
private:
	int r_;
	int n_;
	int c_;
	int ox_,oy_;
};

class box:public figure{
public:
	box(int x, int y, int dx, int dy);
	void    resize(int dx, int dy){dx_=dx; dy_=dy;};
	int     length(){return dx_;};
	int     width(){return dy_;};
	virtual void move_by(int x, int y){_x=10-x; _y=10-y;};
	virtual void draw(HDC)const;
private:
	int dx_,dy_;
};