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

	// http://www.songho.ca/opengl/gl_anglestoaxes.html
	void anglesToAxes(const Rust::Vector3 angles, Rust::Vector3& left, Rust::Vector3& up, Rust::Vector3& forward)
	{
		const float DEG2RAD = 3.141593f / 180;
		float sx, sy, sz, cx, cy, cz, theta;

		// rotation angle about X-axis (pitch)
		theta = angles.x * DEG2RAD;
		sx = sinf(theta);
		cx = cosf(theta);

		// rotation angle about Y-axis (yaw)
		theta = angles.y * DEG2RAD;
		sy = sinf(theta);
		cy = cosf(theta);

		// rotation angle about Z-axis (roll)
		theta = angles.z * DEG2RAD;
		sz = sinf(theta);
		cz = cosf(theta);

		// determine left axis
		left.x = cy*cz;
		left.y = sx*sy*cz + cx*sz;
		left.z = -cx*sy*cz + sx*sz;

		// determine up axis
		up.x = -cy*sz;
		up.y = -sx*sy*sz + cx*cz;
		up.z = cx*sy*sz + sx*cz;

		// determine forward axis
		forward.x = sy;
		forward.y = -sx*cy;
		forward.z = cx*cy;
	}
}