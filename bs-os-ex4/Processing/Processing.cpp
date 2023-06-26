// Processing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <cstdint>
#include <thread>

#define MYNAME "Processing"

// Create handles for each event
HANDLE ghStartProcessingEvent = NULL, ghProcessingFinishedEvent = NULL, ghExitEvent = NULL;

DWORD WINAPI Processing(LPVOID);


int main()
{
	//handles for process and threads
	..

	//set priority and processors
	...
	/*
		15
		THREAD_PRIORITY_TIME_CRITICAL, Base priority of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 31 for REALTIME_PRIORITY_CLASS processes.
		2
		THREAD_PRIORITY_HIGHEST, Priority 2 points above the priority class.
		1
		THREAD_PRIORITY_ABOVE_NORMAL, Priority 1 point above the priority class.
		0
		THREAD_PRIORITY_NORMAL, Normal priority for the priority class.
		- 1
		THREAD_PRIORITY_BELOW_NORMAL, Priority 1 point below the priority class.
		- 2
		THREAD_PRIORITY_LOWEST, Priority 2 points below the priority class.
		- 15
		THREAD_PRIORITY_IDLE, Base priority of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 16 for REALTIME_PRIORITY_CLASS processes.
	*/

	...
	/*
		The SetProcessAffinityMask is inherted to any child process (and threads).
		The SetThreadAffinityMask in threads will be overwritten by this call
		REALTIME_PRIORITY_CLASS 0x00000100
		HIGH_PRIORITY_CLASS 0x00000080
		ABOVE_NORMAL_PRIORITY_CLASS 0x00008000
		NORMAL_PRIORITY_CLASS 0x00000020
		BELOW_NORMAL_PRIORITY_CLASS 0x00004000
		IDLE_PRIORITY_CLASS 0x00000040
		PROCESS_MODE_BACKGROUND_BEGIN 0x00100000
		PROCESS_MODE_BACKGROUND_END 0x00200000
	*/

	//get info
	int processid = ...
	int threadid = ...
	int processornum = ...
	DWORD ppriority = ...
	int tpriority = ...

	std::cout << "Process ID: " << ... << "\nThread ID: " << ... << "\nNow running on processor #" << ...
	<< "\nWith process priority class 0x" << std::hex << ... << "\nWith thread priority " << ... << std::dec << std::endl;


	DWORD dwEvent;
	BOOL bContinue = TRUE;
	volatile int vi, i;

	// open StartProcessingEvent:
	...

	if (...) {
		...
	}

	// open ProcessingFinishedEvent:
	...

	if (...) {
		...
	}

	// open quit event:
	ghExitEvent = OpenEvent(EVENT_ALL_ACCESS | EVENT_MODIFY_STATE, FALSE, TEXT("ExitEvent"));

	if (ghExitEvent == NULL) {
		printf("%s: Error opening event %s! Are you sure Control is running? Hit any key...\n\n", MYNAME, "ExitEvent");
		(void)_getch();
		return 1;
	}

	// wait for events raised by Control) or the quit event (may be set by every participant):
	HANDLE ghEvents[] = { ... }; // array of handles
	int nNumEvents = sizeof(ghEvents) / sizeof(ghEvents[0]);

	printf("%s process is running.\n", MYNAME);

	while (bContinue) {
			dwEvent = WaitForMultipleObjects(
			nNumEvents,	// number of objects to wait for
			ghEvents,	// array of objects to wait for
			FALSE,		// wait for any object ...
			INFINITE);	// ...and for a very long time

		switch (dwEvent) // return value indicates, which event is signalled now
		{
		case WAIT_OBJECT_0 + 0: // StartProcessingEvent
				// Some processing..
				for (i = 0; i < 9000000; i++) { vi = i % 2; }

				// Set ProcessingFinishedEvent
				...
				break;
		case WAIT_OBJECT_0 + 1: // ExitEvent
			bContinue = FALSE;
			break;
		case WAIT_TIMEOUT:
			// no event but timeout
			break;
		default:
			// invalid return value: Error
			printf("%s: Wait error: %d\n", MYNAME, GetLastError());
			ExitProcess(0);
		}

	}

	printf("\n%s: Process stopped. Hit any key to close Window.\n", MYNAME);
	(void)_getch();
	ExitProcess(0);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

