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

#include "ARB_Multisample.h"
#include "ogle.h"
#include "algom.h"
#include "str_pp.h"

enum flat_orient{TOP,BOTTOM,UP,DOWN,LEFT,RIGHT};

void Cube(bool bWired);
void plane(GLfloat l, GLfloat h, GLfloat z, flat_orient fo,  bool bWired);

GLvoid	 print_msg(GLfloat, GLfloat, char*);
GLvoid	 print_msg(GLfloat, GLfloat, char*,   GLfloat);
GLfloat* make_vect(GLfloat, GLfloat, GLfloat, GLfloat);
GLfloat  GradToRad(GLfloat);

void glCube(bool, GLfloat, GLfloat, GLfloat);
void glCube(bool, GLfloat, GLfloat, GLfloat, GLfloat);
void glCube(bool, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void glCube(GLfloat, GLfloat, GLfloat, bool, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void glCube(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, bool, GLfloat, GLfloat, GLfloat, GLfloat);

void glBox(bool, GLfloat, GLfloat, GLfloat);
void glBox(bool, GLfloat, GLfloat, GLfloat, GLfloat);
void glBox(GLfloat, GLfloat, GLfloat, bool, GLfloat, GLfloat, GLfloat, GLfloat);
void glBox(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, bool, GLfloat, GLfloat, GLfloat, GLfloat);

HGLRC  hGRC=NULL;            // ���������� �������� ����������
HDC  hGDC=NULL;              // ��������� �������� ���������� GDI
HWND  hGWnd=NULL;            // ����� ����� �������� ���������� ����
HINSTANCE hGInst=NULL;

GLfloat angleV[]={0,0};
GLdouble viewpoint[3]={0,0,10};
GLfloat scale=1.0f;

GLdouble    L_BOX=1.f, H_BOX=1.f, W_BOX=1.f;
OglSphere*  C_SPHERE=NULL;
GLint       C_SIZE=0;
GLfloat     F_MAGN=.1f;

bool bRotate=true;

//1.0f
GLfloat Light_Ka[]= { .25f, .25f, .25f, 1.0f }; // �������� ambient �����
GLfloat Light_Kd[]= { .25f, .25f, .25f, 1.0f }; // �������� diffuse �����
GLfloat Light_Ks[]= { .25f, .25f, .25f, 1.0f }; // �������� specular �����
GLfloat Light_Kp[]= { 0.0f, 0.0f, 2.5f, 0.0f };// ������� ����� 

GLfloat LightA_Ka[]=  { 0.3f, 0.0f, 0.0f, 1.0f }; 
GLfloat LightA_Kd[]=  { 0.7f, 0.0f, 1.0f, 1.0f }; 
GLfloat LightA_Ks[]=  { 0.7f, 1.0f, 0.7f, 1.0f }; 
GLfloat LightA_Kp[]=  { 3.0f, 0.0f, 0.0f, 1.0f };  

GLfloat LightB_Ka[]=  { 0.5f, 0.0f, 0.2f, 1.0f }; 
GLfloat LightB_Kd[]=  { 0.7f, 1.0f, 0.0f, 1.0f }; 
GLfloat LightB_Ks[]=  { 0.0f, 1.0f, 0.0f, 1.0f }; 
GLfloat LightB_Kp[]=  { -10.0f, 0.0f, 0.0f, 1.0f }; 

HWND GetOpenGLWindowHandle()
{
	return hGWnd;
}

bool oglIsActive()
{
	return (hGWnd!=NULL);
}

void oglSetDimn(double l, double h, double w)
{
	L_BOX=l;
	H_BOX=h;
	W_BOX=w;
}

void oglGetDimn(double& l, double& h, double& w)
{
	l=L_BOX;
	h=H_BOX;
	w=W_BOX;
}

void oglSetMagn(float m)
{
	F_MAGN=m;
}

double	oglGetVolume()
{
	return (L_BOX*H_BOX*W_BOX);
}

void oglSetPtr(OglSphere* c, int size)
{
	C_SPHERE=c;
	C_SIZE=size;
}

BOOL CreateGLWindow(HWND prnt, HINSTANCE hInst,  WNDPROC WndProc,LPCSTR title, int width, int height, int bits)
{
	HWND     hwnd;
	GLuint   PixelFormat;              // ������ ��������� ����� ������

	DWORD    dwExStyle;              // ����������� ����� ����
	DWORD    dwStyle;              // ������� ����� ����
	
	RECT WindowRect;                // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;              // ���������� ����� ������������ � 0
	WindowRect.right=(long)width;              // ���������� ������ ������������ � Width
	WindowRect.top=(long)0;                // ���������� ������� ������������ � 0
	WindowRect.bottom=(long)height;              // ���������� ������ ������������ � Height

	WNDCLASS  wc;                // ��������� ������ ����
	wc.style    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;      // ���������� ��� ����������� � ������ ������� DC
	wc.lpfnWndProc    = WndProc;          // ��������� ��������� ���������
	wc.cbClsExtra    = 0;              // ��� �������������� ���������� ��� ����
	wc.cbWndExtra    = 0;              // ��� �������������� ���������� ��� ����
	wc.hInstance    = hInst;            // ������������� ����������
	wc.hIcon    = LoadIcon(NULL, IDI_WINLOGO);        // ��������� ������ �� ���������
	wc.hCursor    = LoadCursor(NULL, IDC_ARROW);        // ��������� ��������� �����
	wc.hbrBackground  = NULL;              // ��� �� ��������� ��� GL
	wc.lpszMenuName    = NULL;              // ���� � ���� �� �����
	wc.lpszClassName  = "OpenGL";            // ������������� ��� ������

	//ptC.x=0;
	//ptC.y=0;

	dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;      // ����������� ����� ����
	dwStyle    =   WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL;        // ������� ����� ����

	if( !RegisterClass( &wc ) )              // �������� ���������������� ����� ����
	{
		MessageBox( NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                // ����� � ����������� �������� �������� false
	}

	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );      // ��������� ���� ���������� �������

	if( !( hwnd = CreateWindowEx(  dwExStyle,          // ����������� ����� ��� ����
          "OpenGL",          // ��� ������
          title,            // ��������� ����
          WS_CLIPSIBLINGS |        // ��������� ����� ��� ����
          WS_CLIPCHILDREN |        // ��������� ����� ��� ����
          dwStyle,          // ���������� ����� ��� ����
          0, 0,            // ������� ����
		  width,    // ���������� ���������� ������
          height,    // ���������� ���������� ������
          prnt,            // Parent
          NULL,            // ��� ����
          hInst,          // ���������� ����������
          NULL ) ) )          // �� ������� ������ �� WM_CREATE (???)
	{
		KillGLWindow();                // ������������ �����
		MessageBox( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                // ������� false
	}

	hGWnd=hwnd;
	hGInst=hInst;
	
	static  PIXELFORMATDESCRIPTOR pfd=   // pfd �������� Windows ����� ����� ����� �� ����� ������� �������
	{
		sizeof(PIXELFORMATDESCRIPTOR),   // ������ ����������� ������� ������� ��������
		1,                               // ����� ������
		PFD_DRAW_TO_WINDOW |             // ������ ��� ����
		PFD_SUPPORT_OPENGL |             // ������ ��� OpenGL
		PFD_DOUBLEBUFFER,                // ������ ��� �������� ������
		PFD_TYPE_RGBA,                   // ��������� RGBA �����
		bits,                            // ���������� ��� ������� �����
		0, 0, 0, 0, 0, 0,                // ������������� �������� �����
		0,                               // ��� ������ ������������
		0,                               // ��������� ��� ������������
		0,                               // ��� ������ ����������
		0, 0, 0, 0,                      // ���� ���������� ������������
		32,                              // 32 ������ Z-����� (����� �������)
		0,                               // ��� ������ ���������
		0,                               // ��� ��������������� �������
		PFD_MAIN_PLANE,                  // ������� ���� ���������
		0,                               // ���������������
		0, 0, 0                          // ����� ���� ������������
	};

	if( !( hGDC = GetDC( hwnd ) ) )       // ����� �� �� �������� �������� ����������?
	{
		KillGLWindow();                  // ������������ �����
		MessageBox( NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                    // ������� false
	}

	if(!arbMultisampleSupported)
	{
		if( !( PixelFormat = ChoosePixelFormat( hGDC, &pfd ) ) )        // ������ �� ���������� ������ �������?
		{
			KillGLWindow();                                            // ������������ �����
			MessageBox( NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return false;                                              // ������� false
		}

	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}

	if( !SetPixelFormat( hGDC, PixelFormat, &pfd ) )   // �������� �� ���������� ������ �������?
	{
		KillGLWindow();                               // ������������ �����
		MessageBox( NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                                 // ������� false
	}

	if( !( hGRC = wglCreateContext( hGDC ) ) )          // �������� �� ���������� �������� ����������?
	{
		KillGLWindow();                               // ������������ �����
		MessageBox( NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;                                 // ������� false
	}

	if( !wglMakeCurrent( hGDC, hGRC ) )  // ����������� ������������ �������� ����������
	{
		KillGLWindow();                // ������������ �����
		MessageBox( NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                  // ������� false
	}

	if(!arbMultisampleSupported && CHECK_FOR_MULTISAMPLE)
	{
	
		if(InitMultisample(wc.hInstance,hwnd,pfd))
		{
			KillGLWindow();
			return CreateGLWindow(prnt, hInst, WndProc, title, width, height, 32);
		}
	}

	ShowWindow( hwnd, SW_SHOW );              // �������� ����
	SetForegroundWindow( hwnd );              // ������ ������� ���������
	SetFocus( hwnd );                         // ���������� ����� ���������� �� ���� ����
	ReSizeGLScene( width, height );           // �������� ����������� ��� ������ OpenGL ������.

	if( !InitGL() )                           // ������������� ������ ��� ���������� ����
	{
		KillGLWindow();                       // ������������ �����
		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                         // ������� false
	}

	return true;                              // �� � �������!
}

GLvoid ReSizeGLScene( GLsizei width, GLsizei height )        // �������� ������ � ���������������� ���� GL 
{
	if( height == 0 )              // �������������� ������� �� ���� 
	{
	   height = 1;
	}
    
	glViewport( 0, 0, width, height );  // ����� ������� ������� ������ 
	glMatrixMode( GL_PROJECTION );      // ����� ������� ��������
    glLoadIdentity();					// ����� ������� �������� 

    // ���������� ����������� �������������� �������� ��� ����
    gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f );

	
	/*if(width<=height)
		glOrtho(-20,20,-20*(GLfloat)height/(GLfloat)width,20.0*(GLfloat)height/(GLfloat)width,-20,20);
	else
		glOrtho(-20*(GLfloat)width/(GLfloat)height,20.0*(GLfloat)width/(GLfloat)height,-20,20,-20,20);*/
	

    glMatrixMode( GL_MODELVIEW );            // ����� ������� ���� ������
    glLoadIdentity();              // ����� ������� ���� ������
}

int DrawGLScene( GLvoid )                // ����� ����� ����������� ��� ����������
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );      // �������� ����� � ����� �������
	glEnable(GL_COLOR_MATERIAL);
	
	//FSAA - �����������
	glEnable(GL_MULTISAMPLE_ARB);
	
	//ALPHA
	glEnable(GL_ALPHA_TEST);
  
	//BLENDING
	glEnable(GL_BLEND);

	//Most effective func are:
	//GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
	//GL_SRC_ALPHA_SATURATE, GL_ONE
	//GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA
	//GL_ONE,GL_ONE - best fit with light
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

 
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();              // �������� ������� �������
	//-5.5, 3.9
	print_msg(-5.5,-4,"x", F_MAGN*10);

	viewpoint[2]=10*cos(angleV[0])*cos(angleV[1]);
	viewpoint[0]=10*sin(angleV[0])*cos(angleV[1]);
	viewpoint[1]=10*sin(angleV[1]);
	gluLookAt(viewpoint[0],viewpoint[1],viewpoint[2],0,0,0,0,cos(angleV[1]),0);

	glPushMatrix();
		glScaled(F_MAGN,F_MAGN,F_MAGN);

		if (C_SPHERE!=NULL && C_SIZE>0)
		{
			glPushMatrix();
				//double r=C_SPHERE[0].get_radius();
				glTranslated(-L_BOX/2,H_BOX/2,-W_BOX/2);
				for(int i=0; i<C_SIZE; i++)
					C_SPHERE[i].draw_center();
			glPopMatrix();
		}

		glBox(L_BOX,H_BOX,W_BOX,0,.11,.12,.13,.6f);

		if (C_SPHERE!=NULL && C_SIZE>0)
		{
			glPushMatrix();
				//double r=C_SPHERE[0].get_radius();
				glTranslated(-L_BOX/2,H_BOX/2,-W_BOX/2);
				for(int i=0; i<C_SIZE; i++)
					C_SPHERE[i].draw();
			glPopMatrix();
		}
	glPopMatrix();
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE_ARB);
	glDisable(GL_COLOR_MATERIAL);
	
	glFlush();
	return true;                // ���������� ������ �������
}

GLvoid KillGLWindow( GLvoid )              // ���������� ���������� ����
{
	if( hGRC )                // ���������� �� �������� ����������?
    {
		if( !wglMakeCurrent( NULL, NULL ) )        // �������� �� ���������� RC � DC?
		{
			MessageBox( NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		if( !wglDeleteContext( hGRC ) )        // �������� �� ������� RC?
		{
			MessageBox( NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		hGRC = NULL;              // ���������� RC � NULL
    }
	
	if( hGDC && !ReleaseDC( hGWnd, hGDC ) )          // �������� �� ���������� DC?
	{
		MessageBox( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGDC=NULL;                // ���������� DC � NULL
	}
	

	if(hGWnd && !DestroyWindow(hGWnd))            // �������� �� ���������� ����?
	{
		MessageBox( NULL, "Could Not Release hGWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGWnd = NULL;                // ���������� hGWnd � NULL
	}

	hGWnd=NULL;
	
	if( !UnregisterClass( "OpenGL", hGInst ) )        // �������� �� ����������������� �����
	{	
		
		MessageBox( NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		//hGInst = NULL;                // ���������� hInstance � NULL
	}

	//hGInst=NULL;

	if (C_SPHERE!=NULL)
	{
		VirtualFree(C_SPHERE,0,MEM_RELEASE);
		C_SPHERE=NULL;
		C_SIZE=0;
	}
}

int InitGL( GLvoid )                // ��� ��������� ������� OpenGL ���������� �����
{
	//glShadeModel( GL_SMOOTH );            // ��������� ������� �������� �����������
	
	//LoadTex();        // ��������� ���������
	//glEnable(GL_TEXTURE_2D);		// ���������� ��������� ��������

	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);          // ������� ������ � ������ ����
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glClearDepth( 1.0f );              // ��������� ������� ������ �������
    glEnable( GL_DEPTH_TEST );            // ��������� ���� �������
    glDepthFunc( GL_LEQUAL );            // ��� ����� �������
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );      // ��������� � ���������� �����������
	glHint( GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, Light_Ka);    // ��������� �������� �����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Kd);    // ��������� ���������� �����
	glLightfv(GL_LIGHT1, GL_SPECULAR, Light_Ks);    // ��������� ����������� �����
	glLightfv(GL_LIGHT1, GL_POSITION, Light_Kp);   // ������� �����
	glEnable(GL_LIGHT1); // ���������� ��������� ����� ����� 1

	glLightfv(GL_LIGHT2, GL_AMBIENT, LightA_Ka);    // ��������� �������� �����
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightA_Kd);    // ��������� ���������� �����
	glLightfv(GL_LIGHT2, GL_SPECULAR, LightA_Ks);    // ��������� ����������� �����
	glLightfv(GL_LIGHT2, GL_POSITION, LightA_Kp);   // ������� �����
	//glEnable(GL_LIGHT2); // ���������� ��������� ����� ����� 2

	glLightfv(GL_LIGHT3, GL_AMBIENT, LightB_Ka);    // ��������� �������� �����
	glLightfv(GL_LIGHT3, GL_DIFFUSE, LightB_Kd);    // ��������� ���������� �����
	glLightfv(GL_LIGHT3, GL_SPECULAR, LightB_Ks);    // ��������� ����������� �����
	glLightfv(GL_LIGHT3, GL_POSITION, LightB_Kp);   // ������� �����
	//glEnable(GL_LIGHT3); // ���������� ��������� ����� ����� 3

	glEnable(GL_LIGHTING);
	
	return true;                // ������������� ������ �������
}

VOID CALLBACK oglRotate(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	int idx=0;
	if (bRotate)
	{
		angleV[idx]+=0.035f;
		if(angleV[idx]>=2*M_PI) 
			angleV[idx]-=2*M_PI;
	}
}

void oglDraw()
{
	DrawGLScene();        // ������ �����
    SwapBuffers(hGDC);    // ������ ����� (������� �����������)
}

void oglToggleRotate()
{
	bRotate=!bRotate;
}

GLfloat* make_vect(GLfloat a,GLfloat b, GLfloat c, GLfloat d)
{
	GLfloat* rfm=new GLfloat[4];

	rfm[0]=a; rfm[1]=b; rfm[2]=c; rfm[3]=d;

	return rfm;
}

GLfloat GradToRad(GLfloat a)
{
	return (a * M_PI / 180); // dgr -> rad
}


//CUBE

void glCube(bool bWired, GLfloat r, GLfloat g, GLfloat b)
{
	glCube(bWired, r, g, b, 1.0f);
}

void glCube(bool bWired, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glCube(bWired, 1.0f, r, g, b, a);
}

void glCube(bool bWired, GLfloat s, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glCube(0.0f, 0.0f, 0.0f, bWired, s, r, g, b, a); 
}

void glCube(GLfloat x, GLfloat y, GLfloat z, bool bWired, GLfloat s,
			  GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glCube(x, y, z, s, 0.0f, 0.0f, 0.0f, bWired, r, g, b, a); 
}

void glCube(GLfloat x, GLfloat y, GLfloat z, GLfloat s,
			  GLfloat c, GLfloat k, GLfloat t, bool bWired,
			  GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glPushMatrix();
		//glScalef(s,s,s);
		glRotatef(c,1,0,0);
		glRotatef(k,0,1,0);
		glRotatef(t,0,0,1);
		glTranslatef(x,y,z);
		
		glColor4f(r,g,b,a);

		//glMaterialfv(GL_FRONT,GL_AMBIENT,make_vect(0.2473f, 0.1995f, 0.0745f, 1.0f));
		//glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,make_vect(r, g, b, 1.f));
		//glMaterialfv(GL_FRONT,GL_SPECULAR,make_vect(0.6283f, 0.5558f, 0.3661f, 1.0f));
		//glMaterialfv(GL_FRONT,GL_EMISSION,make_vect(0.0f, 0.0f, 0.0f, 0.0f));
		//glMaterialf(GL_FRONT,GL_SHININESS,3.0f);
		
		Cube(bWired);

		/*if (bWired)
			glutWireCube(s);
		else
			glutSolidCube(s);*/
	glPopMatrix();
}

void Cube(bool bWired)
{
	if (bWired)
		glBegin(GL_LINES);//_STRIP); 
	else
		glBegin(GL_QUADS);

		// �������� �����
		glNormal3f( 0.0f, 0.0f, 1.0f);   // ������� ��������� �� �����������
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // ����� 1 (�����) 
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // ����� 2 (�����)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // ����� 3 (�����)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // ����� 4 (�����)
		if (bWired)
		{
			glVertex3f( 1.0f, 1.0f,  -1.0f); 
			glVertex3f( -1.0f, 1.0f,  -1.0f); 
		}


		// ������ �����
		//glColor4f(0,1,0,TRANSPARENSY);
		glNormal3f( 0.0f, 0.0f,-1.0f);   // ������� ��������� �� �����������
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // ����� 1 (���)
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // ����� 2 (���)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // ����� 3 (���)
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // ����� 4 (���)
		if (bWired)
		{
			glVertex3f( -1.0f,  1.0f,  1.0f); 
			glVertex3f( -1.0f, -1.0f,  1.0f); 
		}

		// ������� �����
		//glColor4f(0,1,1,TRANSPARENSY);
		glNormal3f( 0.0f,  1.0f, 0.0f);   // ������� ��������� �����
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // ����� 1 (����)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // ����� 2 (����)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // ����� 3 (����)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // ����� 4 (����)
		if (bWired)
		{
			glVertex3f( 1.0f, -1.0f,  -1.0f); 
			glVertex3f( 1.0f, -1.0f,  1.0f); 
		}

		// ������ �����
		//glColor4f(1,0,0,TRANSPARENSY);
		glNormal3f( 0.0f, -1.0f,  0.0f); // ������� ��������� ����
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // ����� 1 (���)
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // ����� 2 (���)
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // ����� 3 (���)
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // ����� 4 (���)

		// ������ �����
		//glColor4f(1,0,1,TRANSPARENSY);
		glNormal3f( 1.0f,  0.0f,  0.0f); // ������� ��������� ������
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // ����� 1 (�����)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // ����� 2 (�����)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // ����� 3 (�����)
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // ����� 4 (�����)

		// ����� �����
		//glColor4f(1,1,0,TRANSPARENSY);
		glNormal3f(-1.0f, 0.0f, 0.0f);   // ������� ��������� �����
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // ����� 1 (����)
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // ����� 2 (����)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // ����� 3 (����)
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // ����� 4 (����)
	glEnd();
}


//BOX

void glBox(bool bWired, GLfloat r, GLfloat g, GLfloat b)
{
	glBox(bWired,r,g,b,1.f);
}

void glBox(bool bWired, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glBox(1.f,1.f,1.f,bWired,r,g,b,a);
}

void glBox(GLfloat l, GLfloat h, GLfloat w, bool bWired,
		   GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glBox(l,h,w,0.f,0.f,0.f,bWired,r,g,b,a);
}

void glBox(GLfloat l, GLfloat h, GLfloat w,
		   GLfloat c, GLfloat k, GLfloat t, bool bWired,
		   GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glPushMatrix();
		//glScalef(s,s,s);
		glRotatef(c,1,0,0);
		glRotatef(k,0,1,0);
		glRotatef(t,0,0,1);
		//glTranslatef(x,y,z);
		
		glColor4f(r,g,b,a);

		//glMaterialfv(GL_FRONT,GL_AMBIENT,make_vect(0.2473f, 0.1995f, 0.0745f, 1.0f));
		//glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,make_vect(r, g, b, 1.f));
		//glMaterialfv(GL_FRONT,GL_SPECULAR,make_vect(0.6283f, 0.5558f, 0.3661f, 1.0f));
		//glMaterialfv(GL_FRONT,GL_EMISSION,make_vect(0.0f, 0.0f, 0.0f, 0.0f));
		//glMaterialf(GL_FRONT,GL_SHININESS,3.0f);
	
		//TOP
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f( 0.0f, 0.0f, 1.0f);  
			glNormal3f(-1.0f, -1.0f,  1.0f);
			glVertex3d(-l/2, -h/2, w/2);
			glNormal3f( 1.0f, -1.0f,  1.0f);
			glVertex3d( l/2, -h/2, w/2);
			glNormal3f( 1.0f, -1.0f,  1.0f);
			glVertex3d( l/2,  h/2, w/2);
			glNormal3f(-1.0f, 1.0f,  1.0f);
			glVertex3d(-l/2,  h/2, w/2);
		glEnd();
		
		//LEFT
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f(-1.0f, 0.0f, 0.0f);  
			glNormal3f(-1.0f, -1.0f, -1.0f);
			glVertex3d(-l/2, -h/2, -w/2);
			glNormal3f(-1.0f, -1.0f, 1.0f);
			glVertex3d(-l/2, -h/2,  w/2);
			glNormal3f(-1.0f, 1.0f, 1.0f);
			glVertex3d(-l/2,  h/2,  w/2);
			glNormal3f(-1.0f, 1.0f, -1.0f);
			glVertex3d(-l/2,  h/2, -w/2);
		glEnd();
		
		//BOTTOM
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f( 0.0f, 0.0f,-1.0f);  
			glNormal3f(-1.0f, -1.0f, -1.0f);
			glVertex3d(-l/2, -h/2, -w/2);
			glNormal3f(-1.0f, 1.0f, -1.0f);
			glVertex3d(-l/2,  h/2, -w/2);
			glNormal3f( 1.0f, 1.0f, -1.0f);
			glVertex3d( l/2,  h/2, -w/2);
			glNormal3f( 1.0f, -1.0f, -1.0f);
			glVertex3d( l/2, -h/2, -w/2);
		glEnd();

		//UP
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f( 0.0f,  1.0f,  0.0f);
			glNormal3f(-1.0f,  1.0f, -1.0f);
			glVertex3d(-l/2,  h/2, -w/2);
			glNormal3f(-1.0f,  1.0f,  1.0f);
			glVertex3d(-l/2,  h/2,  w/2);
			glNormal3f( 1.0f,  1.0f, 1.0f);
			glVertex3d( l/2,  h/2,  w/2);
			glNormal3f( 1.0f,  1.0f, -1.0f);
			glVertex3d( l/2,  h/2, -w/2);
		glEnd();

		//RIGHT
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f( 1.0f,  0.0f,  0.0f);
			glNormal3f( 1.0f, -1.0f, -1.0f);
			glVertex3d( l/2, -h/2, -w/2);
			glNormal3f( 1.0f,  1.0f, -1.0f);
			glVertex3d( l/2,  h/2, -w/2);
			glNormal3f( 1.0f,  1.0f,  1.0f);
			glVertex3d( l/2,  h/2,  w/2);
			glNormal3f( 1.0f,  -1.0f, 1.0f);
			glVertex3d( l/2, -h/2,  w/2);
		glEnd();

		//DOWN
		if (bWired)
			glBegin(GL_LINE_STIPPLE);
		else
			glBegin(GL_QUADS);
			glNormal3f( 0.0f, -1.0f,  0.0f); // ������� ��������� ����
			glNormal3f(-1.0f, -1.0f, -1.0f);
			glVertex3d(-l/2, -h/2, -w/2);
			glNormal3f( 1.0f, -1.0f, -1.0f);
			glVertex3d( l/2, -h/2, -w/2);
			glNormal3f( 1.0f, -1.0f,  1.0f);
			glVertex3d( l/2, -h/2,  w/2);
			glNormal3f(-1.0f,  -1.0f, 1.0f);
			glVertex3d(-l/2, -h/2,  w/2);
		glEnd();


	glPopMatrix();
}

void plane(GLfloat l, GLfloat h, GLfloat z, flat_orient fo,  bool bWired)
{
	GLint resol=50;
	GLfloat rl=l/resol;
	GLfloat rh=h/resol;
	GLfloat bl=-(l/2);
	GLfloat br=-bl;
	GLfloat top=h/2;
	GLfloat btm=-top;

	//row
	for(GLfloat i=bl; i<br; i+=rl)
	{
		//column
		if (bWired)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,0,z);
		for(GLfloat j=btm; j<top; j+=rh)
		{
			if (fo==TOP || fo==BOTTOM)
			{
				glNormal3f(i,j,z);
				glVertex3f(i,j,z);
				glNormal3f(i+rl,j,z);
				glVertex3f(i+rl,j,z);
			}
			if (fo==LEFT || fo==RIGHT)
			{
				glNormal3f(z,j,i);
				glVertex3f(z,j,i);
				glNormal3f(z,j,i+rl);
				glVertex3f(z,j,i+rl);
			}
			if (fo==UP || fo==DOWN)
			{
				glNormal3f(j,z,i);
				glVertex3f(j,z,i);
				glNormal3f(j,z,i+rl);
				glVertex3f(j,z,i+rl);
			}
		}
		glEnd();
	}
}

GLvoid print_msg(GLfloat x, GLfloat y, char* s, GLfloat m)
{
	char* str_ret;
	int cap=0;
	int sl1=strlen(s);
	int d=2;
	
	if (m>10) d=3;

	char* bfr=ftoa(m,d);
	int il=strlen(bfr);

	cap=sl1+il+2;
	str_ret=new char[cap];
	
	ZeroMemory(str_ret,sizeof(char)*cap);
	strcat(str_ret,s);
	strcat(str_ret," ");
	strcat(str_ret,bfr);

	print_msg(x, y, str_ret);

	if (bfr!=NULL)
		delete[] bfr;
	if (str_ret!=NULL)
		delete[] str_ret;
}

GLvoid print_msg(GLfloat x, GLfloat y, char* s)
{
	glPushMatrix();
		gluLookAt(0,0,10,0,0,0,0,1,0);
	
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glColor3f(0,1,0);
		/*glMaterialfv(GL_FRONT,GL_AMBIENT,make_vect(0.0f, 1.0f, 0.0f, 1.0f));
		glMaterialfv(GL_FRONT,GL_DIFFUSE,make_vect(0.0f,  1.0f, 0.0f, 1.0f));
		glMaterialfv(GL_FRONT,GL_SPECULAR,make_vect(0.0f, 1.0f, 0.0f, 1.0f));*/

		glRasterPos3f(x,y,0);
		if (s && strlen(s)) 
		{
		  while (*s) 
		  {
			 glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s);
			 s++;
		  }
		}
	glPopMatrix();
}