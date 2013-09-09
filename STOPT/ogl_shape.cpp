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

#include "ogl_shape.h"
#include <cassert>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

//OpenGL Headers Files
#include <gl\glut.h>

void glSphere(bool, GLfloat, GLfloat, GLfloat);
void glSphere(bool, GLfloat, GLfloat, GLfloat, GLfloat);
void glSphere(bool, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void glSphere(GLfloat, GLfloat, GLfloat, bool, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);

OglSphere::OglSphere(double x, double y, double z, double r)
{
	ox_=x; oy_=y; oz_=z;
	assert(r>0);
	r_=r;
}

OglSphere::OglSphere()
{
	ox_=oy_=oz_=0;
	r_=1;
}

OglSphere& OglSphere::operator =(const OglSphere& r)
{
	ox_=r.ox_;
	oy_=r.oy_;
	r_=r.r_;
	col_r=r.col_r;
	col_g=r.col_g;
	col_b=r.col_b;
	col_alp=r.col_alp;
	c_=r.c_;

	return (*this);
}

void OglSphere::set_color(float r, float g, float b)
{
	set_color(r,g,b,1.0f);
}

void OglSphere::set_color(float r, float g, float b, float a)
{
	col_r=r;
	col_g=g;
	col_b=b;
	col_alp=a;
}

void OglSphere::draw_center()
{
	glPushMatrix();
		glSphere(ox_, oy_, oz_, 0, r_/20, 0.9,0.9,0.9,1.f);
	glPopMatrix();
}

void OglSphere::draw()
{
	glPushMatrix();
		glSphere(ox_, oy_, oz_, 0, r_*1.16f, col_r, col_g, col_b, col_alp);
	glPopMatrix();
}


//SPHERE
void glSphere(bool bWired, GLfloat r, GLfloat g, GLfloat b)
{
	glSphere(bWired, r, g, b, 1.0f);
}

void glSphere(bool bWired, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glSphere(bWired, 1.0f, r, g, b, a);
}

void glSphere(bool bWired, GLfloat s, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glSphere(0.0f, 0.0f, 0.0f, bWired, s, r, g, b, a);
}

void glSphere(GLfloat x, GLfloat y, GLfloat z, bool bWired, GLfloat s, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glPushMatrix();
		//glScalef(s,s,s);
		glTranslatef(x,y,z);
		
		glColor4f(r,g,b,a);

		//glMaterialfv(GL_FRONT,GL_AMBIENT,make_vect(0.2473f, 0.1995f, 0.0745f, 1.0f));
		//glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,make_vect(r,g,b,1.f));
		//glMaterialfv(GL_FRONT,GL_SPECULAR,make_vect(0.6283f, 0.5558f, 0.3661f, 1.0f));
		//glMaterialfv(GL_FRONT,GL_EMISSION,make_vect(0.0f, 0.0f, 0.0f, 0.0f));
		//glMaterialf(GL_FRONT,GL_SHININESS,3.0f);

		if (bWired)
			glutWireSphere(s,60,60);
		else
			glutSolidSphere(s,60,60);
	glPopMatrix();
}