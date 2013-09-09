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

#include <stdexcept>
#include <windows.h>
#include "time_m.h"


HiResTimer::HiResTimer()
//:counterPeriod
{
	LARGE_INTEGER frequency;
	BOOL success = QueryPerformanceFrequency(&frequency);

	if(!success) throw std::runtime_error("No high-resolution performance counters installed");
	counterPeriod_ = 1.0/frequency.QuadPart;
}

__int64 HiResTimer::queryCounter()
{
	LARGE_INTEGER counterValue;
	QueryPerformanceCounter(&counterValue);

	return counterValue.QuadPart;
}

double HiResTimer::toSeconds(__int64 counterValue) const
{
	return counterValue*counterPeriod_;
}

double HiResTimer::getCounterPeriod()
{
	return this->toSeconds(1);
}
