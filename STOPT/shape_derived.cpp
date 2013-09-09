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

#include <cmath>
#include "shape_derived.h"
#include <cassert>

circle::circle(int x, int y, int r, int n)
:figure(x,y), r_(r), n_(n), ox_(x),oy_(y)
{
	assert(r>0);
}

circle::circle()
:figure(0,0), r_(2), n_(0), ox_(0),oy_(0)
{
	
}

circle& circle::operator =(const circle& r)
{
	ox_=r.ox_;
	oy_=r.oy_;
	n_=r.n_;
	r_=r.r_;
	col_=r.col_;
	c_=r.c_;

	return (*this);
}

void circle::draw(HDC hdc)const{
	HPEN pen=CreatePen(PS_SOLID,0,col_);
	HGDIOBJ old=SelectObject(hdc,pen);
	HGDIOBJ obr;
	LOGBRUSH lbrsh;
	lbrsh.lbColor=col_;
	lbrsh.lbStyle=BS_HOLLOW;
	HBRUSH brs=CreateBrushIndirect(&lbrsh);
	char bfr[8];
	int tlen;
	int rr=r_;
	
	SetBkMode(hdc, TRANSPARENT);
	obr=SelectObject(hdc,brs);
	::ZeroMemory(bfr,sizeof(bfr));
	_itoa(n_,bfr,10);
	tlen=strlen(bfr);
	::Ellipse(hdc, _x-rr, _y-rr, _x+rr, _y+rr);
	SelectObject(hdc,obr);
	DeleteObject(brs);
	brs=CreateSolidBrush(col_);
	::TextOut(hdc,_x-4,_y-18,(LPCSTR)bfr,tlen);
	obr=SelectObject(hdc,brs);
	::Ellipse(hdc,_x-2,_y-2,_x+2,_y+2);

	SelectObject(hdc,old);
	SelectObject(hdc,obr);
	DeleteObject(pen);
	DeleteObject(brs);
}

box::box(int x, int y,int dx, int dy)
:figure(x,y),dx_(dx),dy_(dy)
{}

void box::draw(HDC hdc)const{
	HPEN pen=CreatePen(PS_SOLID,0,col_);
	HGDIOBJ old=SelectObject(hdc,pen);

	//	POINT OldPos;
	::MoveToEx(hdc,_x,_y,NULL);
	::LineTo(hdc, _x+dx_, _y);
	::LineTo(hdc, _x+dx_, _y+dy_);
	::LineTo(hdc, _x, _y+dy_);
	::LineTo(hdc, _x, _y);

	SelectObject(hdc,old);
	DeleteObject(pen);
}