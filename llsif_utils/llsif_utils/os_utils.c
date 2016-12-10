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

#include "os_utils.h"

#include <stdio.h>

#if _WIN32
#include <Windows.h>
#define internal_os_sleep Sleep
#define popen _popen
#define pclose _pclose
#else
#include <signal.h>
#include <unistd.h>
#define internal_os_sleep(x) usleep(x * 1000)
#endif

volatile int keeprunning = 1;

#if _WIN32
BOOL WINAPI console_handler(DWORD dwType)
{
	switch (dwType) {
	case CTRL_C_EVENT:
		printf("Caught ctrl-c\n");
		keeprunning = 0;
		break;
	case CTRL_BREAK_EVENT:
		printf("Caught break\n");
		keeprunning = 0;
		break;
	}
	return TRUE;
}
#else
void sigint_handler(int crap) {
	printf("Caught SIGINT\n");
	keeprunning = 0;
}
#endif

/* installs the library's internal sigint handler which will set the keeprunning flag */
void os_install_sigint_handler() {
#if _WIN32
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)console_handler, TRUE)) {
		fprintf(stderr, "Unable to install console ctrl handler.\n");
		exit(1);
	}
#else
	signal(SIGINT, sigint_handler);
#endif
}

/* blocks the current thread for the given amount of milliseconds */
void os_sleep(uint32_t millisec) {
	internal_os_sleep(millisec);
}

/*
 * executes cmd and copies the contents of stdout to result.
 * result_maxlen is the size of result. if stdout is larger than result_maxlen,
 * it will be truncated to fit result.
 * any carriage return character (\r) will be automatically stripped from the result.
 * returns non-zero if unsucessful.
 */
int os_shellexec(const char *cmd, char *result, size_t result_maxlen) {
#if _WIN32
	/* horrible super-verbose win32 api code to avoid console windows popping up */
	HANDLE stdout_rd = NULL, stdout_wr = NULL; /* child stdout handles */
	SECURITY_ATTRIBUTES sa = { 0 }; /* stdout pipe security attributes */
	PROCESS_INFORMATION pi = { 0 }; /* will contain the process info */
	STARTUPINFOA si = { 0 }; /* various start-up params for the process */
	BOOL succ = FALSE;
	DWORD cbread; /* number of bytes read from stdout */
	int offset = 0; /* offset into result when reading stdout */
	int i = 0;
	char *cleanresult = NULL; /* will be used to strip carriage returns */
	char *result_iterator = NULL;

	/* apparently createprocess wants a non-const cmdline buffer */
	CHAR cmdcopy[MAX_PATH * 10] = { 0 };
	strcpy_s(cmdcopy, MAX_PATH * 10, cmd);

	/* create stdout pipe */
	sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&stdout_rd, &stdout_wr, &sa, 0)) {
		return 1;
	}

	if (!SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0)) {
		return 1;
	}

	/* start process */
	si.cb = sizeof(STARTUPINFO); 
	si.hStdError = stdout_wr;
	si.hStdOutput = stdout_wr;
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; /* hide console window before it's even created */

	succ = CreateProcessA(NULL, cmdcopy, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!succ) {
		return -1;
	}

	/* close all open handles to the process*/
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(stdout_wr);

	/* copy stdout to result*/
	while (1) {
		if (!ReadFile(stdout_rd, result + offset, result_maxlen - offset, &cbread, NULL)) {
			break;
		}
		if (!cbread) {
			break;
		}
		offset += cbread;
	}
	CloseHandle(stdout_rd);

	/* erase windows carriage returns */
	cleanresult = (char *)malloc(sizeof(char) * result_maxlen);
	i = 0;
	for (result_iterator = result; *result_iterator != '\0'; result_iterator++) {
		if (*result_iterator != '\r') { 
			cleanresult[i] = *result_iterator;
			i++;
		}
	}
	cleanresult[i] = '\0';
	memcpy_s(result, result_maxlen, cleanresult, i + 1);
	free(cleanresult);

#else
	FILE *fp;
	size_t i;

	/* start process */
	fp = popen(cmd, "r");
	if (fp == NULL) {
		return 1;
	}

	/* copy stdout to result */
	for (i = 0; i < result_maxlen; i++) {
		char c;
		c = getc(fp);
		if (c == EOF) {
			break;
		}
		result[i] = c;
	}

	pclose(fp);
#endif

	return 0;
}

/* executes the given shell command and prints the command and output to stdout */
void os_printoutput(const char *cmd) {
	char buf[MAX_OUTPUT] = { 0 };
	os_shellexec(cmd, buf, MAX_OUTPUT);
	printf("$ %s\n%s\n", cmd, buf);
}
