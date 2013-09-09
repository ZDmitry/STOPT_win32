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

#include "flate.h"
#include "analitycs.h"
#include "str_pp.h"

LRESULT CALLBACK SideWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FrontWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TopWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

int     PL_BOX=1, PH_BOX=1, PW_BOX=1;
int		C_RADIUS=0;
int     M_SCALE=10;

bool    bActive=false;

int		fs_vsh=0, fs_hsh=0;
int		ff_vsh=0, ff_hsh=0;
int		ft_vsh=0, ft_hsh=0;

char    szSide[120]="Профильная  -  x";
char    szFront[120]="Фронтальная  -  x";
char    szTop[120]="Горизонтальная  -  x";

HINSTANCE hGInsta=NULL;
HWND hGPrnt=NULL;
HWND hGSideWnd=NULL;
HWND hGFrontWnd=NULL;
HWND hGTopWnd=NULL;

bool fltIsActive()
{
	return bActive;
}

void fltSetDimn(int l, int h, int w)
{
	PL_BOX=l;
	PW_BOX=w;
	PH_BOX=h;
}
void fltGetDimn(double& l, double& h, double& w)
{
	l=PL_BOX;
	w=PW_BOX;
	h=PH_BOX;
}

int fltGetVolume()
{
	return (PL_BOX*PW_BOX*PH_BOX);
}

void fltSetOptVar(int r)
{
	C_RADIUS=r;
}

void fltUpZOrder(HWND hWnd)
{
	SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	InvalidateRect(hGPrnt,NULL,0);
}

void fltSetTitle(HWND hwnd, int x)
{
	if (hwnd==hGSideWnd)
		SetWindowText(hwnd, GetTitle(szSide,x));
	if (hwnd==hGFrontWnd)
		SetWindowText(hwnd, GetTitle(szFront,x));
	if (hwnd==hGTopWnd)
		SetWindowText(hwnd, GetTitle(szTop,x));
}

BOOL CreateFLATEWindow(HINSTANCE hInst, HWND hPrnt)
{
	WNDCLASSEX   wcls;

	//ClntWndClass - клас клиентского окна
	wcls.cbSize			= sizeof(wcls); 
	wcls.style			= CS_HREDRAW | CS_VREDRAW;
	wcls.lpfnWndProc	= DefWindowProc;
	wcls.cbClsExtra		= 0;
	wcls.cbWndExtra		= 0;
	wcls.hInstance		= hInst;
	wcls.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcls.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcls.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcls.lpszMenuName	= NULL;
	wcls.lpszClassName	= "FLATE";
	wcls.hIconSm		= LoadIcon(NULL, IDI_APPLICATION); 
	
	RegisterClassEx(&wcls); 

	hGSideWnd=CreateWindowEx(WS_EX_CLIENTEDGE, "FLATE", "",   //клиентское окно
    WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_CAPTION | WS_HSCROLL | WS_VSCROLL , 
                              0, 0, 
							  360, 280, 
                              hPrnt,NULL, hInst, NULL);

	SetWindowLongPtr(hGSideWnd,GWL_WNDPROC,(LONG_PTR)SideWndProc);

	hGFrontWnd=CreateWindowEx(WS_EX_CLIENTEDGE, "FLATE", "",   //клиентское окно
	WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_CAPTION | WS_HSCROLL | WS_VSCROLL , 
                              380, 0, 
							  360, 280, 
                              hPrnt,NULL, hInst, NULL);

	SetWindowLongPtr(hGFrontWnd,GWL_WNDPROC,(LONG_PTR)FrontWndProc);

	hGTopWnd=CreateWindowEx(WS_EX_CLIENTEDGE, "FLATE", "",   //клиентское окно
	WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_CAPTION | WS_HSCROLL | WS_VSCROLL , 
                              0, 300, 
							  360, 280, 
                              hPrnt,NULL, hInst, NULL);

	SetWindowLongPtr(hGTopWnd,GWL_WNDPROC,(LONG_PTR)TopWndProc);

	fltSetTitle(hGSideWnd,1);
	fltSetTitle(hGFrontWnd,1);
	fltSetTitle(hGTopWnd,1);

	hGInsta=hInst;
	hGPrnt=hPrnt;
	bActive=true;

	return true;
}

BOOL KillFLATEWindow()
{
	bool bRet=false;

	if(hGSideWnd && !DestroyWindow(hGSideWnd))            // Возможно ли уничтожить окно?
	{
		MessageBox( NULL, "Could Not Release hGSideWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGSideWnd = NULL;                // Установить hGWnd в NULL
		//return false;
	}
	hGSideWnd=NULL;

	if(hGFrontWnd && !DestroyWindow(hGFrontWnd))            // Возможно ли уничтожить окно?
	{
		MessageBox( NULL, "Could Not Release hGFrontWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGFrontWnd = NULL;                // Установить hGWnd в NULL
		//return false;
	}
	hGFrontWnd=NULL;

	if(hGTopWnd && !DestroyWindow(hGTopWnd))            // Возможно ли уничтожить окно?
	{
		MessageBox( NULL, "Could Not Release hGTopWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		hGTopWnd = NULL;                // Установить hGWnd в NULL
		//return false;
	}
	hGTopWnd=NULL;

	if( !UnregisterClass( "FLATE", hGInsta) )        // Возможно ли разрегистрировать класс
	{	
		
		MessageBox( NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		//hGInst = NULL;                // Установить hInstance в NULL
		//return false;
	}

	bRet=true;	
	bActive=false;
	return bRet;
}

LRESULT CALLBACK SideWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO  si;
	HDC			hdc;
	PAINTSTRUCT ps;
	static int  h1=0,v1=0, sh1=0;

	switch (uMsg)                             // Проверка сообщения для окна
	{
		case WM_CREATE:
		{
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 25;
			si.nPage  = 10; 
			si.nPos   = fs_vsh=fs_hsh=1; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SetScrollRange(hwnd, SB_HORZ, 0, 255, FALSE); 
			SetScrollPos  (hwnd, SB_HORZ, 0, FALSE); 

			return 0;
		}
		case WM_SYSCOMMAND:                   // Перехватываем системную команду
		{
			switch ( wParam )                 // Останавливаем системный вызов
			{
				case SC_SCREENSAVE:           // Пытается ли запустится скринсейвер?
				case SC_MONITORPOWER:         // Пытается ли монитор перейти в режим сбережения энергии?
				return 0;                     // Предотвращаем это
			}
			break;              // Выход
		}
		case WM_CLOSE:              // Мы получили сообщение о закрытие?
		{
			PostQuitMessage( 0 );          // Отправить сообщение о выходе
			return 0;            // Вернуться назад
		}
		case WM_PAINT: //прорисовка
		{
			if (C_RADIUS>0)
			{
				hdc = BeginPaint(hwnd, &ps);

				int nCircle=0;
				POINT* pPt=GetPoints(PH_BOX*(M_SCALE+sh1),PW_BOX*(M_SCALE+sh1),C_RADIUS*(M_SCALE+sh1),10,&nCircle);
				
				if (PW_BOX>0 && PH_BOX>0)
				{
					box border(10,10,PW_BOX*(M_SCALE+sh1),PH_BOX*(M_SCALE+sh1));
					border.move_by(h1,v1);
					border.draw(hdc);
					//delete border;
				}
				if (nCircle!=0)
				{
					for (int ic=0; ic<nCircle; ic++)
					{
						circle pCircle(pPt[ic].x,pPt[ic].y,C_RADIUS*(M_SCALE+sh1),ic+1);
						pCircle.shift_by(h1,v1);
						pCircle.draw(hdc);
						//delete pCircle;
					}				
				}
				
				if(pPt!=NULL)
					delete[] pPt;
				EndPaint(hwnd, &ps);
			}
		}
		break;
		case WM_NCLBUTTONDOWN:
		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			fltUpZOrder(hwnd);
			break;
		case WM_KEYDOWN:            // Была ли нажата кнопка?
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
					sh1--;
					if (sh1>0)
						fltSetTitle(hwnd,sh1+1);
					else
						fltSetTitle(hwnd,sh1-1);
					break;
				case VK_OEM_PLUS:
					sh1++;
					if (sh1>0)
						fltSetTitle(hwnd,sh1+1);
					else
						fltSetTitle(hwnd,sh1-1);
					break;
			}
			if (wScrollNotify != -1) 
			{
				if (!hor) SendMessage(hwnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
				else SendMessage(hwnd, WM_HSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
			}
			
			return 0;            // Возвращаемся
		}
		case WM_KEYUP:              // Была ли отпущена клавиша?
		{
			//keys[wParam] = false;          //  Если так, мы присваиваем этой ячейке false
			return 0;            // Возвращаемся
		}
		case WM_MOVE:
		{
			InvalidateRect(hGPrnt,NULL,0);
			return DefWindowProc( hwnd, uMsg, wParam, lParam );
		}
		case WM_SIZE:              // Изменены размеры OpenGL окна
		{
			RECT rc;
			GetClientRect(hwnd,&rc);

			//изменение структуры, отвечающей за полосы прокрутки
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = fs_hsh=0;//rc.bottom/2; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			h1=fs_hsh;

			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = fs_vsh=0;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			v1=fs_vsh;

			//ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );  // Младшее слово=Width, старшее слово=Height
			return 0;            // Возвращаемся
		}
		case WM_HSCROLL : //производимые действия над горизонтальной полосой прокрутки
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         fs_hsh += 15; 
						 break;
                    case SB_LINEDOWN : 
                         fs_hsh = min(5255, fs_hsh + 1);
                         break; 
                    case SB_PAGEUP : 
						 fs_hsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         fs_hsh = max(0, fs_hsh - 1); 
                         break; 
                    case SB_TOP : 
                         fs_hsh = 0; 
                         break; 
                    case SB_BOTTOM : 
                         fs_hsh = 5255;
                         break; 
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         fs_hsh = HIWORD(wParam); 
                         break; 
 
                    default: 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = fs_hsh; 
			   h1=fs_hsh;
			   SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
	    case WM_VSCROLL : //производимые действия над вертикальной полосой прокрутки
               
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         fs_vsh += 15; 
						 break;                           
                    case SB_LINEDOWN : 
                         fs_vsh = min(5255, fs_vsh + 1); 
                         break;  
                    case SB_PAGEUP : 
						 fs_vsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         fs_vsh = max(0, fs_vsh - 1); 
                         break;  
                    case SB_TOP : 
                         fs_vsh = 0; 
                         break;  
                    case SB_BOTTOM : 
                         fs_vsh = 5255;
                         break;  
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         fs_vsh = HIWORD(wParam); 
                         break; 
 
                    default : 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = fs_vsh; 
			   v1=fs_vsh;
			   SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

LRESULT CALLBACK FrontWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO  si;
	HDC			hdc;
	PAINTSTRUCT ps;
	static int  h1=0,v1=0,sh2=0;

	switch (uMsg)                             // Проверка сообщения для окна
	{
		case WM_CREATE:
		{
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 25;
			si.nPage  = 10; 
			si.nPos   = ff_vsh=ff_hsh=1; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SetScrollRange(hwnd, SB_HORZ, 0, 255, FALSE); 
			SetScrollPos  (hwnd, SB_HORZ, 0, FALSE); 

			return 0;
		}
		case WM_SYSCOMMAND:                   // Перехватываем системную команду
		{
			switch ( wParam )                 // Останавливаем системный вызов
			{
				case SC_SCREENSAVE:           // Пытается ли запустится скринсейвер?
				case SC_MONITORPOWER:         // Пытается ли монитор перейти в режим сбережения энергии?
				return 0;                     // Предотвращаем это
			}
			break;              // Выход
		}
		case WM_CLOSE:              // Мы получили сообщение о закрытие?
		{
			PostQuitMessage( 0 );          // Отправить сообщение о выходе
			return 0;            // Вернуться назад
		}
		case WM_PAINT: //прорисовка
		{
			if (C_RADIUS>0)
			{
				hdc = BeginPaint(hwnd, &ps);
				
				int nCircle=0;
				POINT* pPt=GetPoints(PH_BOX*(M_SCALE+sh2),PL_BOX*(M_SCALE+sh2),C_RADIUS*(M_SCALE+sh2),10,&nCircle);
				
				if (PL_BOX>0 && PH_BOX>0)
				{
					box border(10,10,PL_BOX*(M_SCALE+sh2),PH_BOX*(M_SCALE+sh2));
					border.move_by(h1,v1);
					border.draw(hdc);
				}
				if (nCircle!=0)
				{
					for (int ic=0; ic<nCircle; ic++)
					{
						circle pCircle(pPt[ic].x,pPt[ic].y,C_RADIUS*(M_SCALE+sh2),ic+1);
						pCircle.shift_by(h1,v1);
						pCircle.draw(hdc);
						//delete pCircle;
					}				
				}
				
				if(pPt!=NULL)
					delete[] pPt;
				EndPaint(hwnd, &ps);
			}
		}
		break;
		case WM_NCLBUTTONDOWN:
		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			fltUpZOrder(hwnd);
			break;
		case WM_MOVE:
		{
			InvalidateRect(hGPrnt,NULL,0);
			return DefWindowProc( hwnd, uMsg, wParam, lParam );
		}
		case WM_KEYDOWN:            // Была ли нажата кнопка?
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
					sh2--;
					if (sh2>0)
						fltSetTitle(hwnd,sh2+1);
					else
						fltSetTitle(hwnd,sh2-1);
					break;
				case VK_OEM_PLUS:
					sh2++;
					if (sh2>0)
						fltSetTitle(hwnd,sh2+1);
					else
						fltSetTitle(hwnd,sh2-1);
					break;
			}
			if (wScrollNotify != -1) 
			{
				if (!hor) SendMessage(hwnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
				else SendMessage(hwnd, WM_HSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
			}
			
			//keys[wParam] = true;          // Если так, мы присваиваем этой ячейке true
			return 0;            // Возвращаемся
		}
		case WM_KEYUP:              // Была ли отпущена клавиша?
		{
			//keys[wParam] = false;          //  Если так, мы присваиваем этой ячейке false
			return 0;            // Возвращаемся
		}
		case WM_SIZE:              // Изменены размеры OpenGL окна
		{
			RECT rc;
			GetClientRect(hwnd,&rc);

			//изменение структуры, отвечающей за полосы прокрутки
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = ff_hsh=0;//rc.bottom/2; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			h1=ff_hsh;

			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = ff_vsh=0;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			v1=ff_vsh;

			//ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );  // Младшее слово=Width, старшее слово=Height
			return 0;            // Возвращаемся
		}
		case WM_HSCROLL : //производимые действия над горизонтальной полосой прокрутки
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         ff_hsh += 15; 
						 break;
                    case SB_LINEDOWN : 
                         ff_hsh = min(5255, ff_hsh + 1);
                         break; 
                    case SB_PAGEUP : 
						 ff_hsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         ff_hsh = max(0, ff_hsh - 1); 
                         break; 
                    case SB_TOP : 
                         ff_hsh = 0; 
                         break; 
                    case SB_BOTTOM : 
                         ff_hsh = 5255;
                         break; 
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         ff_hsh = HIWORD(wParam); 
                         break; 
 
                    default: 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = ff_hsh; 
			   h1=ff_hsh;
			   SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
	    case WM_VSCROLL : //производимые действия над вертикальной полосой прокрутки
               
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         ff_vsh += 15; 
						 break;                           
                    case SB_LINEDOWN : 
                         ff_vsh = min(5255, ff_vsh + 1); 
                         break;  
                    case SB_PAGEUP : 
						 ff_vsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         ff_vsh = max(0, ff_vsh - 1); 
                         break;  
                    case SB_TOP : 
                         ff_vsh = 0; 
                         break;  
                    case SB_BOTTOM : 
                         ff_vsh = 5255;
                         break;  
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         ff_vsh = HIWORD(wParam); 
                         break; 
 
                    default : 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = ff_vsh; 
			   v1=ff_vsh;
			   SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

LRESULT CALLBACK TopWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO  si;
	HDC			hdc;
	PAINTSTRUCT ps;
	static int  h1=0,v1=0, sh3=0;

	switch (uMsg)                             // Проверка сообщения для окна
	{
		case WM_CREATE:
		{
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 25;
			si.nPage  = 10; 
			si.nPos   = ft_vsh=ft_hsh=1; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SetScrollRange(hwnd, SB_HORZ, 0, 255, FALSE); 
			SetScrollPos  (hwnd, SB_HORZ, 0, FALSE); 

			return 0;
		}
		case WM_SYSCOMMAND:                   // Перехватываем системную команду
		{
			switch ( wParam )                 // Останавливаем системный вызов
			{
				case SC_SCREENSAVE:           // Пытается ли запустится скринсейвер?
				case SC_MONITORPOWER:         // Пытается ли монитор перейти в режим сбережения энергии?
				return 0;                     // Предотвращаем это
			}
			break;              // Выход
		}
		case WM_CLOSE:              // Мы получили сообщение о закрытие?
		{
			PostQuitMessage( 0 );          // Отправить сообщение о выходе
			return 0;            // Вернуться назад
		}
		case WM_PAINT: //прорисовка
		{
			if (C_RADIUS>0)
			{
				hdc = BeginPaint(hwnd, &ps);
				
				int nCircle=0;
				POINT* pPt=GetPoints(PL_BOX*(M_SCALE+sh3),PW_BOX*(M_SCALE+sh3),C_RADIUS*(M_SCALE+sh3),10,&nCircle);
				
				if (PW_BOX>0 && PL_BOX>0)
				{
					box border(10,10,PW_BOX*(M_SCALE+sh3),PL_BOX*(M_SCALE+sh3));
					border.move_by(h1,v1);
					border.draw(hdc);
				}
				if (nCircle!=0)
				{
					for (int ic=0; ic<nCircle; ic++)
					{
						circle pCircle(pPt[ic].x,pPt[ic].y,C_RADIUS*(M_SCALE+sh3),ic+1);
						pCircle.shift_by(h1,v1);
						pCircle.draw(hdc);
						//delete pCircle;
					}				
				}

				if(pPt!=NULL)
					delete[] pPt;
				EndPaint(hwnd, &ps);
			}
		}
		break;
		case WM_MOVE:
		{
			InvalidateRect(hGPrnt,NULL,0);
			return DefWindowProc( hwnd, uMsg, wParam, lParam );
		}
		case WM_NCLBUTTONDOWN:
		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			fltUpZOrder(hwnd);
			break;
		case WM_KEYDOWN:            // Была ли нажата кнопка?
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
					sh3--;
					if (sh3>0)
						fltSetTitle(hwnd,sh3+1);
					else
						fltSetTitle(hwnd,sh3-1);
					break;
				case VK_OEM_PLUS:
					sh3++;
					if (sh3>0)
						fltSetTitle(hwnd,sh3+1);
					else
						fltSetTitle(hwnd,sh3-1);
					break;
			}
			if (wScrollNotify != -1) 
			{
				if (!hor) SendMessage(hwnd, WM_VSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
				else SendMessage(hwnd, WM_HSCROLL, MAKELONG(wScrollNotify, 0), 0L); 
			}
			
			//keys[wParam] = true;          // Если так, мы присваиваем этой ячейке true
			return 0;            // Возвращаемся
		}
		case WM_KEYUP:              // Была ли отпущена клавиша?
		{
			//keys[wParam] = false;          //  Если так, мы присваиваем этой ячейке false
			return 0;            // Возвращаемся
		}
		case WM_SIZE:              // Изменены размеры OpenGL окна
		{
			RECT rc;
			GetClientRect(hwnd,&rc);

			//изменение структуры, отвечающей за полосы прокрутки
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = ft_hsh=0;//rc.bottom/2; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			h1=ft_hsh;

			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = ft_vsh=0;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			v1=ft_vsh;

			//ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );  // Младшее слово=Width, старшее слово=Height
			return 0;            // Возвращаемся
		}
		case WM_HSCROLL : //производимые действия над горизонтальной полосой прокрутки
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         ft_hsh += 15; 
						 break;
                    case SB_LINEDOWN : 
                         ft_hsh = min(5255, ft_hsh + 1);
                         break; 
                    case SB_PAGEUP : 
						 ft_hsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         ft_hsh = max(0, ft_hsh - 1); 
                         break; 
                    case SB_TOP : 
                         ft_hsh = 0; 
                         break; 
                    case SB_BOTTOM : 
                         ft_hsh = 5255;
                         break; 
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         ft_hsh = HIWORD(wParam); 
                         break; 
 
                    default: 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = ft_hsh; 
			   h1=ft_hsh;
			   SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
	    case WM_VSCROLL : //производимые действия над вертикальной полосой прокрутки
               
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         ft_vsh += 15; 
						 break;                           
                    case SB_LINEDOWN : 
                         ft_vsh = min(5255, ft_vsh + 1); 
                         break;  
                    case SB_PAGEUP : 
						 ft_vsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         ft_vsh = max(0, ft_vsh - 1); 
                         break;  
                    case SB_TOP : 
                         ft_vsh = 0; 
                         break;  
                    case SB_BOTTOM : 
                         ft_vsh = 5255;
                         break;  
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         ft_vsh = HIWORD(wParam); 
                         break; 
 
                    default : 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = ft_vsh; 
			   v1=ft_vsh;
			   SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}