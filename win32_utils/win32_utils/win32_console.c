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
#include "win32_console.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define MAX_CONSOLE_LINES 500

/* allocates a console in a Win32 GUI application and redirects stdout to it */
BOOL win32_console_create() {
	int conhandle;
	long stdhandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	BOOL ret;

	ret = AllocConsole();
	if (!ret) {
		return ret;
	}

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	stdhandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	conhandle = _open_osfhandle(stdhandle, _O_TEXT);

	fp = _fdopen(conhandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
	stdhandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	conhandle = _open_osfhandle(stdhandle, _O_TEXT);

	fp = _fdopen(conhandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);
	stdhandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	conhandle = _open_osfhandle(stdhandle, _O_TEXT);

	fp = _fdopen(conhandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);

	return ret;
}