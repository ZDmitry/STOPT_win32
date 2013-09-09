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

#include "str_pp.h"

char* GetTitle(const char* s1, const char* s2, int i)
{
	char* str_ret;
	int cap=0;
	int sl1=strlen(s1);
	int sl2=strlen(s2);
	char bfr[8];
	
	ZeroMemory(bfr,sizeof(bfr));
	_itoa(i,bfr,10);
	int il=strlen(bfr);

	cap=sl1+sl2+il+2;
	str_ret=new char[cap];
	
	ZeroMemory(str_ret,sizeof(char)*cap);
	strcat(str_ret,s1);
	strcat(str_ret,s2);
	strcat(str_ret," ");
	strcat(str_ret,bfr);
	strcat(str_ret,":");

	return str_ret;
}

char* GetTitle(const char* s1, int i)
{
	char* str_ret;
	int cap=0;
	int sl1=strlen(s1);
	char bfr[8];
	
	ZeroMemory(bfr,sizeof(bfr));
	_itoa(i,bfr,10);
	int il=strlen(bfr);

	cap=sl1+il+2;
	str_ret=new char[cap];
	
	ZeroMemory(str_ret,sizeof(char)*cap);
	strcat(str_ret,s1);
	strcat(str_ret," ");
	strcat(str_ret,bfr);
	strcat(str_ret,":");

	return str_ret;

}

char* GetTitle(const char* s1,const char* s2)
{
	char* str_ret;
	int cap=0;
	int sl1=strlen(s1);
	int sl2=strlen(s2);

	cap=sl1+sl2+1;
	str_ret=new char[cap];
	
	ZeroMemory(str_ret,sizeof(char)*cap);
	strcat(str_ret,s1);
	strcat(str_ret,s2);
	strcat(str_ret,":");

	return str_ret;
}

char* GetTitle(const char* s1)
{
	char* str_ret;
	int cap=0;
	int sl1=strlen(s1);

	cap=sl1+1;
	str_ret=new char[cap];
	
	ZeroMemory(str_ret,sizeof(char)*cap);
	strcat(str_ret,s1);
	strcat(str_ret,":");

	return str_ret;
}

bool GetWinFloat(HWND src, double* i)
{
	int iText;
	char* cText;
	bool ok=true;
				
	iText=(GetWindowTextLength(src)+1);
	cText=new char[iText];
	GetWindowText(src,cText,iText);
	double r=atof(cText);

	if (strcmp(cText,"0")!=0 && r==0) 
	{
		ok=false;
		*i=0;
	}
	else
		*i=r;

	return ok;
}

bool GetWinInt(HWND src, long* i)
{
	int iText;
	char* cText;
	bool ok=true;
				
	iText=(GetWindowTextLength(src)+1);
	cText=new char[iText];
	GetWindowText(src,cText,iText);
	int r=atoi(cText);

	if (strcmp(cText,"0")!=0 && r==0) 
	{
		ok=false;
		*i=0;
	}
	else
	{
		//within if at normal
		if (r>0)
			*i=r;
		else	
		{
			ok=false;
			*i=0;
		}
	}

	return ok;
}

char* ftoa(double d, int count) //double в строку (с учетом знака и точки)
{
	int dec, sign, adec;
	char* buff, *res=new char[60];
	char  lbf[30],rbf[30];

	//double в строку (без знака и точки - это отдельными переменными)
	buff=_fcvt(d,count,&dec,&sign); 
	adec=(count-dec);
 
	//анализируем положение точки и приписываем ее в нужном месте
	 if (dec<(-20)) return "0\0"; 
	 if (dec>=0) 
	 {
		if (dec==0) strcpy(lbf,"0\0");
		else
		{
			for (int i=0; i<dec; i++)
				lbf[i]=buff[i];
  			lbf[dec]='\0';
		}

		for (int i=0; i<adec; i++)
			rbf[i]=buff[dec+i];
		rbf[adec]='\0';
		
		//если есть знак (имеется минус) - дописваем спереди
		if (!sign) strcpy(res,lbf);
		else 
		{
			strcpy(res,"-");
			strcat(res,lbf);
		}
		strcat(res,".");
		strcat(res,rbf);
	 }
	 else
	 {
		for(int i=0; i<((-1)*dec); i++)
		 lbf[i]='0';
  		lbf[(-1)*dec]='\0';

		for(int i=0; i<((-1)*dec); i++)
		 rbf[i]=buff[i];
  		rbf[(-1)*dec]='\0';

		//если есть знак (имеется минус) - дописваем спереди
		if (!sign) strcpy(res,"0");
		else 
		{
			strcpy(res,"-");
			strcat(res,"0");
		}
		strcat(res,".");
		strcat(res,lbf);
		strcat(res,rbf);
	 }	

	//возвращаем полученую строку
	return res;
}