/*
	Copyright 2014 Franc[e]sco (lolisamurai@tfwno.gf)
	This file is part of win32_utils.
	win32_utils is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	win32_utils is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with win32_utils. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WIN32_CONSOLE_H
#define WIN32_CONSOLE_H

#include <windows.h>

/* allocates a console in a Win32 GUI application and redirects stdout to it */
BOOL win32_console_create();

#endif
