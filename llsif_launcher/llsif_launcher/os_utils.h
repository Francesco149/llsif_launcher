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

#ifndef OS_UTILS_H
#define OS_UTILS_H

#include <stdlib.h>
#include <stdint.h>

/* maximum expected size of stdout for the commands we're using */
#define MAX_OUTPUT 2048

/* 
 * this flag is used for console applications that need 
 * to stop gracefully on SIGINT or other signals 
 */
extern volatile int keeprunning;

/* 
 * this macro checks keeprunning and breaks out of the loop if it's false. 
 * this is meant to be used inside a loop. 
 */
#define check_sigint() if (!keeprunning) break;

/*
 * this macro checks keeprunning, breaks out of the loop if it's false and 
 * blocks the thread for the given amount of milliseconds.
 * this is meant to be used inside a loop.
 */
#define csleep(x) if(1) {	\
	check_sigint()			\
	os_sleep(x);			\
	check_sigint()			\
}

/* installs the library's internal sigint handler which will set the keeprunning flag */
void os_install_sigint_handler();

/* blocks the current thread for the given amount of milliseconds */
void os_sleep(uint32_t millisec);

/*
 * executes cmd and copies the contents of stdout to result.
 * result_maxlen is the size of result. if stdout is larger than result_maxlen,
 * it will be truncated to fit result.
 * returns non-zero if unsucessful.
 */
int os_shellexec(const char *cmd, char *result, size_t result_maxlen);

/* executes the given shell command and prints the command and output to stdout */
void os_printoutput(const char *cmd);

#endif
