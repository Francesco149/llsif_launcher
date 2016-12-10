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

#include "win32_gui.h"
#include <commctrl.h>
#include <stdio.h>

/*
 * prints to stdout an arbitrary error message followed by GetLastError and the error code and message.
 * if GetLastError is zero, the call will be ignored and no message will be displayed.
 */
void win32_show_error(const char *msg) {
	DWORD gle;
	char *gle_msg;

	gle = GetLastError();
	if (!gle) {
		return;
	}

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, gle,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&gle_msg,
		0, NULL);

	printf("%s, GLE=%.08X (%lu): %s\n", msg, gle, gle, gle_msg);
	LocalFree(gle_msg);
}

/* creates a child window with the default background color */
HWND win32_make_child_window_ex(HWND parent, const char *classname, const char *title, HINSTANCE inst,
	WNDPROC wndproc, HICON icon, int w, int h, int cmdshow, int exstyle, int style) {

	WNDCLASSEXA wc = { 0 };
	HWND wnd;
	char buf[128] = { 0 };

	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.lpfnWndProc = wndproc;
	wc.hInstance = inst;
	wc.hIcon = icon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = classname;
	wc.hIconSm = wc.hIcon;

	if (!RegisterClassExA(&wc)) {
		sprintf_s(buf, 128, "RegisterClassExA failed (%s)", classname);
		win32_show_error(buf);
		return NULL;
	}

	wnd = CreateWindowExA(
		exstyle,
		classname,
		title,
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		w, h, parent, NULL, inst, NULL);

	if (wnd == NULL) {
		sprintf_s(buf, 128, "CreateWindowExA failed (%s)", classname);
		win32_show_error(buf);
		return NULL;
	}

	ShowWindow(wnd, cmdshow);
	UpdateWindow(wnd);
	return wnd;
}

/* runs a blocking win32 message loop and returns the last message's wparam */
int win32_message_loop(HWND wnd) {
	MSG msg;

	while (GetMessageA(&msg, NULL, 0, 0)) {
		if (!IsDialogMessageA(wnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	return msg.wParam;
}

/*
 * processes all messages currently in the message
 * queue and returns the last message's wparam
 */
int win32_clear_message_queue(HWND wnd) {
	MSG msg;

	while (PeekMessage(&msg, wnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

/* returns the components xy position relative to the parent's client rect */
void win32_get_client_xy(HWND parent, int componentid, POINT *dst) {
	HWND component;
	RECT componentrect;

	component = GetDlgItem(parent, componentid);
	GetWindowRect(component, &componentrect); /* get component screen rect (x, y, x + width, y + height) */

	/* convert component xy to xy relative to the parent's client area */
	dst->x = componentrect.left;
	dst->y = componentrect.top;
	ScreenToClient(parent, dst);
}

int win32_listview_resize_column(HWND list, int colindex, int width);

/* expands the given component to the right to match its parent's width with a margin */
void win32_expand_horizontal(HWND parent, int componentid, int margin, int componentheight) {
	RECT wndrect;
	POINT lt;
	char compclass[MAX_PATH] = { 0 };
	HWND comp;

	/* get component xy relative to the parent's client rect */
	win32_get_client_xy(parent, componentid, &lt);

	/*
	* calculate final width of the component
	* by subtracting component x and the margin from the parent's rect width
	*/
	GetClientRect(parent, &wndrect); /* get window client rect (0, 0, width, height) */
	win32_resize(parent, componentid, wndrect.right - margin - lt.x, componentheight);

	/* expand last column if it's a listview */
	comp = GetDlgItem(parent, componentid);
	GetClassNameA(comp, compclass, MAX_PATH);
	if (strcmp(compclass, WC_LISTVIEWA) == 0) {
		win32_listview_resize_column(comp, -1, wndrect.right - margin - lt.x - 20);
	}
}

/* expands the given component to the right and bottom to match its parent's width with a margin */
void win32_expand(HWND parent, int componentid, int marginright, int marginbottom) {
	RECT wndrect;
	POINT lt;
	char compclass[MAX_PATH] = { 0 };
	HWND comp;

	/* get component xy relative to the parent's client rect */
	win32_get_client_xy(parent, componentid, &lt);

	/*
	* calculate final width of the component
	* by subtracting xy and the margin from the parent's rect width/height
	*/
	GetClientRect(parent, &wndrect); /* get window client rect (0, 0, width, height) */
	win32_resize(parent, componentid,
		wndrect.right - marginright - lt.x,
		wndrect.bottom - marginbottom - lt.y);

	/* expand last column if it's a listview */
	comp = GetDlgItem(parent, componentid);
	GetClassNameA(comp, compclass, MAX_PATH);
	if (strcmp(compclass, WC_LISTVIEWA) == 0) {
		win32_listview_resize_column(comp, -1, wndrect.right - marginright - lt.x - 20);
	}
}

/* makes the component stick to the right edge of the parent window with a margin */
void win32_dock_right(HWND parent, int componentid, int componentwidth, int marginright) {
	RECT wndrect;
	POINT lt;
	win32_get_client_xy(parent, componentid, &lt);
	GetClientRect(parent, &wndrect);
	win32_move(parent, componentid, wndrect.right - marginright - componentwidth, lt.y);
}

/* makes the component stick to the bottom edge of the parent window with a margin */
void win32_dock_bottom(HWND parent, int componentid, int componentheight, int marginbottom) {
	RECT wndrect;
	POINT lt;
	size_t i = 0;

	win32_get_client_xy(parent, componentid, &lt);
	GetClientRect(parent, &wndrect);

	/* for some reason there's 1 extra pixel at the bottom */
	win32_move(parent, componentid, lt.x, wndrect.bottom - marginbottom - componentheight + 1);
}

/*
 * creates a win32 control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
HWND win32_make_control(HWND parent, int controlid, const char *class,
	const char *name, const char *title, int x, int y, int w, int h,
	DWORD style, DWORD exstyle) {

	HWND res;
	char buf[128];

	res = CreateWindowExA(
		exstyle, class, title,
		style,
		x, y, w, h,
		parent, (HMENU)controlid,
		GetModuleHandle(NULL), NULL);

	if (!res) {
		sprintf_s(buf, 128, "CreateWindowExA failed (%s)", name);
		win32_show_error(buf);
	}
	else {
		SendMessage(res, WM_SETFONT,
			(WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
	}

	return res;
}

/* adds a column to a listview control */
int win32_listview_add_column(HWND list, int index, const char *title, int width) {
	LVCOLUMNA col;
	int res;
	char tmp[MAX_PATH] = { 0 };

	strcpy_s(tmp, MAX_PATH, title);
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = width;
	col.pszText = tmp;
	col.iSubItem = index;
	res = ListView_InsertColumn(list, index, &col);
	if (res == -1) {
		win32_show_error("ListView_InsertColumn failed");
	}

	return res;
}

/*
 * sets the width of the given listview's column.
 * if colindex is -1, the last column will be automatically obtained.
 * returns the index of the affected column or -1 if unsuccessful.
 */
int win32_listview_resize_column(HWND list, int colindex, int width) {
	HWND header = NULL;

	/* auto-obtain last column */
	if (colindex == -1) {
		header = ListView_GetHeader(list);
		if (!header) {
			win32_show_error("ListView_GetHeader failed");
			return -1;
		}
		colindex = Header_GetItemCount(header) - 1;
		if (colindex == -1) {
			win32_show_error("Header_GetItemCount failed");
			return -1;
		}
	}

	if (!ListView_SetColumnWidth(list, colindex, width)) {
		return -1;
	}

	return colindex;
}