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

#include "videom.h"
#include "mn.h"
#include "ogle.h"
#include "flate.h"

const UINT IDT_TIMER=1001;

WNDPROC PVM_WndProc=NULL;
LRESULT CALLBACK VideoWndProc  (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RenderWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
HRESULT CreateOpenGLWin(HWND prnt, HWND hwnd, HINSTANCE hInst);

VMODE   cvm=V_2D;
VMODE   pvm=V_2D;
bool    active=true;
int		v_vsh=0, v_hsh=0;

//------3D Window----------
HWND h3DWin;

bool vmodSetFocus()
{
	if (active)
	{
		SetFocus(h3DWin);
		return true;
	}
	else
		return false;
}

void VM_WndProcAssign(HWND hwnd)
{
	 PVM_WndProc=(WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC);
	 SetWindowLongPtr(hwnd,GWL_WNDPROC,(LONG_PTR)VideoWndProc);
}

LRESULT CALLBACK VideoWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	
	switch (iMsg)
	{
		case WM_COMMAND :
		 {
			wmId = GetWindowLong((HWND) lParam, GWL_ID); //wParam
			pvm=cvm;
			cvm=(VMODE)wmId;

			if (cvm==V_2D)
			{
				FieldWnd_ShowWidth(false);
			}
			else
				FieldWnd_ShowWidth(true);

			HINSTANCE hInst=GetModuleHandle(NULL);
			HWND hPrnt=GetParent(hwnd);
			SetVideoMode(cvm,hPrnt,h3DWin,hInst);
		 }
		default: 
			return 	CallWindowProc(PVM_WndProc,hwnd, iMsg, wParam, lParam); 
	}

	return 0;
}

LRESULT CALLBACK RenderWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO  si;
	static int  h1=0,v1=0;
	static float sh1=.1f;

	switch (uMsg)                             // Проверка сообщения для окна
	{
		case WM_CREATE:
		{
			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 25;
			si.nPage  = 10; 
			si.nPos   = v_vsh=v_hsh=1; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SetScrollRange(hwnd, SB_HORZ, 0, 255, FALSE); 
			SetScrollPos  (hwnd, SB_HORZ, 0, FALSE); 

			SetTimer(hwnd, IDT_TIMER, 30, oglRotate);
			active=true;

			return 0;
		}
		case WM_ACTIVATE:                     // Проверка сообщения активности окна
		{
			if( !HIWORD( wParam ) )           // Проверить состояние минимизации
			{
				active = true;                // Программа активна
			}
			else
			{
				active = false;               // Программа теперь не активна
			}
			return 0;                         // Возвращаемся в цикл обработки сообщений
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
		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			return 0;
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
					sh1-=0.01f;
					if (sh1<0.01f) sh1=0.1f;
					oglSetMagn(sh1);//1/abs(sh1));
					break;
				case VK_OEM_PLUS:
					sh1+=0.01f;
					//if (sh1==0) sh1+=0.01f;
					oglSetMagn(sh1);
					break;

				case VK_SPACE:
					oglToggleRotate();
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
			si.nPos   = v_hsh=0;//rc.bottom/2; 
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			h1=v_hsh;

			si.cbSize = sizeof(si); 
			si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
			si.nMin   = 0; 
			si.nMax   = 2000;
			si.nPage  = 10; 
			si.nPos   = v_vsh=0;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			v1=v_vsh;

			ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );  // Младшее слово=Width, старшее слово=Height
			return 0;            // Возвращаемся
		}
		case WM_HSCROLL : //производимые действия над горизонтальной полосой прокрутки
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         v_hsh += 15; 
						 break;
                    case SB_LINEDOWN : 
                         v_hsh = min(5255, v_hsh + 1);
                         break; 
                    case SB_PAGEUP : 
						 v_hsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         v_hsh = max(0, v_hsh - 1); 
                         break; 
                    case SB_TOP : 
                         v_hsh = 0; 
                         break; 
                    case SB_BOTTOM : 
                         v_hsh = 5255;
                         break; 
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         v_hsh = HIWORD(wParam); 
                         break; 
 
                    default: 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = v_hsh; 
			   h1=v_hsh;
			   SetScrollInfo(hwnd, SB_HORZ, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
	    case WM_VSCROLL : //производимые действия над вертикальной полосой прокрутки
               
               switch(LOWORD(wParam)) 
                    { 
                    case SB_PAGEDOWN : 
                         v_vsh += 15; 
						 break;                           
                    case SB_LINEDOWN : 
                         v_vsh = min(5255, v_vsh + 1); 
                         break;  
                    case SB_PAGEUP : 
						 v_vsh -= 15; 
                         break;                        
                    case SB_LINEUP : 
                         v_vsh = max(0, v_vsh - 1); 
                         break;  
                    case SB_TOP : 
                         v_vsh = 0; 
                         break;  
                    case SB_BOTTOM : 
                         v_vsh = 5255;
                         break;  
                    case SB_THUMBPOSITION : 
                    case SB_THUMBTRACK : 
                         v_vsh = HIWORD(wParam); 
                         break; 
 
                    default : 
                         break; 
                    }
			   si.cbSize = sizeof(si); 
			   si.fMask  = SIF_POS; 
			   si.nPos   = v_vsh; 
			   v1=v_vsh;
			   SetScrollInfo(hwnd, SB_VERT, &si, TRUE); 
			   InvalidateRect(hwnd,NULL,1);
               break; 
		case WM_DESTROY:
		{
			active=false;
			KillTimer(hwnd, IDT_TIMER);
			return 0;
		}
	}
	
	// oglDraw();
	// пересылаем все необработанные сообщения DefWindowProc
	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

VMODE GetVideoMode()
{
	return cvm;
}

HRESULT SetVideoMode(VMODE m, HWND prnt, HWND hwnd, HINSTANCE hinst)
{
	SwapWindow((int)m);
	
	if (m!=pvm)
	{
		if (cvm==V_3D)
		{
			if (fltIsActive())
				KillFLATEWindow();
			return CreateOpenGLWin(prnt,hwnd,hinst);
		}
		else
		{
			if (oglIsActive())
				KillGLWindow();
		}

		if (cvm==V_PROJ)
			return CreateFLATEWindow(hinst,prnt);
		else
		{
			if (fltIsActive())
				KillFLATEWindow();
		}
	}

	return S_OK;
}


HRESULT CreateOpenGLWin(HWND prnt, HWND hwnd, HINSTANCE hInst)
{
	if (!CreateGLWindow(prnt,hInst,RenderWndProc,"OGL",800,580,32))
		return E_FAIL;
	else
		return S_OK;
}

void vmodDraw()
{
	if (cvm==V_3D)
		oglDraw();
}

bool vmodIsActive()
{
	return active;
}

