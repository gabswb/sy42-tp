/**
 ******************************************************************************
 * @file      syscalls.c
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32CubeIDE Minimal System calls file
 *
 *            For more information about which c-functions
 *            need which of these lowlevel functions
 *            please consult the Newlib libc-manual
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>


/* Variables */
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}		/* Make sure we hang here */
}

/* ------------------IO buffuring for stdlib -----------------------------*/
	/* stdin and stdout IO are buffed by default, you can change the
	 * default behaviour with setvbuff.
	 * int setvbuf ( FILE * stream, char * buffer, int mode, size_t size );
	 * set buffering on stdio if needed, if buffer == NULL, malloc is called
	 *_IOFBF full buffering, _IOLBF line buffering, _IONBF 	no buffering
	 * example : using a static buffer, no buffering
	 * setvbuf( stdout, stdin_buffer, _IONBF, (size_t) 0);
	 * setvbuf( stdin, stdout_buffer, _IONBF, (size_t) 0);
	 */

__attribute__((weak)) int _read(int file, char *ptr, int len)
{

	/* here, just read one datum, otherwise it will wait for lot's of data,
	 * depending of the buffer size requested (1024 by default on newlib)
	 * that can also be changed with setvbuf()
	 *
	 * You can also wait for the \n char to return (for fgets).
	 */
	int ch;

	/* get the char */
	ch = __io_getchar();

	/* change '\r' in '\n' (unix type) or remove it (if \r\n received).
	 * it depends on the terminal parameters */
	if (ch == '\r')
		ch = '\n';

	*ptr = ch;

return 1;
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}

int _close(int file)
{
	return -1;
}


int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}
