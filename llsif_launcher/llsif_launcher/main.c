/*
	Copyright 2014 Franc[e]sco (lolisamurai@tfwno.gf)
	This file is part of llsif_launcher.
	llsif_launcher is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	llsif_launcher is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with llsif_launcher. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <time.h>
#include <windows.h>

#include <commctrl.h>
#pragma comment(lib, "Comctl32")

#include "resource.h"
#include "llsif_utils.h"
#include "adb_utils.h"
#include "os_utils.h"
#include "win32_console.h"
#include "win32_gui.h"

#if _DEBUG
#pragma comment(lib, "llsif_utils_d.lib")
#pragma comment(lib, "win32_utils_d.lib")
#else
#pragma comment(lib, "llsif_utils.lib")
#pragma comment(lib, "win32_utils.lib")
#endif

/* marks unused params to avoid warnings */
#define UNUSED(...) (void)(__VA_ARGS__)

/* app info */
#define LAUNCHER_CLASS "llsif_launcher"
#define LAUNCHER_NAME "LLSIF Launcher"
#define LAUNCHER_VERSION "1.0"
#define LAUNCHER_CONFIG ".\\settings.ini"
#define LAUNCHER_DEFAULT_PATH "/data/data/klb.android.lovelive_en/shared_prefs"

/* menus */
#define IDM_FILE_EXIT 101
#define IDM_SETTINGS_REMEMBER 111
#define IDM_HELP_ABOUT 121

/* controls */
#define IDC_GROUP 130
#define IDC_EDIT_XMLPATH 131
#define IDC_BUTTON_LOAD 132
#define IDC_LIST_XMLS 133
#define IDC_BUTTON_START 134
#define IDC_BUTTON_KILL 135

/* 
 * control sizes or margins 
 * NOTE: buttons will be 2px smaller in width and height than 
 *       specified because win32 is stupid. the coordinates will 
 *       also be shifted by 1px to the right and bottom.
 */
#define LAUNCHER_W 420 /* blaze it */
#define LAUNCHER_H 363
#define GROUP_WMARGIN 10
#define GROUP_HMARGIN 10
#define EDIT_XMLPATH_WMARGIN (50 + 14 + 10)
#define EDIT_XMLPATH_H 20
#define BUTTON_LOAD_W 42
#define BUTTON_LOAD_H 22
#define BUTTON_LOAD_DOCK_MARGIN 23
#define LIST_XMLS_WMARGIN 24
#define	LIST_XMLS_HMARGIN 54
#define BUTTON_START_W 50
#define BUTTON_START_H 22
#define BUTTON_START_DOCK_MARGIN 24
#define BUTTON_KILL_W 50
#define BUTTON_KILL_H 22
#define BUTTON_KILL_DOCK_MARGIN 24

/* customizable adb connect command*/
static volatile char launcher_connect_command[MAX_PATH] = { 0 };

/* 
 * checks that adb is available and displays an error if it isn't. 
 * returns nonzero if adb is available and working. 
 */
int launcher_check_adb(HWND wnd) {
	char adbver[MAX_OUTPUT] = { 0 };

	os_shellexec("adb version", adbver, MAX_OUTPUT);
	printf("$ adb version\n%s\n", adbver);

	if (!strstr(adbver, "Android Debug Bridge")) {
		MessageBoxA(wnd, "Oops! I couldn't detect adb. Make sure that you either have adb in the "
			"same folder as the launcher or adb's folder included in your PATH variable.",
			LAUNCHER_NAME, MB_OK | MB_ICONERROR);
		return 0;
	}

	return 1;
}

/* loads the current settings from settings.ini */
void launcher_load_settings(HWND wnd) {
	int remember = 0;
	char xmlpath[MAX_PATH] = { 0 };
	HMENU menu;

	GetPrivateProfileStringA(
		"Settings", "ConnectCommand",
		"adb connect 127.0.0.1:5555",
		(char *)launcher_connect_command, 
		MAX_PATH, LAUNCHER_CONFIG);

	remember = GetPrivateProfileIntA("Settings", "RememberPath", 1, LAUNCHER_CONFIG);
	if (remember) {
		remember = MF_CHECKED;
		GetPrivateProfileStringA("Settings", "Path",
			LAUNCHER_DEFAULT_PATH, xmlpath, MAX_PATH, LAUNCHER_CONFIG);
		SetDlgItemTextA(wnd, IDC_EDIT_XMLPATH, xmlpath);
	}
	else {
		remember = MF_UNCHECKED;
	}

	menu = GetMenu(wnd);
	CheckMenuItem(menu, IDM_SETTINGS_REMEMBER, remember);
}

/* saves the current settings from settings.ini */
void launcher_save_settings(HWND wnd) {
	const char *remember = "0";
	char xmlpath[MAX_PATH] = { 0 };
	HMENU menu;

	menu = GetMenu(wnd);
	if (GetMenuState(menu, IDM_SETTINGS_REMEMBER, MF_BYCOMMAND) == MF_CHECKED) {
		remember = "1";
	}

	GetDlgItemTextA(wnd, IDC_EDIT_XMLPATH, xmlpath, MAX_PATH);

	WritePrivateProfileStringA("Settings", "ConnectCommand", (char *)launcher_connect_command, LAUNCHER_CONFIG);
	WritePrivateProfileStringA("Settings", "RememberPath", remember, LAUNCHER_CONFIG);
	WritePrivateProfileStringA("Settings", "Path", xmlpath, LAUNCHER_CONFIG);
}

/* initializes the main window's controls and performs other initialization-time operations */
LRESULT launcher_initwnd(HWND wnd) {
	HMENU menu, submenu;
	HWND group, edit_xmlpath, button_load, list_xmls, button_start, button_kill;
	RECT wndrect = { 0 };
	int colindex;

	GetClientRect(wnd, &wndrect);

	/* menu */
	menu = CreateMenu();
	submenu = CreatePopupMenu();
	AppendMenuA(menu, MF_STRING | MF_POPUP, (UINT_PTR)submenu, "&File");
	AppendMenuA(submenu, MF_STRING, IDM_FILE_EXIT, "&Exit");
	submenu = CreatePopupMenu();
	AppendMenuA(menu, MF_STRING | MF_POPUP, (UINT_PTR)submenu, "&Settings");
	AppendMenuA(submenu, MF_STRING | MF_CHECKED, IDM_SETTINGS_REMEMBER, "&Remember path");
	submenu = CreatePopupMenu();
	AppendMenuA(menu, MF_STRING | MF_POPUP, (UINT_PTR)submenu, "&Help");
	AppendMenuA(submenu, MF_STRING, IDM_HELP_ABOUT, "&About");
	SetMenu(wnd, menu);

	/* main groupbox */
	group = win32_make_groupbox(
		wnd, IDC_GROUP, "group", "Accounts", 10, 5,
		wndrect.right - GROUP_WMARGIN - 10,
		wndrect.bottom - GROUP_HMARGIN - 25);
	if (!group) {
		return -1;
	}

	/* xml path textbox */
	edit_xmlpath = win32_make_edit(
		wnd, IDC_EDIT_XMLPATH, "edit_xmlpath", LAUNCHER_DEFAULT_PATH, 
		24, 29, 
		wndrect.right - EDIT_XMLPATH_WMARGIN - 24, 
		EDIT_XMLPATH_H);
	if (!edit_xmlpath) {
		return -1;
	}

	/* load button */
	button_load = win32_make_button(
		wnd, IDC_BUTTON_LOAD, "button_load", "Load",
		wndrect.right - BUTTON_LOAD_W - BUTTON_LOAD_DOCK_MARGIN, 28,
		BUTTON_LOAD_W, BUTTON_LOAD_H);
	if (!button_load) {
		return -1;
	}

	/* file listview TODO: make this a virtual listview */
	list_xmls = win32_make_listview(
		wnd, IDC_LIST_XMLS, "list_xmls",
		24, 59,
		wndrect.right - LIST_XMLS_WMARGIN - 10,
		wndrect.bottom - LIST_XMLS_HMARGIN - 55);
	if (!list_xmls) {
		return -1;
	}

	colindex = win32_listview_add_column(list_xmls, 0, "File", 
		wndrect.right - LIST_XMLS_WMARGIN - 10 - 10);
	if (colindex == -1) {
		return -1;
	}

	ListView_SetExtendedListViewStyle(list_xmls, LVS_EX_FULLROWSELECT);

	/* start button */
	button_start = win32_make_button(
		wnd, IDC_BUTTON_START, "button_start", "Start",
		23, wndrect.bottom - BUTTON_START_H - BUTTON_START_DOCK_MARGIN,
		BUTTON_START_W, BUTTON_START_H);
	if (!button_start) {
		return -1;
	}

	/* kill button */
	button_kill = win32_make_button(
		wnd, IDC_BUTTON_KILL, "button_kill", "Kill",
		23 + BUTTON_START_W + 10 - 2, 
		wndrect.bottom - BUTTON_KILL_H - BUTTON_KILL_DOCK_MARGIN,
		BUTTON_KILL_W, BUTTON_KILL_H);
	if (!button_kill) {
		return -1;
	}

	launcher_load_settings(wnd);
	launcher_save_settings(wnd);
	launcher_check_adb(wnd);

	return 0;
}

/* triggered by Settings -> Remember path */
void launcher_remember_clicked(HWND wnd) {
	HMENU menu;
	UINT check;

	menu = GetMenu(wnd);
	switch (GetMenuState(menu, IDM_SETTINGS_REMEMBER, MF_BYCOMMAND)) {
	case MF_CHECKED:
		check = MF_UNCHECKED;
		break;

	case MF_UNCHECKED:
		check = MF_CHECKED;
		break;
	}

	CheckMenuItem(menu, IDM_SETTINGS_REMEMBER, check);
	launcher_save_settings(wnd);
}

/* triggered by Help -> About */
void launcher_about_clicked(HWND wnd) {
	char info[MAX_OUTPUT] = { 0 };
	sprintf_s(info, MAX_OUTPUT, "%s v%s\r\n\r\n"
		"by Franc[e]sco (lolisamurai@tfwno.gf)\r\n" 
		"Licensed under the GPLv3 license.\r\n"  
		"Source code at: https://github.com/Francesco149/llsif_launcher", 
		LAUNCHER_NAME, LAUNCHER_VERSION);
	MessageBoxA(wnd, info, "About", MB_OK | MB_ICONINFORMATION);
}

/* triggered by the "Load" button */
void launcher_load_clicked(HWND wnd) {
	char buf[200000] = { 0 }; /* our directory listing could be pretty huge */
	int n; /* number of characters copied by GetWindowTextA */
	char *line, *next_token = NULL; /* next_token is some crap used by strtok_s */
	size_t i = 0; /* listview item index */
	LVITEMA item = { 0 }; /* listview item struct for item insertion */
	HWND lv = NULL; /* listview wnd */
	char connectres[MAX_OUTPUT] = { 0 };

	if (!launcher_check_adb(wnd)) {
		return;
	}
	
	/* save the connect debug output for later to make the UI appear faster and snappier */
	os_shellexec((const char *)launcher_connect_command, connectres, MAX_OUTPUT);

	/* get text from the path textbox */
	n = GetWindowTextA(GetDlgItem(wnd, IDC_EDIT_XMLPATH), buf, 200000);
	if (!strlen(buf)) {
		MessageBoxA(wnd, "You must provide a path to your xml files.",
			LAUNCHER_NAME, MB_OK | MB_ICONERROR);
		return;
	}
	if (!n) {
		win32_show_error("GetWindowTextA failed");
		return;
	}

	/* try listing the directory */
	if (!adb_ls(buf, buf, 200000)) {
		/* to improve performance, only print ls output if something goes wrong */
		printf("ls result:\n%s\n", buf); 
		MessageBoxA(wnd, "Failed to obtain file list. Please check your xml folder path.",
			LAUNCHER_NAME, MB_OK | MB_ICONERROR);
		return;
	}

	/* iterate all lines in the output and insert each as an item */
	lv = GetDlgItem(wnd, IDC_LIST_XMLS);
	ListView_DeleteAllItems(lv);
	i = 0;
	line = strtok_s(buf, "\n", &next_token);
	while (line) { /* I could make it ignore AdXPrefrences.xml but whatever */
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.cchTextMax = MAX_PATH;
		item.pszText = line;

#if _DEBUG
		for (i = 0; i < strlen(line); i++) {
			if (line[i] == '\r') {
				MessageBoxA(NULL, "Filename contains an unexpected carriage return! "
					"Please report this to the developer.", LAUNCHER_NAME, MB_OK | MB_ICONERROR);
			}
		}
#endif

		if (ListView_InsertItem(lv, &item) == -1) {
			printf("ls result:\n%s\n", buf);
			win32_show_error("ListView_InsertItem failed");
			return;
		}

		line = strtok_s(NULL, "\n", &next_token);
		i++;
	}

	puts(connectres);
	launcher_save_settings(wnd);
}

/* triggered by the "Start" button */
void launcher_start_clicked(HWND wnd) {
	HWND lv = NULL; /* listview wnd */
	int selected; /* selected item index */
	char file[MAX_PATH] = { 0 }; /* filename */
	char path[MAX_PATH] = { 0 }; /* xml path */
	char fullpath[MAX_PATH] = { 0 }; /* used to format various paths */
	char msg[MAX_PATH] = { 0 }; /* used to format infor / error messages */
	int n; /* number of characters copied by GetWindowTextA */

	if (!launcher_check_adb(wnd)) {
		return;
	}

	lv = GetDlgItem(wnd, IDC_LIST_XMLS);

	/* grab first selected item */
	selected = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
	if (selected < 0) {
		MessageBoxA(wnd, "No file selected.", LAUNCHER_NAME, MB_OK | MB_ICONERROR);
		return;
	}

	ListView_GetItemText(lv, selected, 0, file, MAX_PATH);

	/* get xml path and append filename to path */
	n = GetWindowTextA(GetDlgItem(wnd, IDC_EDIT_XMLPATH), path, MAX_PATH);
	if (!strlen(path)) {
		MessageBoxA(wnd, "You must provide a path to your xml files.",
			LAUNCHER_NAME, MB_OK | MB_ICONERROR);
		return;
	}
	if (!n) {
		win32_show_error("GetWindowTextA failed");
		return;
	}

	/* check for existing xmls and delete if necessary */
	sprintf_s(fullpath, MAX_PATH, "%s/GameEngineActivity.xml", path);
	if (adb_exists(fullpath) && !adb_is_symlink(fullpath)) {
		n = MessageBoxA(wnd, "It looks like you already have a GameEngineActivity.xml "
			"in your shared_prefs directory! Do you want to make a backup? If you "
			"select \"No\", the file will be lost forever!",
			LAUNCHER_NAME, MB_YESNO | MB_ICONWARNING);
		if (n == IDYES) {
			/* generate an unique filename by appending the current unix timestamp */
			do {
				time_t unix;
				time(&unix);
				sprintf_s(fullpath, MAX_PATH, 
					"GameEngineActivity_backup_%lu.xml", 
					(uint32_t)unix);
			} while (llsif_xml_exists(fullpath));

			/* rename xml */
			llsif_rename_xml_full(fullpath);
			launcher_load_clicked(wnd); /* refresh filelist */
			sprintf_s(msg, MAX_PATH, "Your GameEngineActivity.xml was moved to %s", fullpath);
			MessageBoxA(wnd, msg, LAUNCHER_NAME, MB_OK | MB_ICONINFORMATION);
		}
	}

	/* remove old symlink and symlink the desired file */
	sprintf_s(fullpath, MAX_PATH, "%s/%s", path, file);
	llsif_remove_xml();
	llsif_symlink_xml(fullpath);
	llsif_start();
}

/* triggered by the "kill" button */
void launcher_kill_clicked(HWND wnd) {
	if (!launcher_check_adb(wnd)) {
		return;
	}
	llsif_kill();
}

/* handles resizing of the main window */
void launcher_onresize(HWND wnd) {
	win32_expand(wnd, IDC_GROUP, GROUP_WMARGIN, GROUP_HMARGIN);
	win32_expand_horizontal(wnd, IDC_EDIT_XMLPATH, EDIT_XMLPATH_WMARGIN, EDIT_XMLPATH_H);
	win32_dock_right(wnd, IDC_BUTTON_LOAD, BUTTON_LOAD_W, BUTTON_LOAD_DOCK_MARGIN);
	win32_expand(wnd, IDC_LIST_XMLS, LIST_XMLS_WMARGIN, LIST_XMLS_HMARGIN);
	win32_dock_bottom(wnd, IDC_BUTTON_START, BUTTON_START_H, BUTTON_START_DOCK_MARGIN);
	win32_dock_bottom(wnd, IDC_BUTTON_KILL, BUTTON_KILL_H, BUTTON_KILL_DOCK_MARGIN);
}

/* processes notify messages and handles the listview's doubleclick event */
void launcher_onnotify(HWND wnd, LPNMHDR msg) {
	int selected;
	HWND lv;

	if (msg->idFrom != IDC_LIST_XMLS) {
		return;
	}
	if (msg->code != NM_DBLCLK) {
		return;
	}

	lv = GetDlgItem(wnd, IDC_LIST_XMLS);
	selected = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
	if (selected < 0) {
		return;
	}

	launcher_start_clicked(wnd);
}

/* processes the main window messages */
LRESULT CALLBACK launcher_wndproc(HWND wnd, UINT msgid, WPARAM wparam, LPARAM lparam) {
	switch (msgid) {
	case WM_CREATE:
		return launcher_initwnd(wnd);

	case WM_SIZE:
		launcher_onresize(wnd);
		break;

	case WM_CLOSE:
		launcher_save_settings(wnd);
		DestroyWindow(wnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDM_FILE_EXIT:
			PostQuitMessage(0);
			return TRUE;

		case IDM_SETTINGS_REMEMBER:
			launcher_remember_clicked(wnd);
			return TRUE;

		case IDM_HELP_ABOUT:
			launcher_about_clicked(wnd);
			return TRUE;

		case IDC_BUTTON_LOAD:
			launcher_load_clicked(wnd);
			return TRUE;

		case IDC_BUTTON_START:
			launcher_start_clicked(wnd);
			return TRUE;

		case IDC_BUTTON_KILL:
			launcher_kill_clicked(wnd);
			return TRUE;
		}
		break;

	case WM_NOTIFY:
		launcher_onnotify(wnd, (LPNMHDR)lparam);
		break;

	default:
		return DefWindowProc(wnd, msgid, wparam, lparam);
	}

	return 0;
}

/* registers and creates the main window */
int launcher_create(HINSTANCE inst, int cmdshow) {
	HWND wnd;
#if _DEBUG
	char err[512] = { 0 };
	DWORD gle;

	if (!win32_console_create()) {
		gle = GetLastError();
		sprintf_s(err, 512, "Failed to allocate console, you won't "
			"be able to see debug logs. GLE=%.08X (%lu)", gle, gle);
		MessageBoxA(NULL, err, LAUNCHER_NAME, MB_OK | MB_ICONWARNING);
	}
#endif

	InitCommonControls();
	wnd = win32_make_window(
		LAUNCHER_CLASS, LAUNCHER_NAME,
		inst, launcher_wndproc,
		LoadIcon(inst, MAKEINTRESOURCE(IDI_ICON1)), 
		LAUNCHER_W, LAUNCHER_H, cmdshow);
	if (!wnd) {
		return EXIT_FAILURE;
	}

	return win32_message_loop(NULL);
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE previnst, LPSTR cmdline, int cmdshow) {
	UNUSED(previnst, cmdline);
	int res;
	HWND console;

	res = launcher_create(inst, cmdshow);

	if (res != EXIT_SUCCESS) {
		win32_clear_message_queue(NULL); /* clear message queue, otherwise the messagebox won't work */
		MessageBoxA(NULL, "Oops! Looks like something broke. "
			"Please report this to the developer or check debug logs.",
			LAUNCHER_NAME, MB_OK | MB_ICONERROR);

		/* pause on errors to see the log */
		console = GetConsoleWindow();
		if (console) {
			system("pause");
			FreeConsole();
		}
	}

	return res;
}