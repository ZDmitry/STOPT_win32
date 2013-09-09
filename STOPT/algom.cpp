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

#include "algom.h"

WNDPROC PAM_WndProc=NULL;
LRESULT CALLBACK AlgoWndProc   (HWND, UINT, WPARAM, LPARAM);

AMODE  cam=A_STD;
//AMODE  pam=A_STD;

void AM_WndProcAssign(HWND hwnd)
{
	 PAM_WndProc=(WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC);
	 SetWindowLongPtr(hwnd,GWL_WNDPROC,(LONG_PTR)AlgoWndProc);
}

LRESULT CALLBACK AlgoWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	
	switch (iMsg)
	{
		case WM_COMMAND :
		 {
			wmId = GetWindowLong((HWND) lParam, GWL_ID); //wParam
			//pam=cam;
			SetAlgoMode((AMODE)wmId);
		 }
		 break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default: 
			return 	CallWindowProc(PAM_WndProc,hwnd, iMsg, wParam, lParam); 
	}

	return 0;
}

void  SetAlgoMode(AMODE m)
{
	cam=m;
}

AMODE GetAlgoMode()
{
	return cam;
}