/*
 * This file is part of Opentube - Open video hosting engine
 *
 * Copyright (C) 2011 - Xpast; http://xpast.me/; <vvladxx@gmail.com>
 *
 * Opentube is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Opentube is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Opentube.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "process.hpp"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#ifdef HAVE_PRCTL_H
#include <sys/prctl.h>
#endif
#else
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>
#endif
#include <signal.h>
#include "unicode.hpp"

inline const char * statustomsg (int status)
{
#ifdef WIN32
	switch (status)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			return "Access violation";
			break;
		case EXCEPTION_STACK_OVERFLOW:
			return "Stack overflow";
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "Divide by zero";
			break;
		case WORKER_RESTART_STATUS_CODE:
			return "Scheduled restart";
			break;
	}
#else
	switch (status)
	{
		case SIGINT:
			return "INT";
			break;
		case SIGKILL:
			return "KILL";
			break;
		case SIGSEGV:
			return "SEGV - segmentation fault";
			break;
		case SIGTERM:
			return "TERM";
			break;
		case SIGFPE:
			return "FPE - floating point exception";
			break;
		case WORKER_RESTART_STATUS_CODE:
			return "Scheduled restart";
			break;
	}
#endif
	
	return "Unknown";
}

static void setprocname (const char * newprocname)
{
#ifdef WIN32
	SetConsoleTitleA(newprocname);
#else
	extern char ** argv;
	char * procname = argv[0];
	const size_t len = strlen(procname);
	memset(procname, 0, len);
	memcpy(procname, newprocname, min(len, strlen(newprocname)));
#ifdef HAVE_PRCTL_H
	prctl(PR_SET_NAME, newprocname, 0, 0, 0);
#endif
#ifdef HAVE_SETPROCTITLE
	setproctitle(newprocname);
#endif
#endif
}

#ifdef WIN32
static bool find_parent_process ()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return false;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	if (!Process32First(hProcessSnap, &pe32))
		goto fail;
	do
	{
		if (pe32.th32ProcessID == GetCurrentProcessId())
		{
			TCHAR curprocpath[MAX_PATH];
			TCHAR parentprocpath[MAX_PATH];
			if (!GetModuleFileName(0, curprocpath, sizeof(curprocpath) / sizeof(curprocpath[0])))
				goto fail;
			HANDLE hParent = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ParentProcessID);
			if (!hParent)
				goto fail;
			if (!GetModuleFileNameEx(hParent, 0, parentprocpath, sizeof(parentprocpath) / sizeof(parentprocpath[0])))
			{
				CloseHandle(hParent);
				goto fail;
			}
			if (_tcscmp(curprocpath, parentprocpath) == 0)
			{
				CloseHandle(hParent);
				CloseHandle(hProcessSnap);
				return true;
			}
			break;
		}
	}
	while (Process32Next(hProcessSnap, &pe32));
	
	fail:
	CloseHandle(hProcessSnap);
	return false;
}
#endif

static void setup_signals (void (* quit_function) (int))
{
	if (signal(SIGINT, quit_function) == SIG_ERR)
		LOG_ERROR("Can't handle signal " << SIGINT);
	if (signal(SIGTERM, quit_function) == SIG_ERR)
		LOG_ERROR("Can't handle signal " << SIGTERM);
	#ifndef WIN32
	if (signal(SIGQUIT, quit_function) == SIG_ERR)
		LOG_ERROR("Can't handle signal " << SIGQUIT);
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		LOG_ERROR("Can't handle signal " << SIGPIPE);
	#endif
}

#ifdef WIN32
static HANDLE worker_process = 0;
#else
static pid_t worker_process = 0;
#endif

static void quit_master (int status)
{
	if (status == SIGINT)
		cout << endl;
	if (status == SIGINT || status == SIGTERM)
		status = EXIT_SUCCESS;
	if (worker_process)
	{
#ifdef WIN32
		TerminateProcess(worker_process, 0);
		WaitForSingleObject(worker_process, 1000);
		CloseHandle(worker_process);
#else
		kill(worker_process, SIGTERM);
#endif
	}
	process_exit(status);
}

static void quit_worker (int status)
{
	if (status == SIGINT || status == SIGTERM)
		status = EXIT_SUCCESS;
	process_exit(status);
}

void process_init ()
{
	time_t start_time;
	uint respawn_fails = 0;
	
	setup_signals(quit_master);
	setprocname(PROG_NAME " (master)");
	
#ifdef WIN32
	if (!find_parent_process())
	{
#if DEBUG_LEVEL < 1
		for (;;)
		{
			start_time = time(NULL);
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);
			if (CreateProcess(0, GetCommandLine(), 0, 0, FALSE, DETACHED_PROCESS, 0, 0, &si, &pi))
			{
				DWORD status = 0;
				worker_process = pi.hProcess;
				WaitForSingleObject(pi.hProcess, INFINITE);
				GetExitCodeProcess(pi.hProcess, &status);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				
				if (status == WORKER_RESTART_STATUS_CODE)
					continue;
				
				if (status == EXIT_SUCCESS || status == EXIT_FAILURE)
					quit_master(status);
				
				if (time(NULL) - start_time <= 1)
					respawn_fails++;
				else
					respawn_fails = 0;
				
				if (respawn_fails > 10)
					LOG_CRITICAL("Worker process crashed with status " << status << " (" << statustomsg(status) << ')');
				
				DEBUG_PRINT_1("Worker process terminated with status " << status << " (" << statustomsg(status) << ')' << ", respawning...");
			}
			else
				LOG_CRITICAL_P("Could not start worker process");
		}
#endif
	}
	
	opentube::logger.ostdout = false;
	
	{
		DWORD size = 0;
		GetUserNameW(0, &size);
		wchar_t * buf = new wchar_t[size];
		if (!GetUserNameW(buf, &size))
			LOG_CRITICAL_P("Could not get current user name");
		wstring ws = buf;
		string user = unicode::utf16_to_utf8(ws);
		if (opentube::config.user != user)
			LOG_ERROR("Current user \"" << user << "\" does not match \"server.user\" = \"" << opentube::config.user << "\"");
		delete[] buf;
	}
	
	setprocname(PROG_NAME " (worker)");
	setup_signals(quit_worker);
#else
	pid_t pid = 0;
	
	for (;;)
	{
		start_time = time(NULL);
		pid = fork();
		
		if (pid < 0)
			LOG_CRITICAL_P("fork()");
		if (pid == 0)
		{
			pid = getpid();
			errno = 0;
			struct passwd * pwd = getpwnam(opentube::config.user.c_str());
			struct group * grp = getgrnam(opentube::config.group.c_str());
			if (!pwd)
			{
				if (errno == 0)
				{
					LOG_CRITICAL("User \"" << opentube::config.user << "\" not found");
				}
				else
					LOG_CRITICAL_P(opentube::config.user);
			}
			if (setuid(pwd->pw_uid))
				LOG_CRITICAL_P("Can't do setuid(" << opentube::config.user << ')');
			if (grp)
				setgid(grp->gr_gid);
			else
				setgid(pwd->pw_gid);
			setprocname(PROG_NAME " (worker)");
			setup_signals(quit_worker);
			break;
		}
		
		worker_process = pid;
		
		int status;
		waitpid(pid, &status, 0);
		
		if (WIFEXITED(status))
		{
			status = WEXITSTATUS(status);
			
			if (status != WORKER_RESTART_STATUS_CODE)
				quit_master(status);
		}
		else
		{
			status = WTERMSIG(status);
			
			if (time(NULL) - start_time <= 1)
				respawn_fails++;
			else
				respawn_fails = 0;
			
			if (respawn_fails > 10)
				LOG_CRITICAL("Worker process crashed with status " << status << " (" << statustomsg(status) << ')');
		}
		
		DEBUG_PRINT_1("Worker process terminated with status " << status << " (" << statustomsg(status) << ')' << ", respawning...");
	}
#endif
}

typedef void (* exit_handler) (void);
static exit_handler exit_handlers_list[8];
static size_t exit_handlers_len = 0;

void process_before_exit (exit_handler handler)
{
	exit_handlers_list[exit_handlers_len] = handler;
	exit_handlers_len++;
}

void process_exit (int code)
{
	while (exit_handlers_len)
	{
		exit_handlers_len--;
		exit_handlers_list[exit_handlers_len]();
	}
	exit(code);
}
