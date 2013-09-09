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

#include "mn.h"

#include "shape_derived.h"
#include "ogl_shape.h"
#include "analitycs.h"
#include "analitycs3d.h"
#include "str_pp.h"

#include "videom.h"
#include "algom.h"
#include "ogle.h"
#include "flate.h"

#include <windows.h> 
#include <stdlib.h> 
#include <iostream>

#include <commctrl.h> 

#ifdef _DEBUG
#include <assert.h>
#endif

using namespace std;

HINSTANCE    hInstance;                         //инстанс главного окна
POINT	     bPt;                               //нижняя правая точка поля
circle*      pCircle=NULL;
OglSphere*   pSphere=NULL;
int          nCircle=0;
int          nSphere=0;
COLORREF     indic=RGB(200,200,200);
circle       tcl(0,0,10,0);
TCHAR		 szFileName[_MAX_FNAME + _MAX_EXT]; //имя файла для сохранения
OPENFILENAME of;                                //структура файла для сохранения
HBITMAP      hBMP;                              //указатель на bitmap
PBITMAPINFO  pBmpInf;                           //указатель на структуру информации о bitmap'е
static box*  border;

int  vsh=0, hsh=0; //сдвиг картинки по вертикали, горизонтали
int  I_SCALE=0;
bool inter=0;

LRESULT CALLBACK WndProc   (HWND, UINT, WPARAM, LPARAM);                           //оконная процедура главного окна
LRESULT CALLBACK IModeWndProc   (HWND, UINT, WPARAM, LPARAM);	
WNDPROC PrevIModeWndProc=NULL;
LRESULT CALLBACK ClntWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);  //оконная процедура клиентского окна

void    LoadStrTable();                                                   //подгрузка строк из ресурсного файла
void	SaveToEmfFile();                                                  //сохранить в файл
void	SaveToTxtFile(); 
void	SaveToTxtFile(const char*, const POINT3D*, int); 
bool	GetFieldsValue(POINT* var);
bool	GetFieldBound();
void	AddCircle(POINT pt, int r, int c);
bool	OutOfBound(circle pC, box* pB);								   //получить число из текста окна
int 	GetCirclesOptVar(POINT* pt, int n);
int		GetSphereOptVar(POINT* pt, int n);
int		GetSphereOptVar(POINT* pt, int n, int l, int h, int w);
void	FillVCoef2D(POINT* pt, int n, int l, int h);
void	FillVCoef3D(POINT* pt, int n, int l, int h, int w);


//строковые данные
char  szAppTitle[MAX_STRLEN], szAboutTxt[MAX_STRLEN], szBtnBuild[MAX_STRLEN];
char  szGrpF[MAX_STRLEN], szGrpCoord[MAX_STRLEN], szSave[MAX_STRLEN];
char  szExitTxt[MAX_STRLEN], szExitTtl[MAX_STRLEN], szInterVal[MAX_STRLEN];
char  szErrorTxt[MAX_STRLEN],szError[MAX_STRLEN], szOptVal[MAX_STRLEN];
char  szIsFill[MAX_STRLEN], szErrorPnt[MAX_STRLEN];
char  szRadius[MAX_STRLEN], szCost[MAX_STRLEN], szCount[MAX_STRLEN];
char  szSqOvrf[MAX_STRLEN], szSqOvrl[MAX_STRLEN], szSqBox[MAX_STRLEN];
char  sz3DMode[MAX_STRLEN], sz2DMode[MAX_STRLEN], szProjMode[MAX_STRLEN];
char  szAlgoMode[MAX_STRLEN], szVisMode[MAX_STRLEN];
char  szPnt[MAX_STRLEN]="ед.",szMeter[MAX_STRLEN]="м", szMeterVol[MAX_STRLEN]="м^2";
char  szWndClsName[MAX_STRLEN]="field_builder",szClntClass[MAX_STRLEN]="ClntCls" ;

//хендлы окон
HWND  hClntWnd, hMainWnd, hEditWnd, hBldBtnWnd, hGrpFWnd;
HWND  hCHeightWnd, hCLengthWnd, hTHeightWnd, hTLengthWnd, hMHeightWnd, hMLengthWnd, hTWidthWnd, hCWidthWnd, hMWidthWnd;
HWND  hGrpOptValWnd, hCX0Wnd, hCY0Wnd, hCZ0Wnd, hTX0Wnd, hTY0Wnd, hTZ0Wnd,  hMX0Wnd, hMY0Wnd, hMZ0Wnd;
HWND  hGrpSqrValWnd, hCXSWnd, hCYSWnd, hCZSWnd, hTXSWnd, hTYSWnd, hTZSWnd,  hMXSWnd, hMYSWnd, hMZSWnd;
HWND  hGrpISqrValWnd, hICXSWnd, hICYSWnd, hICZSWnd, hITXSWnd, hITYSWnd, hITZSWnd, hIMXSWnd, hIMYSWnd, hIMZSWnd;
HWND  hGrpInterValWnd, hInterSwWnd ,hICX0Wnd, hICY0Wnd, hITX0Wnd, hITY0Wnd, hIMX0Wnd, hIMY0Wnd;
HWND  hIFill, hICX1Wnd, hICY1Wnd, hITX1Wnd, hITY1Wnd, hIMX1Wnd, hIMY1Wnd;
HWND  hCX1Wnd, hCY1Wnd, hGrpCoordWnd, hTX1Wnd, hTY1Wnd, hMX1Wnd, hMY1Wnd;
HWND  hCX2Wnd, hCY2Wnd, hTX2Wnd, hTY2Wnd, hMX2Wnd, hMY2Wnd;
HWND  hCX3Wnd, hCY3Wnd, hTX3Wnd, hTY3Wnd, hMX3Wnd, hMY3Wnd;
HWND  hCX4Wnd, hCY4Wnd, hTX4Wnd, hTY4Wnd, hMX4Wnd, hMY4Wnd;
HWND  hCX5Wnd, hCY5Wnd, hTX5Wnd, hTY5Wnd, hMX5Wnd, hMY5Wnd;
HWND  hCAWnd[5], hCBWnd[5], hTAWnd[5], hTBWnd[5], hMAWnd[5], hMB1Wnd[5];

//-------mode handle--------
HWND h3DMode, h2DMode, hProjMode;
HWND hStdAlgo, hAltrAlgo;
HWND hAlgoMode, hVisMode;

//координаты групповых окон формулы и координат
POINT ptGrpField, ptGrpCoord, ptGrpOptVal, ptInterVal; 
POINT ptGrpSqrVal, ptGrpISqrVal, ptGrpAlgoMode, ptGrpVisMode; 

//начальная функция программы 
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, 
                    PSTR szCmdLine, int iCmdShow) 
{ 
     HWND         hwnd;
     MSG          msg; 
	 HACCEL		  hAccelTable;
     WNDCLASSEX   wndclass, wcls;

   //MainWndClass - клас главного окна
     wndclass.cbSize        = sizeof(wndclass); 
     wndclass.style         = CS_HREDRAW | CS_VREDRAW; 
     wndclass.lpfnWndProc   = WndProc;
     wndclass.cbClsExtra    = 0; 
     wndclass.cbWndExtra    = 0; 
     wndclass.hInstance     = hInst; 
     wndclass.hIcon         = LoadIcon(NULL,(LPCSTR)IDI_APPLICATION); 
     wndclass.hCursor       = LoadCursor(NULL,(LPCSTR)IDC_ARROW); 
     wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	 wndclass.lpszMenuName  = NULL; 
     wndclass.lpszClassName = szWndClsName; 
     wndclass.hIconSm       = LoadIcon(hInst,(LPCSTR)IDI_APPLICATION); 
 
     RegisterClassEx(&wndclass); 
	
	//ClntWndClass - клас клиентского окна
	wcls.cbSize			= sizeof(wndclass); 
	wcls.style			= CS_HREDRAW | CS_VREDRAW;
	wcls.lpfnWndProc	= ClntWndProc;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= 0;
	wcls.hInstance		= hInst;
	wcls.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcls.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcls.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcls.lpszMenuName	= NULL;
	wcls.lpszClassName	= szClntClass;
	wcls.hIconSm		= LoadIcon(NULL, IDI_APPLICATION); 
	
	RegisterClassEx(&wcls); 

	 LoadStrTable();

	 //создание окон

	 hwnd = CreateWindow(szWndClsName, szAppTitle,                //главное окно
						  WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, 
                          CW_USEDEFAULT, CW_USEDEFAULT, 
						  CW_USEDEFAULT, CW_USEDEFAULT, 
                          NULL, NULL, hInst, NULL); 
	 

	 hClntWnd=CreateWindowEx(WS_EX_CLIENTEDGE, szClntClass, "",   //клиентское окно
                              WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL , 
                              0, 0, 
							  800, 580, 
                              hwnd,NULL, hInstance, NULL);

	 hGrpFWnd=CreateWindow("button", szGrpF,                      //групповое окно формулы
                              WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);
	 
	 hGrpCoordWnd=CreateWindow("button", szGrpCoord,               //групповое окно координат
                              WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);
 
	 hEditWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",        //окно ввода формулы
                              WS_CHILD | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 //----------------------interactive-----------------------
	 
	 hGrpInterValWnd=CreateWindow("button", szInterVal,               //групповое окно координат
                              WS_CHILD | /*WS_VISIBLE |*/ BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 hICX0Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | WS_DISABLED, 
                              100,25,90,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);
	 
	 hICY0Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | WS_DISABLED, 
                              100,65,90,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hITX0Wnd=CreateWindow("static", GetTitle(szRadius), 
                              WS_CHILD | WS_VISIBLE, 
                              10,25,60,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hITY0Wnd=CreateWindow("static", GetTitle(szCost), 
                              WS_CHILD | WS_VISIBLE, 
                              10,65,80,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hIMX0Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              200,25,20,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hIMY0Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              200,65,22,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hICX1Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,125,90,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);
	 
	 hICY1Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,165,90,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hITX1Wnd=CreateWindow("static", GetTitle("Общая ",szCost), 
                              WS_CHILD | WS_VISIBLE, 
                              10,120,80,45, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hITY1Wnd=CreateWindow("static", GetTitle(szCount), 
                              WS_CHILD | WS_VISIBLE, 
                              10,165,82,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hIMX1Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              200,125,22,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hIMY1Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              200,165,22,25, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 hInterSwWnd=CreateWindow("button", "Интерактивный режим", 
							  WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 hIFill=CreateWindow("static", szIsFill, 
                              WS_CHILD | WS_VISIBLE, 
                              10,205,80,40, 
                              hGrpInterValWnd,NULL, hInstance, NULL);

	 PrevIModeWndProc=(WNDPROC)GetWindowLong(hGrpInterValWnd,GWL_WNDPROC);
	 SetWindowLongPtr(hGrpInterValWnd,GWL_WNDPROC,(LONG_PTR)IModeWndProc);

	 //-----------------------optimal--------------------------

	 hGrpOptValWnd=CreateWindow("button", szOptVal,               //групповое окно координат
                              WS_CHILD | WS_VISIBLE |  BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 hCX0Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,20,90,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);
	 
	 hCY0Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,60,90,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hCZ0Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,100,90,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hTX0Wnd=CreateWindow("static", GetTitle(szRadius), 
                              WS_CHILD | WS_VISIBLE, 
                              10,20,80,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hTY0Wnd=CreateWindow("static", GetTitle(szCost), 
                              WS_CHILD | WS_VISIBLE, 
                              10,60,80,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hTZ0Wnd=CreateWindow("static", GetTitle(szCount), 
                              WS_CHILD | WS_VISIBLE, 
                              10,100,82,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hMX0Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              195,20,20,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hMY0Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              195,60,22,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 hMZ0Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              195,100,22,25, 
                              hGrpOptValWnd,NULL, hInstance, NULL);

	 
	 
	 //-----------------------square--------------------------

	 hGrpSqrValWnd=CreateWindow("button", szSqOvrf,               //групповое окно координат
                              WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 hCXSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,25,70,25, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);
	 
	 hCYSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,75,70,25, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hCZSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,125,70,25, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hTXSWnd=CreateWindow("static", szSqBox, 
                              WS_CHILD | WS_VISIBLE, 
                              10,20,80,40, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hTYSWnd=CreateWindow("static", szSqOvrl, 
                              WS_CHILD | WS_VISIBLE, 
                              10,60,80,50, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hTZSWnd=CreateWindow("static", GetTitle(szSqOvrf), 
                              WS_CHILD | WS_VISIBLE, 
                              10,120,82,40, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hMXSWnd=CreateWindow("static", szMeterVol, 
                              WS_CHILD | WS_VISIBLE, 
                              180,25,25,25, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hMYSWnd=CreateWindow("static", szMeterVol, 
                              WS_CHILD | WS_VISIBLE, 
                              180,75,25,25,
                              hGrpSqrValWnd,NULL, hInstance, NULL);

	 hMZSWnd=CreateWindow("static", "", 
                              WS_CHILD | WS_VISIBLE, 
                              180,125,25,25, 
                              hGrpSqrValWnd,NULL, hInstance, NULL);


	 //-------------------interactive square--------------------------

	 hGrpISqrValWnd=CreateWindow("button", szSqOvrf,               //групповое окно координат
                              WS_CHILD |/* WS_VISIBLE |*/ BS_GROUPBOX, 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 hICXSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,25,70,25, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);
	 
	 hICYSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,75,70,25, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hICZSWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_LOWERCASE | ES_READONLY, 
                              100,125,70,25, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hITXSWnd=CreateWindow("static", szSqBox, 
                              WS_CHILD | WS_VISIBLE, 
                              10,20,80,40, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hITYSWnd=CreateWindow("static", szSqOvrl, 
                              WS_CHILD | WS_VISIBLE, 
                              10,60,80,50, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hITZSWnd=CreateWindow("static", GetTitle(szSqOvrf), 
                              WS_CHILD | WS_VISIBLE, 
                              10,120,82,35, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hIMXSWnd=CreateWindow("static", szMeterVol, 
                              WS_CHILD | WS_VISIBLE, 
                              180,25,25,25, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hIMYSWnd=CreateWindow("static", szMeterVol, 
                              WS_CHILD | WS_VISIBLE, 
                              180,75,25,25, 
                              hGrpISqrValWnd,NULL, hInstance, NULL);

	 hIMZSWnd=CreateWindow("static", "", 
                              WS_CHILD | WS_VISIBLE, 
                              180,125,25,25,  
                              hGrpISqrValWnd,NULL, hInstance, NULL);


	  //-----------------------field---------------------------
	 
	 hCHeightWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              80,20,90,25, 
                              hGrpFWnd,NULL, hInstance, NULL);
	 
	 hCLengthWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              80,50,90,25, 
                              hGrpFWnd,NULL, hInstance, NULL);
	 hCWidthWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              80,80,90,25, 
                              hGrpFWnd,NULL, hInstance, NULL);


	 hTHeightWnd=CreateWindow("static", "Высота:", 
                              WS_CHILD | WS_VISIBLE, 
                              10,20,64,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 hTLengthWnd=CreateWindow("static", "Длинна:", 
                              WS_CHILD | WS_VISIBLE, 
                              10,50,64,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 hTWidthWnd=CreateWindow("static", "Ширина:", 
                              WS_CHILD | WS_VISIBLE, 
                              10,80,64,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 hMHeightWnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              180,20,10,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 hMLengthWnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              180,50,10,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 hMWidthWnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              180,80,10,25, 
                              hGrpFWnd,NULL, hInstance, NULL);

	 //=======================================================
	 //-----------------------1---------------------------
	 
	 hCX1Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,20,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCY1Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,20,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTX1Wnd=CreateWindow("static", GetTitle(szRadius,1), 
                              WS_CHILD | WS_VISIBLE, 
                              10,20,80,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTY1Wnd=CreateWindow("static", GetTitle(szCost,1), 
                              WS_CHILD | WS_VISIBLE, 
                              250,20,85,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMX1Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              160,20,20,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMY1Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              410,20,22,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);




	 hCAWnd[0]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,60,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCBWnd[0]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,60,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTAWnd[0]=CreateWindow("static", GetTitle("Стоим. ед. покрытия",1), 
                              WS_CHILD | WS_VISIBLE, 
                              10,55,80,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTBWnd[0]=CreateWindow("static", GetTitle("Коефициент перекрытия",1), 
                              WS_CHILD | WS_VISIBLE, 
                              250,55,100,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMAWnd[0]=CreateWindow("static", GetTitle("ед./",szMeterVol), 
                              WS_CHILD | WS_VISIBLE, 
                              160,60,49,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 //------------------------2--------------------------

	 hCX2Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,100,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCY2Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,100,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTX2Wnd=CreateWindow("static", GetTitle(szRadius,2), 
                              WS_CHILD | WS_VISIBLE, 
                              10,100,80,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTY2Wnd=CreateWindow("static", GetTitle(szCost,2), 
                              WS_CHILD | WS_VISIBLE, 
                              250,100,85,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMX2Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              160,100,20,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMY2Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              410,100,22,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);



	 hCAWnd[1]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,140,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCBWnd[1]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,140,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTAWnd[1]=CreateWindow("static", GetTitle("Стоим. ед. покрытия",2), 
                              WS_CHILD | WS_VISIBLE, 
                              10,135,80,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTBWnd[1]=CreateWindow("static", GetTitle("Коефициент перекрытия",2), 
                              WS_CHILD | WS_VISIBLE, 
                              250,135,100,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMAWnd[1]=CreateWindow("static", GetTitle("ед./",szMeterVol), 
                              WS_CHILD | WS_VISIBLE, 
                              160,140,49,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 //------------------------3----------------------------------

	 hCX3Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,180,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCY3Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,180,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTX3Wnd=CreateWindow("static", GetTitle(szRadius,3), 
                              WS_CHILD | WS_VISIBLE, 
                              10,180,80,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTY3Wnd=CreateWindow("static", GetTitle(szCost,3), 
                              WS_CHILD | WS_VISIBLE, 
                              250,180,85,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMX3Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              160,180,20,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMY3Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              410,180,22,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);



	 hCAWnd[2]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,220,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCBWnd[2]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,220,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTAWnd[2]=CreateWindow("static", GetTitle("Стоим. ед. покрытия",3), 
                              WS_CHILD | WS_VISIBLE, 
                              10,215,80,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTBWnd[2]=CreateWindow("static", GetTitle("Коефициент перекрытия",3), 
                              WS_CHILD | WS_VISIBLE, 
                              250,215,100,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMAWnd[2]=CreateWindow("static", GetTitle("ед./",szMeterVol), 
                              WS_CHILD | WS_VISIBLE, 
                              160,220,49,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 //-----------------------4-----------------------------------

	 hCX4Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,260,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCY4Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,260,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTX4Wnd=CreateWindow("static", GetTitle(szRadius,4), 
                              WS_CHILD | WS_VISIBLE, 
                              10,260,80,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTY4Wnd=CreateWindow("static", GetTitle(szCost,4), 
                              WS_CHILD | WS_VISIBLE, 
                              250,260,85,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMX4Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              160,260,20,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMY4Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              410,260,22,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);




	 hCAWnd[3]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,300,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCBWnd[3]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,300,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTAWnd[3]=CreateWindow("static", GetTitle("Стоим. ед. покрытия",4), 
                              WS_CHILD | WS_VISIBLE, 
                              10,295,80,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTBWnd[3]=CreateWindow("static", GetTitle("Коефициент перекрытия",4), 
                              WS_CHILD | WS_VISIBLE, 
                              250,295,100,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMAWnd[3]=CreateWindow("static", GetTitle("ед./",szMeterVol), 
                              WS_CHILD | WS_VISIBLE, 
                              160,300,49,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 //-------------------------5---------------------------------

	 hCX5Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,340,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCY5Wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,340,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTX5Wnd=CreateWindow("static", GetTitle(szRadius,5), 
                              WS_CHILD | WS_VISIBLE, 
                              10,340,80,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTY5Wnd=CreateWindow("static", GetTitle(szCost,5), 
                              WS_CHILD | WS_VISIBLE, 
                              250,340,85,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMX5Wnd=CreateWindow("static", szMeter, 
                              WS_CHILD | WS_VISIBLE, 
                              160,340,20,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMY5Wnd=CreateWindow("static", szPnt, 
                              WS_CHILD | WS_VISIBLE, 
                              410,340,22,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);
	 



	 hCAWnd[4]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Х функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              100,380,50,25, 
							  hGrpCoordWnd,NULL, hInstance, NULL);
	 
	 hCBWnd[4]=CreateWindowEx(WS_EX_CLIENTEDGE,"edit", "",           //окно отображения координаты Y функции
							  WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT |ES_AUTOHSCROLL |ES_LOWERCASE , 
                              350,380,50,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTAWnd[4]=CreateWindow("static", GetTitle("Стоим. ед. покрытия",5), 
                              WS_CHILD | WS_VISIBLE, 
                              10,375,80,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hTBWnd[4]=CreateWindow("static", GetTitle("Коефициент перекрытия",5), 
                              WS_CHILD | WS_VISIBLE, 
                              250,375,100,30, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 hMAWnd[4]=CreateWindow("static", GetTitle("ед./",szMeterVol), 
                              WS_CHILD | WS_VISIBLE, 
                              160,380,49,25, 
                              hGrpCoordWnd,NULL, hInstance, NULL);

	 //===========================================================

	 hBldBtnWnd=CreateWindow("button", szBtnBuild,                //кнопка "Построить"
                              WS_CHILD | WS_VISIBLE | WS_BORDER , 
                              0,0,0,0, 
                              hwnd,NULL, hInstance, NULL);

	 //------------MODE SWITCHER----------------------------------
	 hVisMode=CreateWindow("button",szVisMode,
							 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
							 0,0,0,0,
							 hwnd,NULL,hInstance,NULL);

	 h3DMode=CreateWindowEx(0,"button",sz3DMode,
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							 10,20,80,20,
							 hVisMode,(HMENU)0,hInstance,NULL);

	 h2DMode=CreateWindowEx(0,"button",sz2DMode,
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							 10,50,80,20,
							 hVisMode,(HMENU)1,hInstance,NULL);

	 hProjMode=CreateWindowEx(0,"button",szProjMode,
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							 10,80,100,20,
							 hVisMode,(HMENU)2,hInstance,NULL);
	
	 VM_WndProcAssign(hVisMode);

	 hAlgoMode=CreateWindow("button",szAlgoMode,
							 WS_CHILD | /*WS_VISIBLE |*/ BS_GROUPBOX,
							 0,0,0,0,
							 hwnd,NULL,hInstance,NULL);

	 hStdAlgo=CreateWindowEx(0,"button","Стандартный",
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							 10,20,140,20,
							 hAlgoMode,(HMENU)0,hInstance,NULL);

	 hAltrAlgo=CreateWindowEx(0,"button","Альтернативный",
							 WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							 10,50,140,20,
							 hAlgoMode,(HMENU)1,hInstance,NULL);

	 AM_WndProcAssign(hAlgoMode);


	 border=new box(10,10,40,40);
	 border->set_color(RGB(0,0,0));
	 tcl.set_color(RGB(0,0,0));
	 bPt.x=0;

	 FieldWnd_ShowWidth(0);
	 ShowWindow(hwnd, SW_MAXIMIZE);//iCmdShow); 
     UpdateWindow(hwnd);

	 SendMessage(h2DMode,BM_SETCHECK,1,0);
	 SendMessage(hStdAlgo,BM_SETCHECK,1,0);

	 hAccelTable = LoadAccelerators(hInst,(LPCSTR)IDR_ACCEL);
	 hInstance=hInst;
	 hMainWnd=hwnd;
	
	 //Цыкл обработки сообщений
     /*while(GetMessage(&msg, NULL, 0, 0)) 
     {
		 if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		 {
            TranslateMessage(&msg); 
            DispatchMessage (&msg);
		 }
     } */
	 bool done=false;
	 while( !done )                // Цикл продолжается, пока done не равно true
	 {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )    // Есть ли в очереди какое-нибудь сообщение?
		{
			if( msg.message == WM_QUIT )        // Мы поучили сообщение о выходе?
			{
				done = true;          // Если так, done=true
			}
			else              // Если нет, обрабатывает сообщения
			{
				TranslateMessage( &msg );        // Переводим сообщение
				DispatchMessage( &msg );        // Отсылаем сообщение
			}
		}
		else                // Если нет сообщений
		{
			// Прорисовываем сцену.
			if( vmodIsActive() )          // Активна ли программа?
			{
				{
					 vmodDraw();
				}
			}
		}
	 }
     return msg.wParam; 
} 

LRESULT CALLBACK IModeWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) //оконная процедура главного окна
{
	//int		wmId, wmEvent;
	HDC			hdc;
	
	switch (iMsg)
	{
		case WM_PAINT:
			{
				hdc=GetDC(hwnd);
				HBRUSH hbr=CreateSolidBrush(indic);
				HGDIOBJ old=SelectObject(hdc,hbr);
				::Ellipse(hdc,120,215,140,235);
				SelectObject(hdc,old);
				DeleteObject(hbr);
				DeleteObject(old);
				ReleaseDC(hwnd,hdc);
			}
		default: 
			return 	CallWindowProc(PrevIModeWndProc,hwnd, iMsg, wParam, lParam); 
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) //оконная процедура главного окна
{  
    int			wmId, wmEvent;
	//HDC			hdc;
	//PAINTSTRUCT ps;
	RECT rc;

	switch (iMsg)
	{
		case WM_CREATE:
		{	
			SetScrollRange(hClntWnd, SB_HORZ, 0, 255, FALSE); 
			SetScrollPos  (hClntWnd, SB_HORZ, 0, FALSE); 
			
			//заполнение структуры Открытия (сохранения) файла
			of.lStructSize=sizeof(OPENFILENAME);                 
			of.hwndOwner=hwnd;
			of.hInstance=hInstance;
			of.lpstrFilter="Windows Meta File(*.EMF)\0*.emf\0\0"; 
			of.lpstrCustomFilter=NULL;
			of.nMaxCustFilter=0;
			of.nFilterIndex=0;
			of.lpstrFile=NULL;
			of.nMaxFile=_MAX_PATH;
			of.lpstrFileTitle=NULL;
			of.nMaxFileTitle=_MAX_FNAME + _MAX_EXT;
			of.lpstrInitialDir=NULL;
			of.lpstrTitle=NULL;
			of.Flags=0;
			of.nFileOffset=0;
			of.nFileExtension=0;
			of.lpstrDefExt="emf";
			of.lCustData=0L;
			of.lpfnHook=NULL;
			of.lpTemplateName=NULL;
		}
		case WM_SIZE: 
		{
			GetClientRect(hwnd,&rc);

			ptInterVal.x=(rc.right-rc.left)-475;
			ptInterVal.y=5;
			
			ptGrpField.x=(rc.right-rc.left)-220;
			ptGrpField.y=rc.top+5;

			ptGrpCoord.x=ptInterVal.x;
			ptGrpCoord.y=(rc.bottom-rc.top)-460;

			ptGrpOptVal.x=ptInterVal.x;
			ptGrpOptVal.y=5;

			ptGrpSqrVal.x=ptGrpField.x;
			ptGrpSqrVal.y=ptGrpField.y+130;

			ptGrpISqrVal.x=ptGrpField.x;
			ptGrpISqrVal.y=ptGrpField.y+130;

			ptGrpVisMode.x=10;
			ptGrpVisMode.y=(rc.bottom-rc.top)-170;

			ptGrpAlgoMode.x=200;
			ptGrpAlgoMode.y=(rc.bottom-rc.top)-170;
			
			//переразмещение элементов главного окна

			//-------------------------Field-----------------------------
			MoveWindow(hGrpFWnd,ptGrpField.x, ptGrpField.y, 200,120,0);
			MoveWindow(hEditWnd,ptGrpField.x, ptGrpField.y+220, 60,25,0);
			//-------------------------Optimal value----------------------
			MoveWindow(hGrpOptValWnd, ptGrpOptVal.x, ptGrpOptVal.y, 220,140,0);


			MoveWindow(hGrpSqrValWnd, ptGrpSqrVal.x, ptGrpSqrVal.y, 210,170,0);
			//-------------------------Square----------------------------


			MoveWindow(hGrpISqrValWnd,ptGrpISqrVal.x, ptGrpISqrVal.y, 210,170,0);
			//---------------------Interactive Square---------------------


			MoveWindow(hGrpInterValWnd, ptInterVal.x, ptInterVal.y, 240,250,0);
			//-------------------------Interactive-------------------------
			
			MoveWindow(hGrpCoordWnd, ptGrpCoord.x, ptGrpCoord.y, 460,420,0);

			MoveWindow(hBldBtnWnd,10, (rc.bottom-rc.top)-35, 160,25,0);
			MoveWindow(hInterSwWnd,ptGrpAlgoMode.x, (rc.bottom-rc.top)-35, 180,25,0);

			//-------------------MODE SWITCHER----------------------------
			MoveWindow(hVisMode,ptGrpVisMode.x,ptGrpVisMode.y,160,110,0);
			MoveWindow(hAlgoMode,ptGrpAlgoMode.x,ptGrpAlgoMode.y,160,80,0);

			
			InvalidateRect(hwnd,NULL,1);
		}break;
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			
			inter=(int)SendMessage(hInterSwWnd, BM_GETCHECK, 0, 0L);
			if (inter==1)
			{
				EnableWindow(hICX0Wnd,1);      EnableWindow(hICY0Wnd,1);

				EnableWindow(hCX1Wnd,0);      EnableWindow(hCY1Wnd,0);
				EnableWindow(hCX2Wnd,0);      EnableWindow(hCY2Wnd,0);
				EnableWindow(hCX3Wnd,0);      EnableWindow(hCY3Wnd,0);
				EnableWindow(hCX4Wnd,0);      EnableWindow(hCY4Wnd,0);
				EnableWindow(hCX5Wnd,0);      EnableWindow(hCY5Wnd,0);

				EnableWindow(hBldBtnWnd,0); 

				ShowWindow(hGrpOptValWnd,SW_HIDE);
				ShowWindow(hGrpInterValWnd,SW_SHOW);
				ShowWindow(hGrpCoordWnd,SW_HIDE);
				ShowWindow(hGrpISqrValWnd,SW_SHOW);
				ShowWindow(hGrpSqrValWnd,SW_HIDE);

				indic=RGB(200,20,20);
				InvalidateRect(hMainWnd,NULL,0);
			}
			else
			{
				EnableWindow(hICX0Wnd,0);      EnableWindow(hICY0Wnd,0);

				EnableWindow(hCX1Wnd,1);      EnableWindow(hCY1Wnd,1);
				EnableWindow(hCX2Wnd,1);      EnableWindow(hCY2Wnd,1);
				EnableWindow(hCX3Wnd,1);      EnableWindow(hCY3Wnd,1);
				EnableWindow(hCX4Wnd,1);      EnableWindow(hCY4Wnd,1);
				EnableWindow(hCX5Wnd,1);      EnableWindow(hCY5Wnd,1);

				EnableWindow(hBldBtnWnd,1); 
				ShowWindow(hGrpOptValWnd,SW_SHOW);
				ShowWindow(hGrpInterValWnd,SW_HIDE);
				ShowWindow(hGrpCoordWnd,SW_SHOW);
				ShowWindow(hGrpISqrValWnd,SW_HIDE);
				ShowWindow(hGrpSqrValWnd,SW_SHOW);

				indic=RGB(200,200,200);
				InvalidateRect(hMainWnd,NULL,0);
			}
			
			if ((HWND)lParam==hInterSwWnd)
			{
				nCircle=0;
				nSphere=0;
			}

			if ((HWND)lParam==hBldBtnWnd) //при нажатии кнопки "Построить" 
			{
				long x=0;
				long y=0;
				POINT var[5];
				bool ok=true;

				ok&=GetFieldsValue(var);

				ok&=GetFieldBound();

				if (!ok) MessageBox(hwnd,szErrorTxt, szError, MB_OK | MB_ICONEXCLAMATION); //выдать предупреждение об ошибке
				else
				{
					int vc=0;
					for (int i=0; i<5; i++)
					{
						if (var[i].x==0) break;
						vc++;
					}
					
					int v=0;
					if (GetVideoMode()==V_2D)
						v=GetCirclesOptVar(var,vc);
					else
						v=GetSphereOptVar(var,vc);

					char* bfr=new char[8];
					ZeroMemory(bfr,sizeof(bfr));
					_itoa(var[v].x,bfr,10);
					SetWindowText(hCX0Wnd,bfr);

					if (GetVideoMode()==V_2D)
						_itoa((var[v].y*nCircle),bfr,10);
					else
						_itoa((var[v].y*nSphere),bfr,10);
					SetWindowText(hCY0Wnd,bfr);

					if (GetVideoMode()==V_2D)
						_itoa(nCircle,bfr,10);
					else
						_itoa(nSphere,bfr,10);
					SetWindowText(hCZ0Wnd,bfr);
					ZeroMemory(bfr,sizeof(bfr));

					if (GetVideoMode()==V_2D)
						_itoa(BoxSquare(border),bfr,10);
					else if (GetVideoMode()==V_3D)
						_itoa(oglGetVolume(),bfr,10);
					else
						_itoa(fltGetVolume(),bfr,10);
					SetWindowText(hCXSWnd,bfr);

					if (GetVideoMode()==V_2D)
						_itoa(OvrlSquare(pCircle,nCircle),bfr,10);
					else
						_itoa(OvrlVolume(pSphere,nSphere),bfr,10);

					SetWindowText(hCYSWnd,bfr);
					if (bfr!=NULL)
					{
						if(strlen(bfr)>1)
							delete[] bfr;
						else
							delete bfr;
					}

					if (GetVideoMode()==V_2D)
						bfr=ftoa(EfficientSquare(pCircle,nCircle,border),10);
					else
					{
						double l,h,w; 
						if(GetVideoMode()==V_3D)
							oglGetDimn(l,h,w);
						else
							fltGetDimn(l,h,w);
						bfr=ftoa(EfficientVolume(pSphere,nSphere,l,h,w),10);
					}
					SetWindowText(hCZSWnd,bfr);
					delete[] bfr;
					if (GetVideoMode()==V_2D)
						SaveToTxtFile();

					if (GetVideoMode()==V_2D)
					{
						if (pSphere!=NULL)
						{
							VirtualFree(pSphere,0,MEM_RELEASE);
							pSphere=NULL;
							nSphere=0;
						}
					}
					else
					{
						if (pCircle!=NULL)
							delete[] pCircle;
						pCircle=NULL;

						if(GetVideoMode()==V_PROJ)
							fltSetOptVar(var[v].x);
						else
							oglSetPtr(pSphere,nSphere);
					}

				}
				InvalidateRect(hClntWnd,NULL,1);
				if (GetVideoMode()==V_2D)
					SetFocus(hwnd);
				if (GetVideoMode()==V_3D)
					vmodSetFocus();
			}
			
			switch (wmId)
			{	
				case IDA_SAVE:
					SaveToEmfFile(); 
					break;
				case IDA_EXIT:
				   SendMessage(hwnd,WM_CLOSE,NULL,NULL);
				   break;
				default:
				   return DefWindowProc(hwnd, iMsg, wParam, lParam);
			}
		}
		break;
		case WM_KEYDOWN: //нажали клавишу, отвечающую за управление полосами прокрутки
			{
				WORD wScrollNotify = 0xFFFF; 
				bool hor=0;

				switch(wParam)
				{	
					case VK_LEFT:
						hor=1;
					case VK_UP: 
						wScrollNotify = SB_LINEUP; 
						break; 
					
					case VK_HOME:
						hor=1;
					case VK_PRIOR: 
						wScrollNotify = SB_PAGEUP; 
						break; 
					
					case VK_END: 
						hor=1;
					case VK_NEXT: 
						wScrollNotify = SB_PAGEDOWN; 
						break; 
					
					case VK_RIGHT:
						hor=1;
					case VK_DOWN: 
						wScrollNotify = SB_LINEDOWN; 
						break; 

				case VK_OEM_MINUS:
					I_SCALE--;
					break;
				case VK_OEM_PLUS:
					I_SCALE++;
					break;
				}
				if (wScrollNotify != -1) 
				{
					if (!hor) SendMessage(hClntWnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
					else	  SendMessage(hClntWnd, WM_HSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
				}
			}
			break;
		case WM_CLOSE: //Закрыть приложение без сохранения данных
			{
				
				int sel=IDNO;
				if (GetVideoMode()==V_2D)
					sel=MessageBox(hwnd,szExitTxt, szExitTtl, MB_YESNOCANCEL | MB_ICONQUESTION);
				switch (sel)
				{
					 case IDYES:
						 SaveToEmfFile();
						 DestroyWindow(hwnd);
						 break;
					 case IDNO:
						 DestroyWindow(hwnd);
						 break;
					 case IDCANCEL:
						 break;
				 }
				 break;
			}
			break;
		case WM_DESTROY :  //закрытие приложения
			delete[] pCircle;
			PostQuitMessage(0); 
			return 0; 
		default: 
			return DefWindowProc(hwnd, iMsg, wParam, lParam); 
	}

	return 0;
} 

void LoadStrTable() //подгрузка строк из ресурсного файла
{
		LoadString(hInstance,IDS_TITLE,szAppTitle, MAX_STRLEN);
		LoadString(hInstance,IDS_BTN_BUILD,szBtnBuild, MAX_STRLEN);
		LoadString(hInstance,IDS_GRP_FORMULA,szGrpF, MAX_STRLEN);
		LoadString(hInstance,IDS_GRP_COORD,szGrpCoord, MAX_STRLEN);
		LoadString(hInstance,IDS_SAVE,szSave, MAX_STRLEN); 
		LoadString(hInstance,IDS_EXIT_TXT,szExitTxt, MAX_STRLEN);
		LoadString(hInstance,IDS_EXIT_TTL,szExitTtl, MAX_STRLEN);
		LoadString(hInstance,IDS_ERROR_TXT,szErrorTxt, MAX_STRLEN);
		LoadString(hInstance,IDS_ERROR,szError, MAX_STRLEN);
		LoadString(hInstance,IDS_OPTIMAL,szOptVal, MAX_STRLEN);
		LoadString(hInstance,IDS_INTER_VAL,szInterVal, MAX_STRLEN);
		LoadString(hInstance,IDS_FILLED,szIsFill, MAX_STRLEN);
		LoadString(hInstance,IDS_ERROR_PNT,szErrorPnt, MAX_STRLEN);
		LoadString(hInstance,IDS_RADIUS,szRadius, MAX_STRLEN);
		LoadString(hInstance,IDS_COST,szCost, MAX_STRLEN);
		LoadString(hInstance,IDS_COUNT,szCount, MAX_STRLEN);
		LoadString(hInstance,IDS_SQ_OVERFILL,szSqOvrf, MAX_STRLEN);
		LoadString(hInstance,IDS_SQ_OVRL,szSqOvrl, MAX_STRLEN);
		LoadString(hInstance,IDS_SQ_BOX,szSqBox, MAX_STRLEN);
		LoadString(hInstance,IDS_3D_MODE,sz3DMode,MAX_STRLEN);
		LoadString(hInstance,IDS_2D_MODE,sz2DMode,MAX_STRLEN);
		LoadString(hInstance,IDS_PROJ_MODE,szProjMode,MAX_STRLEN);
		LoadString(hInstance,IDS_ALGO_MODE,szAlgoMode,MAX_STRLEN);
		LoadString(hInstance,IDS_VIS_MODE,szVisMode,MAX_STRLEN);
}

LRESULT CALLBACK ClntWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) //оконная процедура клиентского окна
{  
	HDC			hdc;
	PAINTSTRUCT ps;
	SCROLLINFO  si;
	static int  h1=0,v1=0;
	static bool mm=0;
          
	switch (iMsg)
	{
		case WM_CREATE:
		{	
			//заполнение структуры, отвечающей за полосы прокрутки
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 25;
			si.nPage  = 10; 
			si.nPos   = vsh=hsh=1; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		}
		case WM_SIZE: 
		{
			RECT rc;
			GetClientRect(hwnd,&rc);

			//изменение структуры, отвечающей за полосы прокрутки
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = hsh=0;//rc.bottom/2; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			h1=hsh;

			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = vsh=0;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			v1=vsh;
		}break;
		case WM_LBUTTONDOWN:
			if (inter)
			{
				long x;
				bool ok=GetFieldBound();
				

				if (ok) 
				{
					ok=GetWinInt(hICX0Wnd,&x);

					if (x>0)
						tcl.set_radius(x);
					else
						ok=0;
				}
				
				if (ok) 
				{
					ok=GetWinInt(hICY0Wnd,&x);
					tcl.set_cost(x);
				}

				if (!ok)
					MessageBox(hwnd,szErrorTxt, szError, MB_OK | MB_ICONEXCLAMATION);
				else
					mm=1;

				tcl.move_by(LOWORD(lParam)+h1,HIWORD(lParam)+v1);
				InvalidateRect(hwnd,NULL,true);
			}
			break;
		case WM_LBUTTONUP:
			if (inter && mm)
			{
				POINT mpt;
				mpt.x=(LOWORD(lParam)+h1);
				mpt.y=(HIWORD(lParam)+v1);
				if (OutOfBound(tcl,border))
					MessageBox(hwnd,szErrorPnt, szError, MB_OK | MB_ICONEXCLAMATION);
				else
					AddCircle(mpt,tcl.get_radius(),tcl.get_cost());
				SaveToTxtFile();
				if (IsFilled(pCircle, nCircle, border)) indic=RGB(20,200,20);
				else indic=RGB(200,20,20);
				mm=0;
			}
			break;
		case WM_MOUSEMOVE:
			if (mm && inter) 
			{
				tcl.move_by(LOWORD(lParam)+h1,HIWORD(lParam)+v1);
				InvalidateRect(hwnd,NULL,true);
			}
			break;
		case WM_PAINT: //прорисовка
			hdc = BeginPaint(hwnd, &ps);
			
			if (bPt.x!=0)
			{
				border->move_by(h1,v1);
				border->draw(hdc);
			}
			if (nCircle!=0)
			{
				for (int ic=0; ic<nCircle; ic++)
				{
					pCircle[ic].shift_by(h1,v1);
					pCircle[ic].draw(hdc);
				}				
			}
			if (inter && mm)
			{
				tcl.shift_by(h1,v1);
				tcl.draw(hdc);
			}

			EndPaint(hwnd, &ps);
			break;
		case WM_HSCROLL : //производимые действия над горизонтальной полосой прокрутки
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         hsh += 15; 
						 break;
                    case SB_LINEDOWN : 
                         hsh = min(5255, hsh + 1);
                         break; 
                    case SB_PAGEUP : 
						 hsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         hsh = max(0, hsh - 1); 
                         break; 
                    case SB_TOP : 
                         hsh = 0; 
                         break; 
                    case SB_BOTTOM : 
                         hsh = 5255;
                         break; 
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         hsh = HIWORD(wParam); 
                         break; 
 
                    default: 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = hsh; 
			   h1=hsh;
			   SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
	    case WM_VSCROLL : //производимые действия над вертикальной полосой прокрутки
               
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         vsh += 15; 
						 break;                           
                    case SB_LINEDOWN : 
                         vsh = min(5255, vsh + 1); 
                         break;  
                    case SB_PAGEUP : 
						 vsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         vsh = max(0, vsh - 1); 
                         break;  
                    case SB_TOP : 
                         vsh = 0; 
                         break;  
                    case SB_BOTTOM : 
                         vsh = 5255;
                         break;  
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         vsh = HIWORD(wParam); 
                         break; 
 
                    default : 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = vsh; 
			   v1=vsh;
			   SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
		case WM_DESTROY : 
			PostQuitMessage(0); 
			return 0; 
		default: 
			return DefWindowProc(hwnd, iMsg, wParam, lParam); 
	}

	return 0;
} 

bool GetFieldBound()
{
	long x,y,z;

	if(!GetWinInt(hCLengthWnd,&x))
		return false;

	if(!GetWinInt(hCHeightWnd,&y))
		return false;

	if (GetVideoMode()==V_3D || GetVideoMode()==V_PROJ)
	{
		if(!GetWinInt(hCWidthWnd,&z))
			return false;

		if (x!=0 && y!=0 && z!=0)
		{
			if (GetVideoMode()==V_3D)
				oglSetDimn(x,y,z);
			else
				fltSetDimn(x,y,z);
		}
	}

	if ((x!=0 && y!=0) && (bPt.x!=x || bPt.y!=y)) 
	{
		bPt.x=x;
		bPt.y=y;
		border->resize(bPt.x,bPt.y);
	}

	return true;
}

void AddCircle(POINT pt, int r, int c)
{
	nCircle++;
	circle* cc=new circle[nCircle];
	int cst=0;
	HCURSOR hCur, hOldCur;

	if ((nCircle-1)!=0)
	{
		for (int i=0; i<(nCircle-1); i++)
			cc[i]=pCircle[i];
	}

	delete[] pCircle; 

	cc[(nCircle-1)].move_by(pt.x,pt.y);
	cc[(nCircle-1)].set_number((nCircle-1));
	cc[(nCircle-1)].set_radius(r);
	cc[(nCircle-1)].set_cost(c);
	cc[(nCircle-1)].set_color(RGB(0,0,0));

	pCircle=new circle[nCircle];

	for (int i=0; i<nCircle; i++)
			pCircle[i]=cc[i];
	
	char* bfr=new char[8];
	ZeroMemory(bfr,sizeof(bfr));
	_itoa(nCircle,bfr,10);
	SetWindowText(hICY1Wnd,bfr);	

	for (int i=0; i<nCircle; i++)
		cst+=pCircle[i].get_cost();

	ZeroMemory(bfr,sizeof(bfr));
	_itoa(cst,bfr,10);
	SetWindowText(hICX1Wnd,bfr);

	ZeroMemory(bfr,sizeof(bfr));
	_itoa(BoxSquare(border),bfr,10);
	SetWindowText(hICXSWnd,bfr);
	_itoa(OvrlSquare(pCircle,nCircle),bfr,10);
	SetWindowText(hICYSWnd,bfr);
	delete[] bfr;
	hCur=LoadCursor(hInstance,IDC_WAIT);
	hOldCur=SetCursor(hCur);
	bfr=ftoa(EfficientSquare(pCircle,nCircle,border),10);
	SetWindowText(hICZSWnd,bfr);
	SetCursor(hOldCur);
	
	delete[] bfr;
	delete[] cc;
}

bool OutOfBound(circle pC, box* pB)
{
	bool b=0;
	int l=pB->length();
	int w=pB->width();

	if ((pC.get_x()-10)>l || (pC.get_x()-10)<0) b=1;
	if (!b && ((pC.get_y()-10)>w || (pC.get_y()-10)<0)) b=1;

	return b;
}

int GetCirclesOptVar(POINT* pt, int n)
{
	delete[] pCircle;
	int ov=GetOptVariant(pt,n,border->width(),border->length());
	FillVCoef2D(pt,n,border->width(),border->length());
	POINT* cPt=GetPoints(border->width(),border->length(),pt[ov].x,&nCircle);

	pCircle=new circle[nCircle];
	for(int i=0; i<nCircle; i++)
	{
		pCircle[i].move_by(cPt[i].x+10,cPt[i].y+10);
		pCircle[i].set_number(i);
		pCircle[i].set_radius(pt[ov].x);
		pCircle[i].set_cost(pt[ov].y);
		pCircle[i].set_color(RGB(0,0,0));
	}

	delete[] cPt;
	return ov;
}

int GetSphereOptVar(POINT* pt, int n)
{
	double l, h, w;
	
	if (GetVideoMode()==V_3D)
		oglGetDimn(l,h,w);

	if (GetVideoMode()==V_PROJ)
		fltGetDimn(l,h,w);
	
	return GetSphereOptVar(pt,n,l,h,w);
}

int GetSphereOptVar(POINT* pt, int n, int l, int h, int w)
{
	int ov=GetOptVariant3d(pt,n,l,h,w);
	FillVCoef3D(pt,n,l,h,w);

	if (pSphere!=NULL)
	{
		VirtualFree(pSphere,0,MEM_RELEASE);
		pSphere=NULL;
		nSphere=0;
	}
	
	POINT3D* cPt=GetPoints3d(h,l,w,pt[ov].x,&nSphere);
	SaveToTxtFile("points_3d.txt",cPt,nSphere); // may be re\moved
	int alloc_space=sizeof(OglSphere)*nSphere;
	//VirtualAlloc <= malloc <= new
	pSphere=(OglSphere*)VirtualAlloc(NULL,alloc_space, MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE); 
	for(int i=0; i<nSphere; i++)
	{
		pSphere[i].move_to(cPt[i].x,cPt[i].y,cPt[i].z);
		pSphere[i].set_radius(pt[ov].x);
		pSphere[i].set_cost(pt[ov].y);
		pSphere[i].set_color(0.11,0.15,0.11,.6f);
	}

	VirtualFree(cPt,0,MEM_RELEASE);
	return   ov;
}

void SaveToTxtFile() //сохранение точек в файл
{
	FILE* f=fopen("points.txt","w");
	for (int i=0; i<nCircle; i++)
		fprintf(f,"Точка %d: X=%d, Y=%d\n",i,pCircle[i].get_x(),pCircle[i].get_y());
	fprintf(f,"==========================================================\n");
	long x=0;
	if (inter)
		GetWinInt(hICX1Wnd,&x);
	else 
		GetWinInt(hCY0Wnd,&x);
	fprintf(f,"Общая стоимость: %d ",x); 
	fclose(f);
}

void SaveToTxtFile(const char* file, const POINT3D* pt, int s) //сохранение точек в файл
{
	int k=1;
	if (GetVideoMode()!=V_2D) k=(-1);

	FILE* f=fopen(file,"w");
	for (int i=0; i<s; i++)
		fprintf(f,"Точка %d: X=%f, Y=%f, Z=%f\n",i,pt[i].x,pt[i].y*k,pt[i].z);
	fprintf(f,"==========================================================\n");
	long x=0;
	if (inter)
		GetWinInt(hICX1Wnd,&x);
	else 
		GetWinInt(hCY0Wnd,&x);
	fprintf(f,"Общая стоимость: %d ",x); 
	fclose(f);
}

void SaveToEmfFile() //сохранение графики в файл
{
	of.lpstrFile = szFileName;
	of.Flags = OFN_OVERWRITEPROMPT;
	
	if (GetSaveFileName(&of))
	{
		HDC hdc=CreateEnhMetaFile(NULL,of.lpstrFile,NULL,NULL);

		if (bPt.x!=0)
			border->draw(hdc);
		if (nCircle!=0)
			for (int ic=0; ic<nCircle; ic++)
			{
				pCircle[ic].draw(hdc);
			}

		HENHMETAFILE hTemp=CloseEnhMetaFile(hdc);
        DeleteEnhMetaFile(hTemp);
	}
}


bool GetFieldsValue(POINT* var)
{
	bool ok=true;

	ok&=GetWinInt(hCX1Wnd,&(var[0].x));
	if (var[0].x==0)
	{
		SetWindowText(hCX1Wnd,"");
		SetWindowText(hCY1Wnd,"");
		SetWindowText(hCAWnd[0],"");
		SetWindowText(hCBWnd[0],"");
	}

	ok&=GetWinInt(hCY1Wnd,&(var[0].y));
	if (var[0].y==0)
	{
		SetWindowText(hCX1Wnd,"");
		SetWindowText(hCY1Wnd,"");
		SetWindowText(hCAWnd[0],"");
		SetWindowText(hCBWnd[0],"");
	}


	GetWinInt(hCX2Wnd,&(var[1].x));
	if (var[1].x==0)
	{
		SetWindowText(hCX2Wnd,"");
		SetWindowText(hCY2Wnd,"");
		SetWindowText(hCAWnd[1],"");
		SetWindowText(hCBWnd[1],"");
	}

	GetWinInt(hCY2Wnd,&(var[1].y));
	if (var[1].y==0)
	{
		SetWindowText(hCX2Wnd,"");
		SetWindowText(hCY2Wnd,"");
		SetWindowText(hCAWnd[1],"");
		SetWindowText(hCBWnd[1],"");
	}

	GetWinInt(hCX3Wnd,&(var[2].x));
	if (var[2].x==0)
	{
		SetWindowText(hCX3Wnd,"");
		SetWindowText(hCY3Wnd,"");
		SetWindowText(hCAWnd[2],"");
		SetWindowText(hCBWnd[2],"");
	}

	GetWinInt(hCY3Wnd,&(var[2].y));
	if (var[2].y==0)
	{
		SetWindowText(hCX3Wnd,"");
		SetWindowText(hCY3Wnd,"");
		SetWindowText(hCAWnd[2],"");
		SetWindowText(hCBWnd[2],"");
	}

	GetWinInt(hCX4Wnd,&(var[3].x));
	if (var[3].x==0)
	{
		SetWindowText(hCX4Wnd,"");
		SetWindowText(hCY4Wnd,"");
		SetWindowText(hCAWnd[3],"");
		SetWindowText(hCBWnd[3],"");
	}

	GetWinInt(hCY4Wnd,&(var[3].y));
	if (var[3].y==0)
	{
		SetWindowText(hCX4Wnd,"");
		SetWindowText(hCY4Wnd,"");
		SetWindowText(hCAWnd[3],"");
		SetWindowText(hCBWnd[3],"");
	}

	GetWinInt(hCX5Wnd,&(var[4].x));
	if (var[4].x==0)
	{
		SetWindowText(hCX5Wnd,"");
		SetWindowText(hCY5Wnd,"");
		SetWindowText(hCAWnd[4],"");
		SetWindowText(hCBWnd[4],"");
	}

	GetWinInt(hCY5Wnd,&(var[4].y));
	if (var[4].y==0)
	{
		SetWindowText(hCX5Wnd,"");
		SetWindowText(hCY5Wnd,"");
		SetWindowText(hCAWnd[4],"");
		SetWindowText(hCBWnd[4],"");
	}

	return ok;
}

void FieldWnd_ShowWidth(bool b)
{
	EnableWindow(hCWidthWnd,b);
}

void SwapWindow(int m)
{
	if (m==0 || m==2)
	{
		ShowWindow(hClntWnd,SW_HIDE);
		ShowWindow(hAlgoMode,SW_SHOW);
		SendMessage(hInterSwWnd, BM_SETCHECK, 0, 0L);
		SendMessage(hMainWnd,WM_COMMAND,0,(LPARAM)hInterSwWnd);
		EnableWindow(hInterSwWnd,0);
	}
	else
	{
		ShowWindow(hClntWnd,SW_SHOW);
		
		SendMessage(hAltrAlgo, BM_SETCHECK, 0, 0L);
		SendMessage(hStdAlgo, BM_SETCHECK, 1, 0L);
		SetAlgoMode((AMODE)0);
		ShowWindow(hAlgoMode,SW_HIDE);

		EnableWindow(hInterSwWnd,1);
	}

	if (m==1)
	{
		strcpy(szMeterVol,"м^2");
		LoadString(hInstance,IDS_SQ_OVRL,szSqOvrf, MAX_STRLEN);
	}
	else
	{
		strcpy(szMeterVol,"м^3");
		LoadString(hInstance,IDS_VQ_OVRL,szSqOvrf, MAX_STRLEN);
	}

	
	SetWindowText(hTYSWnd,szSqOvrf);
	SetWindowText(hMXSWnd,szMeterVol);
	SetWindowText(hMYSWnd,szMeterVol);

	for(int i=0; i<5; i++)
	{
		SetWindowText(hMAWnd[i],GetTitle("ед./",szMeterVol));
	}
}

void	FillVCoef2D(POINT* pt, int n, int l, int h)
{
	int nSqr=l*h;
	float fOSqr=0;
	float fCCoef=0;
	float fSCoef=0;

	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02;

	for(int i=0; i<n; i++)
	{
		int r=pt[i].x;
		int nn=NBlocks3d(h,r)*NBlocks3d(l,r);

		fOSqr=M_PI*powf(r*k,2.0)*nn;
		fCCoef=pt[i].y/(float)nSqr;
		fSCoef=fOSqr/(float)nSqr;
		SetWindowText(hCAWnd[i],ftoa(fCCoef,10));
		SetWindowText(hCBWnd[i],ftoa(fSCoef,10));
	}

}
void FillVCoef3D(POINT* pt, int n, int l, int h, int w)
{
	int nVol=l*h*w;
	float fOVol=0;
	float fCCoef=0;
	float fVCoef=0;

	float k=1.f;

	//if (GetAlgoMode()==A_ALT) k=1.02;

	for(int i=0; i<n; i++)
	{
		int r=pt[i].x;
		int nn=NBlocks3d(h,r)*NBlocks3d(l,r)*NBlocks3d(w,r);

		fOVol=(3.0/4.0)*M_PI*powf(r*k,3.0)*nn;
		fCCoef=pt[i].y/(float)nVol;
		fVCoef=fOVol/(float)nVol;
		SetWindowText(hCAWnd[i],ftoa(fCCoef,10));
		SetWindowText(hCBWnd[i],ftoa(fVCoef,10));
	}
}