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

#include "llsif_utils.h"
#include "os_utils.h"
#include "adb_utils.h"

#include <string.h>
#include <stdio.h>

/*
* app package, activity and shared prefs folder.
* these should never change unless you switch to JP version
* TODO: make these customizable
*/
#define LLSIF_PACKAGE "klb.android.lovelive_en"
#define LLSIF_ACTIVITY "klb.android.GameEngine.GameEngineActivity"
#define LLSIF_SHARED_PREFS "/data/data/klb.android.lovelive_en/shared_prefs"

/* starts the game */
void llsif_start() {
	static char cmd[MAX_OUTPUT] = { 0 };
	if (strlen(cmd) == 0) {
		sprintf(cmd, "adb shell am start -n %s/%s", LLSIF_PACKAGE, LLSIF_ACTIVITY);
	}
	os_printoutput(cmd);
}

/* force close the game */
void llsif_kill() {
	adb_killbypackage(LLSIF_PACKAGE);
}

/* renames the game's GameEngineActivity.xml to GameEngineActivity<accnum>.xml */
void llsif_rename_xml(int accnum) {
	char newname[260] = { 0 };
	sprintf(newname, "GameEngineActivity%d.xml", accnum);
	llsif_rename_xml_full(newname);
}

/* removes the game's GameEngineActivity.xml */
void llsif_remove_xml() {
	char cmd[MAX_OUTPUT] = { 0 };
	sprintf(cmd, "adb shell \"su -c 'rm %s/GameEngineActivity.xml'\"", 
		LLSIF_SHARED_PREFS);
	os_printoutput(cmd);
}

/* symlinks the given xml as GameEngineActivity.xml */
void llsif_symlink_xml(const char *xmlpath) {
	char cmd[MAX_OUTPUT] = { 0 };
	sprintf(cmd, "adb shell \"su -c 'ln -s %s %s/GameEngineActivity.xml'\"", 
		xmlpath, LLSIF_SHARED_PREFS);
	os_printoutput(cmd);
}

/* renames the game's GameEngineActivity.xml to the given filename */
void llsif_rename_xml_full(const char *newname) {
	char cmd[MAX_OUTPUT] = { 0 };
	sprintf(cmd,
		"adb shell \"su -c 'mv "
		"%s/GameEngineActivity.xml "
		"%s/%s"
		"'\"", LLSIF_SHARED_PREFS, LLSIF_SHARED_PREFS, newname);
	os_printoutput(cmd);
}

/* returns non-zero if the given xml file exists in the shared_prefs folder */
int llsif_xml_exists(const char *file) {
	char path[MAX_OUTPUT] = { 0 };
	sprintf(path, "%s/%s", LLSIF_SHARED_PREFS, file);
	return adb_exists(path);
}
