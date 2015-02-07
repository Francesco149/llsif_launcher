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

#include "adb_utils.h"
#include "os_utils.h"

#include <stdio.h>
#include <string.h>

/* device screen size TODO: make this customizable */
#define DEVICE_W 1600
#define DEVICE_H 900

/*
 * sends a tap event to the android device at the given pixel coordinates.
 * calling this will block the current thread for 100ms to let the tap register.
 * the screen size is hardcoded to 1600x900 for now.
 * NOTE: this is only tested on a specific version of BlueStacks at the moment
 * TODO: make the event interface customizable
 */
void adb_tap(int x, int y) {
	char tapcommand[MAX_OUTPUT] = { 0 };

	// coordinates (the screen area is a [0, 0, 0x8000, 0x8000] rect)
	sprintf(tapcommand, "adb shell sendevent /dev/input/event1 3 0 %d", x * (0x8000 / DEVICE_W));
	os_printoutput(tapcommand);
	os_printoutput("adb shell sendevent /dev/input/event1 0 0 0");
	sprintf(tapcommand, "adb shell sendevent /dev/input/event1 3 1 %d", y * (0x8000 / DEVICE_H));
	os_printoutput(tapcommand);
	os_printoutput("adb shell sendevent /dev/input/event1 0 0 0");

	// press
	os_printoutput("adb shell sendevent /dev/input/event1 1 272 1");
	os_printoutput("adb shell sendevent /dev/input/event1 0 0 0");

	os_sleep(100); // let the tap register

	// release
	os_printoutput("adb shell sendevent /dev/input/event1 1 272 0");
	os_printoutput("adb shell sendevent /dev/input/event1 0 0 0");
}

/* kills a process by package name in the android device */
void adb_killbypackage(const char *package) {
	char buf[MAX_OUTPUT] = { 0 };
	char pidresult[MAX_OUTPUT] = { 0 };
	int pidslen = 0;
	int i = 0;

	sprintf(buf, "adb shell \"ps | grep %s | awk '{ print $2 }'\"", package);
	os_shellexec(buf, pidresult, MAX_OUTPUT);

	pidslen = strlen(pidresult);
	for (i = 0; i < pidslen; i++) {
		if (pidresult[i] == '\n') {
			pidresult[i] = '\0';
		}
	}

	sprintf(buf, "adb shell \"su -c 'kill %s'\"", pidresult);
	os_printoutput(buf);
}

/*
 * stores the directory listing of the given path in result as a newline-separated string.
 * if successful, it will return non-zero.
 */
int adb_ls(const char *path, char *result, size_t result_maxlen) {
	char buf[MAX_OUTPUT] = { 0 };
	int files = 0;

	sprintf(buf, "adb shell \"su -c 'ls %s'\"", path);
	os_shellexec(buf, result, result_maxlen);

	if (strstr(result, ":") != NULL) {
		return 0;
	}

	return 1;
}

/* evaluates contents as [ contents ] and returns non-zero if the condition is true */
int adb_condition(const char *contents) {
	char buf[MAX_OUTPUT] = { 0 };
	char result[8] = { 0 };
	sprintf(buf, "adb shell \"[ %s ] && echo '1' || echo '0'\"", contents);
	os_shellexec(buf, result, 8);
	printf("$ %s\n%s\n", buf, result);
	if (result[0] == '1') {
		return 1;
	}
	return 0;
}

/* returns non-zero if the file/dir exists. */
int adb_exists(const char *path) {
	char buf[MAX_OUTPUT] = { 0 };
	sprintf(buf, "-e %s", path);
	return adb_condition(buf);
}

/* returns non-zero if the file/dir is a symlink */
int adb_is_symlink(const char *path) {
	char buf[MAX_OUTPUT] = { 0 };
	sprintf(buf, "-L %s", path);
	return adb_condition(buf);
}