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

HGLRC  hGRC=NULL;            // Постоянный контекст рендеринга
HDC  hGDC=NULL;              // Приватный контекст устройства GDI
HWND  hGWnd=NULL;            // Здесь будет хранится дескриптор окна
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
GLfloat Light_Ka[]= { .25f, .25f, .25f, 1.0f }; // Значения ambient света
GLfloat Light_Kd[]= { .25f, .25f, .25f, 1.0f }; // Значения diffuse света
GLfloat Light_Ks[]= { .25f, .25f, .25f, 1.0f }; // Значения specular света
GLfloat Light_Kp[]= { 0.0f, 0.0f, 2.5f, 0.0f };// Позиция света 

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
	GLuint   PixelFormat;              // Хранит результат после поиска

	DWORD    dwExStyle;              // Расширенный стиль окна
	DWORD    dwStyle;              // Обычный стиль окна
	
	RECT WindowRect;                // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;              // Установить левую составляющую в 0
	WindowRect.right=(long)width;              // Установить правую составляющую в Width
	WindowRect.top=(long)0;                // Установить верхнюю составляющую в 0
	WindowRect.bottom=(long)height;              // Установить нижнюю составляющую в Height

	WNDCLASS  wc;                // Структура класса окна
	wc.style    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;      // Перерисуем при перемещении и создаём скрытый DC
	wc.lpfnWndProc    = WndProc;          // Процедура обработки сообщений
	wc.cbClsExtra    = 0;              // Нет дополнительной информации для окна
	wc.cbWndExtra    = 0;              // Нет дополнительной информации для окна
	wc.hInstance    = hInst;            // Устанавливаем дескриптор
	wc.hIcon    = LoadIcon(NULL, IDI_WINLOGO);        // Загружаем иконку по умолчанию
	wc.hCursor    = LoadCursor(NULL, IDC_ARROW);        // Загружаем указатель мышки
	wc.hbrBackground  = NULL;              // Фон не требуется для GL
	wc.lpszMenuName    = NULL;              // Меню в окне не будет
	wc.lpszClassName  = "OpenGL";            // Устанавливаем имя классу

	//ptC.x=0;
	//ptC.y=0;

	dwExStyle  =   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;      // Расширенный стиль окна
	dwStyle    =   WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL;        // Обычный стиль окна

	if( !RegisterClass( &wc ) )              // Пытаемся зарегистрировать класс окна
	{
		MessageBox( NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                // Выход и возвращение функцией значения false
	}

	AdjustWindowRectEx( &WindowRect, dwStyle, false, dwExStyle );      // Подбирает окну подходящие размеры

	if( !( hwnd = CreateWindowEx(  dwExStyle,          // Расширенный стиль для окна
          "OpenGL",          // Имя класса
          title,            // Заголовок окна
          WS_CLIPSIBLINGS |        // Требуемый стиль для окна
          WS_CLIPCHILDREN |        // Требуемый стиль для окна
          dwStyle,          // Выбираемые стили для окна
          0, 0,            // Позиция окна
		  width,    // Вычисление подходящей ширины
          height,    // Вычисление подходящей высоты
          prnt,            // Parent
          NULL,            // Нет меню
          hInst,          // Дескриптор приложения
          NULL ) ) )          // Не передаём ничего до WM_CREATE (???)
	{
		KillGLWindow();                // Восстановить экран
		MessageBox( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                // Вернуть false
	}

	hGWnd=hwnd;
	hGInst=hInst;
	
	static  PIXELFORMATDESCRIPTOR pfd=   // pfd сообщает Windows каким будет вывод на экран каждого пикселя
	{
		sizeof(PIXELFORMATDESCRIPTOR),   // Размер дескриптора данного формата пикселей
		1,                               // Номер версии
		PFD_DRAW_TO_WINDOW |             // Формат для Окна
		PFD_SUPPORT_OPENGL |             // Формат для OpenGL
		PFD_DOUBLEBUFFER,                // Формат для двойного буфера
		PFD_TYPE_RGBA,                   // Требуется RGBA форма
		bits,                            // Выбирается бит глубины цвета
		0, 0, 0, 0, 0, 0,                // Игнорирование цветовых битов
		0,                               // Нет буфера прозрачности
		0,                               // Сдвиговый бит игнорируется
		0,                               // Нет буфера накопления
		0, 0, 0, 0,                      // Биты накопления игнорируются
		32,                              // 32 битный Z-буфер (буфер глубины)
		0,                               // Нет буфера трафарета
		0,                               // Нет вспомогательных буферов
		PFD_MAIN_PLANE,                  // Главный слой рисования
		0,                               // Зарезервировано
		0, 0, 0                          // Маски слоя игнорируются
	};

	if( !( hGDC = GetDC( hwnd ) ) )       // Можем ли мы получить Контекст Устройства?
	{
		KillGLWindow();                  // Восстановить экран
		MessageBox( NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                    // Вернуть false
	}

	if(!arbMultisampleSupported)
	{
		if( !( PixelFormat = ChoosePixelFormat( hGDC, &pfd ) ) )        // Найден ли подходящий формат пикселя?
		{
			KillGLWindow();                                            // Восстановить экран
			MessageBox( NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return false;                                              // Вернуть false
		}

	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}

	if( !SetPixelFormat( hGDC, PixelFormat, &pfd ) )   // Возможно ли установить Формат Пикселя?
	{
		KillGLWindow();                               // Восстановить экран
		MessageBox( NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                                 // Вернуть false
	}

	if( !( hGRC = wglCreateContext( hGDC ) ) )          // Возможно ли установить Контекст Рендеринга?
	{
		KillGLWindow();                               // Восстановить экран
		MessageBox( NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;                                 // Вернуть false
	}

	if( !wglMakeCurrent( hGDC, hGRC ) )  // Попробовать активировать Контекст Рендеринга
	{
		KillGLWindow();                // Восстановить экран
		MessageBox( NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                  // Вернуть false
	}

	if(!arbMultisampleSupported && CHECK_FOR_MULTISAMPLE)
	{
	
		if(InitMultisample(wc.hInstance,hwnd,pfd))
		{
			KillGLWindow();
			return CreateGLWindow(prnt, hInst, WndProc, title, width, height, 32);
		}
	}

	ShowWindow( hwnd, SW_SHOW );              // Показать окно
	SetForegroundWindow( hwnd );              // Слегка повысим приоритет
	SetFocus( hwnd );                         // Установить фокус клавиатуры на наше окно
	ReSizeGLScene( width, height );           // Настроим перспективу для нашего OpenGL экрана.

	if( !InitGL() )                           // Инициализация только что созданного окна
	{
		KillGLWindow();                       // Восстановить экран
		MessageBox( NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return false;                         // Вернуть false
	}

	return true;                              // Всё в порядке!
}

GLvoid ReSizeGLScene( GLsizei width, GLsizei height )        // Изменить размер и инициализировать окно GL 
{
	if( height == 0 )              // Предотвращение деления на ноль 
	{
	   height = 1;
	}
    
	glViewport( 0, 0, width, height );  // Сброс текущей области вывода 
	glMatrixMode( GL_PROJECTION );      // Выбор матрицы проекций
    glLoadIdentity();					// Сброс матрицы проекции 

    // Вычисление соотношения геометрических размеров для окна
    gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f );

	
	/*if(width<=height)
		glOrtho(-20,20,-20*(GLfloat)height/(GLfloat)width,20.0*(GLfloat)height/(GLfloat)width,-20,20);
	else
		glOrtho(-20*(GLfloat)width/(GLfloat)height,20.0*(GLfloat)width/(GLfloat)height,-20,20,-20,20);*/
	

    glMatrixMode( GL_MODELVIEW );            // Выбор матрицы вида модели
    glLoadIdentity();              // Сброс матрицы вида модели
}

int DrawGLScene( GLvoid )                // Здесь будет происходить вся прорисовка
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );      // Очистить экран и буфер глубины
	glEnable(GL_COLOR_MATERIAL);
	
	//FSAA - сглаживание
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
	glLoadIdentity();              // Сбросить текущую матрицу
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
	return true;                // Прорисовка прошла успешно
}

GLvoid KillGLWindow( GLvoid )              // Корректное разрушение окна
{
	if( hGRC )                // Существует ли Контекст Рендеринга?
    {
		if( !wglMakeCurrent( NULL, NULL ) )        // Возможно ли освободить RC и DC?
		{
			MessageBox( NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		if( !wglDeleteContext( hGRC ) )        // Возможно ли удалить RC?
		{
			MessageBox( NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		hGRC = NULL;              // Установить RC в NULL
    }
	
	if( hGDC && !ReleaseDC( hGWnd, hGDC ) )          // Возможно ли уничтожить DC?
	{
		MessageBox( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGDC=NULL;                // Установить DC в NULL
	}
	

	if(hGWnd && !DestroyWindow(hGWnd))            // Возможно ли уничтожить окно?
	{
		MessageBox( NULL, "Could Not Release hGWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGWnd = NULL;                // Установить hGWnd в NULL
	}

	hGWnd=NULL;
	
	if( !UnregisterClass( "OpenGL", hGInst ) )        // Возможно ли разрегистрировать класс
	{	
		
		MessageBox( NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		//hGInst = NULL;                // Установить hInstance в NULL
	}

	//hGInst=NULL;

	if (C_SPHERE!=NULL)
	{
		VirtualFree(C_SPHERE,0,MEM_RELEASE);
		C_SPHERE=NULL;
		C_SIZE=0;
	}
}

int InitGL( GLvoid )                // Все установки касаемо OpenGL происходят здесь
{
	//glShadeModel( GL_SMOOTH );            // Разрешить плавное цветовое сглаживание
	
	//LoadTex();        // Загрузить тектстуру
	//glEnable(GL_TEXTURE_2D);		// Разрешение наложение текстуры

	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);          // Очистка экрана в черный цвет
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
	glClearDepth( 1.0f );              // Разрешить очистку буфера глубины
    glEnable( GL_DEPTH_TEST );            // Разрешить тест глубины
    glDepthFunc( GL_LEQUAL );            // Тип теста глубины
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );      // Улучшение в вычислении перспективы
	glHint( GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST);

	glLightfv(GL_LIGHT1, GL_AMBIENT, Light_Ka);    // Установка Фонового Света
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Kd);    // Установка Диффузного Света
	glLightfv(GL_LIGHT1, GL_SPECULAR, Light_Ks);    // Установка Отраженного Света
	glLightfv(GL_LIGHT1, GL_POSITION, Light_Kp);   // Позиция света
	glEnable(GL_LIGHT1); // Разрешение источника света номер 1

	glLightfv(GL_LIGHT2, GL_AMBIENT, LightA_Ka);    // Установка Фонового Света
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightA_Kd);    // Установка Диффузного Света
	glLightfv(GL_LIGHT2, GL_SPECULAR, LightA_Ks);    // Установка Отраженного Света
	glLightfv(GL_LIGHT2, GL_POSITION, LightA_Kp);   // Позиция света
	//glEnable(GL_LIGHT2); // Разрешение источника света номер 2

	glLightfv(GL_LIGHT3, GL_AMBIENT, LightB_Ka);    // Установка Фонового Света
	glLightfv(GL_LIGHT3, GL_DIFFUSE, LightB_Kd);    // Установка Диффузного Света
	glLightfv(GL_LIGHT3, GL_SPECULAR, LightB_Ks);    // Установка Отраженного Света
	glLightfv(GL_LIGHT3, GL_POSITION, LightB_Kp);   // Позиция света
	//glEnable(GL_LIGHT3); // Разрешение источника света номер 3

	glEnable(GL_LIGHTING);
	
	return true;                // Инициализация прошла успешно
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
	DrawGLScene();        // Рисуем сцену
    SwapBuffers(hGDC);    // Меняем буфер (двойная буферизация)
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

		// Передняя грань
		glNormal3f( 0.0f, 0.0f, 1.0f);   // Нормаль указывает на наблюдателя
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 1 (Перед) 
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 2 (Перед)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Перед)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 4 (Перед)
		if (bWired)
		{
			glVertex3f( 1.0f, 1.0f,  -1.0f); 
			glVertex3f( -1.0f, 1.0f,  -1.0f); 
		}


		// Задняя грань
		//glColor4f(0,1,0,TRANSPARENSY);
		glNormal3f( 0.0f, 0.0f,-1.0f);   // Нормаль указывает от наблюдателя
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Зад)
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 2 (Зад)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 3 (Зад)
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 4 (Зад)
		if (bWired)
		{
			glVertex3f( -1.0f,  1.0f,  1.0f); 
			glVertex3f( -1.0f, -1.0f,  1.0f); 
		}

		// Верхняя грань
		//glColor4f(0,1,1,TRANSPARENSY);
		glNormal3f( 0.0f,  1.0f, 0.0f);   // Нормаль указывает вверх
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 1 (Верх)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 2 (Верх)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Верх)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 4 (Верх)
		if (bWired)
		{
			glVertex3f( 1.0f, -1.0f,  -1.0f); 
			glVertex3f( 1.0f, -1.0f,  1.0f); 
		}

		// Нижняя грань
		//glColor4f(1,0,0,TRANSPARENSY);
		glNormal3f( 0.0f, -1.0f,  0.0f); // Нормаль указывает вниз
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Низ)
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 2 (Низ)
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 3 (Низ)
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 4 (Низ)

		// Правая грань
		//glColor4f(1,0,1,TRANSPARENSY);
		glNormal3f( 1.0f,  0.0f,  0.0f); // Нормаль указывает вправо
		glNormal3f( 1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 1 (Право)
		glNormal3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 2 (Право)
		glNormal3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Право)
		glNormal3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 4 (Право)

		// Левая грань
		//glColor4f(1,1,0,TRANSPARENSY);
		glNormal3f(-1.0f, 0.0f, 0.0f);   // Нормаль указывает влево
		glNormal3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Лево)
		glNormal3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 2 (Лево)
		glNormal3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 3 (Лево)
		glNormal3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 4 (Лево)
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
			glNormal3f( 0.0f, -1.0f,  0.0f); // Нормаль указывает вниз
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