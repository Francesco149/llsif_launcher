/*
	Copyright 2014 Franc[e]sco (lolisamurai@tfwno.gf)
	This file is part of llsif_utils.
	llsif_utils is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	llsif_utils is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with llsif_utils. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ADB_UTILS_H
#define ADB_UTILS_H

#include <stddef.h>

/*
 * sends a tap event to the android device at the given pixel coordinates. 
 * the screen size is hardcoded to 1600x900 for now.
 * calling this will block the current thread for 100ms to let the tap register.
 * NOTE: this is only tested on a specific version of BlueStacks at the moment
 * TODO: make the event interface customizable
 */
void adb_tap(int x, int y);

/* kills a process by package name */
void adb_killbypackage(const char *package);

/*
 * stores the directory listing of the given path in result as a newline-separated string.
 * if successful, it will return non-zero.
 */
int adb_ls(const char *path, char *result, size_t result_maxlen);

/* returns non-zero if the file/dir exists. */
int adb_exists(const char *path);

/* returns non-zero if the file/dir is a symlink */
int adb_is_symlink(const char *path);

#endif
