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

#ifndef WIN32_GUI_H
#define WIN32_GUI_H

#include <Windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#pragma comment(lib, "ComCtl32.lib")

/* manifest data that enables windows xp / 7 visual styles */
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
	"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
	"processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
	"processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
	"processorArchitecture='*' publicKeyToken='6595b64144ccf1df' " \
	"language='*'\"")
#endif

/*
 * prints to stdout an arbitrary error message followed by GetLastError and the error code and message.
 * if GetLastError is zero, the call will be ignored and no message will be displayed.
 */
void win32_show_error(const char *msg);

/* creates a child window with the default background color */
HWND win32_make_child_window_ex(HWND parent, const char *classname, const char *title, HINSTANCE inst,
	WNDPROC wndproc, HICON icon, int w, int h, int cmdshow, int exstyle, int style);

/* creates a window with the default background color */
#define win32_make_window_ex(classname, title, inst, wndproc, icon, w, h, cmdshow, exstyle, style) \
	win32_make_child_window_ex(NULL, classname, title, inst, wndproc, icon, w, h, cmdshow, exstyle, style)

/* creates a window with the default background color and default extended style */
#define win32_make_window_s(classname, title, inst, wndproc, icon, w, h, cmdshow, style) \
	win32_make_window_ex(classname, title, inst, wndproc, icon, w, h, cmdshow, 0, style)

/* 
 * creates a window with the default background color, 
 * default extended style and WS_OVERLAPPEDWINDOW style 
 */
#define win32_make_window(classname, title, inst, wndproc, icon, w, h, cmdshow) \
	win32_make_window_s(classname, title, inst, wndproc, icon, w, h, cmdshow, WS_OVERLAPPEDWINDOW)

/* runs a blocking win32 message loop and returns the last message's wparam */
int win32_message_loop(HWND wnd);

/*
 * processes all messages currently in the message
 * queue and returns the last message's wparam
 */
int win32_clear_message_queue(HWND wnd);

/* resize a component to the given size */
#define win32_resize(parent, componentid, w, h) \
	SetWindowPos(GetDlgItem(parent, componentid), HWND_TOP, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE)

/* move a component to the given coordinates */
#define win32_move(parent, componentid, x, y) \
	SetWindowPos(GetDlgItem(parent, componentid), HWND_TOP, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE)

/* returns the components xy position relative to the parent's client rect */
void win32_get_client_xy(HWND parent, int componentid, POINT *dst);

/* expands the given component to the right to match its parent's width with a margin */
void win32_expand_horizontal(HWND parent, int componentid, int margin, int componentheight);

/* expands the given component to the right and bottom to match its parent's width with a margin */
void win32_expand(HWND parent, int componentid, int marginright, int marginbottom);

/* makes the component stick to the right edge of the parent window with a margin */
void win32_dock_right(HWND parent, int componentid, int componentwidth, int marginright);

/* makes the component stick to the bottom edge of the parent window with a margin */
void win32_dock_bottom(HWND parent, int componentid, int componentheight, int marginbottom);

/*
 * creates a win32 control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
HWND win32_make_control(HWND parent, int controlid, const char *class,
	const char *name, const char *title, int x, int y, int w, int h,
	DWORD style, DWORD exstyle);

/*
 * creates a static control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_bitmap(parent, controlid, name, x, y, w, h) \
	win32_make_control(parent, controlid, WC_STATICA, name, NULL, x, y, w, h, \
		WS_VISIBLE | WS_CHILD | SS_BITMAP, 0)

/*
 * creates a static bitmap control.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_static(parent, controlid, name, text, x, y, w, h) \
	win32_make_control(parent, controlid, WC_STATICA, name, text, x, y, w, h, \
	WS_CHILD | WS_VISIBLE | SS_LEFT, 0)

/* default edit control style */
#define WS_EDIT_DEFAULT WS_TABSTOP | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL

/*
 * creates a single-line edit control with the default style and gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_edit(parent, controlid, name, initialtext, x, y, w, h) \
	win32_make_control(parent, controlid, WC_EDITA, name, initialtext, x, y, w, h, \
		WS_EDIT_DEFAULT, WS_EX_CLIENTEDGE)

/*
 * creates a single-line edit control with the default gui font and the given styles.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_edit_s(parent, controlid, name, initialtext, x, y, w, h, style) \
	win32_make_control(parent, controlid, WC_EDITA, name, initialtext, x, y, w, h, \
	style, WS_EX_CLIENTEDGE)

/*
 * creates a groupbox control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_groupbox(parent, controlid, name, text, x, y, w, h) \
	win32_make_control(parent, controlid, WC_BUTTONA, name, text, x, y, w, h, \
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 0)

/*
 * creates a pushbutton control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_button(parent, controlid, name, text, x, y, w, h) \
	win32_make_control(parent, controlid, WC_BUTTONA, name, text, x, y, w, h, \
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 0)

/*
 * creates a checkbox control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_checkbox(parent, controlid, name, text, x, y, w, h) \
	win32_make_control(parent, controlid, WC_BUTTONA, name, text, x, y, w, h, \
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0)

/*
 * creates a combobox control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_combobox(parent, controlid, name, x, y, w, h) \
	win32_make_control(parent, controlid, WC_COMBOBOXA, name, "", x, y, w, h, \
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 0)

/*
 * creates a listview control with the default gui font.
 * returns NULL and outputs an error to stdout with the given control name if unsucessful.
 */
#define win32_make_listview(parent, controlid, name, x, y, w, h) \
	win32_make_control(parent, controlid, WC_LISTVIEW, name, "", x, y, w, h, \
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_ALIGNLEFT | \
		WS_BORDER | WS_TABSTOP, 0)

/* adds a column to a listview control */
int win32_listview_add_column(HWND list, int index, const char *title, int width);

/*
 * sets the width of the given listview's column.
 * if colindex is -1, the last column will be automatically obtained.
 * returns the index of the affected column or -1 if unsuccessful.
 */
int win32_listview_resize_column(HWND list, int colindex, int width);

#endif
