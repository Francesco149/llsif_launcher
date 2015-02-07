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

#ifndef LLSIF_UTILS_H
#define LLSIF_UTILS_H

/* starts the game */
void llsif_start();

/* force close the game */
void llsif_kill();

/* renames the game's GameEngineActivity.xml to GameEngineActivity<accnum>.xml */
void llsif_rename_xml(int accnum);

/* removes the game's GameEngineActivity.xml */
void llsif_remove_xml();

/* symlinks the given xml as GameEngineActivity.xml */
void llsif_symlink_xml(const char *xmlpath);

/* renames the game's GameEngineActivity.xml to the given filename */
void llsif_rename_xml_full(const char *newname);

/* returns non-zero if the given xml file exists in the shared_prefs folder */
int llsif_xml_exists(const char *file);

#endif
