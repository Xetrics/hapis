#pragma once

#include <Windows.h>

#define ATHREAD_NOTHREAD 0
#define ATHREAD_TIMEOUT	1
#define ATHREAD_FINISHED 2

namespace util
{
	/* simple thread class for Windows by Austin */
	class athread
	{
	private:
		HANDLE thread;
		int threadId;

	public:
		athread()
		{
			reset();
		}

		athread(void* f, void* args)
		{
			reset(f, args);
		}

		athread(const athread& other)
		{
			thread = other.thread;
			threadId = other.threadId;
		}

		void reset(void* f, void* args)
		{
			terminate();
			thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)f, args, 0, (LPDWORD)&threadId);
		}

		void reset()
		{
			terminate();
			thread = 0;
			threadId = 0;
		}

		HANDLE get_thread()
		{
			return thread;
		}

		int get_thread_id()
		{
			return threadId;
		}

		bool get_exit_code(int* exit_code)
		{
			if (wait(0) == ATHREAD_FINISHED)
			{
				GetExitCodeThread(thread, (LPDWORD)exit_code);
				return true;
			}

			return false;
		}

		bool running()
		{
			return wait(0) == ATHREAD_TIMEOUT;
		}

		void terminate(int exitcode = 0)
		{
			if (running())
				TerminateThread(thread, exitcode);
		}

		int wait(int timeout = INFINITE)
		{
			if (!thread)
				return ATHREAD_NOTHREAD;

			if (WaitForSingleObject(thread, timeout) == WAIT_TIMEOUT)
				return ATHREAD_TIMEOUT;

			return ATHREAD_FINISHED;
		}
	};
}