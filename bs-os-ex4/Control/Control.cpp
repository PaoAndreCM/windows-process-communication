// Control.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <cstdint>
#include <thread>

#define MYNAME "Control"

// Create handles for each event
HANDLE ghInputEvent = 0, ghStartProcessingEvent = 0, ghProcessingFinishedEvent = 0, ghOutputEvent = 0, ghExitEvent = 0;

DWORD WINAPI Control(LPVOID);

int main()
{ 
	//handles for process and threads 
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();
	// The handles returned by GetCurrentProcess() and GetCurrentThread() are pseudo-handles 
	// that do not need to be explicitly closed. They are valid for the lifetime of the process 
	// and thread, and the system automatically closes them when the process and thread terminate.

	//set priority and processors
	int threadPriority = THREAD_PRIORITY_NORMAL;
	DWORD processAffinityMask = 0x00000001; // Run on the first processor by default

	// Set the thread priority
	if (!SetThreadPriority(hThread, threadPriority))
	{
		printf("%s: Failed to set thread priority. Error code: %d\n", MYNAME, GetLastError());
	}

	// Set the process affinity mask
	if (!SetProcessAffinityMask(hProcess, processAffinityMask))
	{
		printf("%s: Failed to set process affinity mask. Error code: %d\n", MYNAME, GetLastError());
	}
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

		//...
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
	int processid = GetCurrentProcessId();
	int threadid = GetCurrentThreadId();
	int processornum = GetCurrentProcessorNumber();
	DWORD ppriority = GetPriorityClass(hProcess);
	int tpriority = GetThreadPriority(hThread);

	std::cout << "Process ID: " << processid << "\nThread ID: " << threadid << "\nNow running on processor #" << processornum
		<< "\nWith process priority class 0x" << std::hex << ppriority << "\nWith thread priority " << tpriority << std::dec << std::endl;

	BOOL bContinue = TRUE;
	DWORD dwEvent;

	// Create events

	// create input event:
	ghInputEvent = CreateEvent(
		NULL,						// default security attributes
		FALSE,						// auto-reset event object
		FALSE,						// initial state: nonsignaled
		L"InputEvent");	// name of the event to adress from other processes


	if (ghInputEvent == NULL)
	{
		printf("%s: Create InputEvent error: %d\n", MYNAME, GetLastError());
		ExitProcess(0);
	}

	// create start processing event:
	ghStartProcessingEvent = CreateEvent(
		NULL,						// default security attributes
		FALSE,						// auto-reset event object
		FALSE,						// initial state: nonsignaled
		L"StartProcessingEvent");	// name of the event to adress from other processes
	if (ghStartProcessingEvent == NULL)
	{
		printf("%s: Create StartProcessingEvent error: %d\n", MYNAME, GetLastError());
		ExitProcess(0);
	}

	// create processing finished event:
	ghProcessingFinishedEvent = CreateEvent(
		NULL,						// default security attributes
		FALSE,						// auto-reset event object
		FALSE,						// initial state: nonsignaled
		L"ProcessingFinishedEvent");	// name of the event to adress from other processes

	if (ghProcessingFinishedEvent == NULL)
	{
		printf("%s: Create ProcessingFinishedEvent error: %d\n", MYNAME, GetLastError());
		ExitProcess(0);
	}

	// create output event:
	ghOutputEvent = CreateEvent(
		NULL,						// default security attributes
		FALSE,						// auto-reset event object
		FALSE,						// initial state: nonsignaled
		L"ProcessingFinishedEvent");	// name of the event to adress from other processes

	if (ghOutputEvent == NULL)
	{
		printf("%s: Create OutputEvent error: %d\n", MYNAME, GetLastError());
		ExitProcess(0);
	}

	// create exit event:
	ghExitEvent = CreateEvent(
		NULL,						// default security attributes
		TRUE,						// Manual Reset! All processes / threads wait for this
		FALSE,						// initial state: nonsignaled
		TEXT("ExitEvent"));	// name of the event to adress from other processes

	if (ghExitEvent == NULL)
	{
		printf("%s: Create ExitEvent error: %d\n", MYNAME, GetLastError());
		ExitProcess(0);
	}


	// array of handles to wait for:
	HANDLE ghEvents[] = { ghInputEvent, ghProcessingFinishedEvent, ghExitEvent };
	int nNumEvents = sizeof(ghEvents) / sizeof(ghEvents[0]);

	printf("%s process is running.\n", MYNAME);

	while (bContinue) {
		
		// wait (e. g. for events released by other processes):
		dwEvent = WaitForMultipleObjects(
			nNumEvents,	// number of objects to wait for
			ghEvents,	// array of objects to wait for
			FALSE,		// wait for any object ...
			INFINITE);	// ...and for a very long time

		switch (dwEvent) // return value indicates, which event is signalled now
		{
		case WAIT_OBJECT_0 + 0: // InputEvent
			// set StartProcessingEvent
			printf("%s: I got the InputEvent. Now I will set StartProcessingEvent.\n", MYNAME);
			if (!SetEvent(ghStartProcessingEvent)) {
				printf("%s: Error %d SetEvent()\n", MYNAME, GetLastError());
				bContinue = FALSE;
			}
			else {
				printf("%s: Command 'start processing' sent.", MYNAME);
			}
			break;
		case WAIT_OBJECT_0 + 1: // ProcessingFinishedEvent
		// set OutputEvent
			printf("%s: I got the ProcessingFinishedEvent. Now I will set OutputEvent.\n", MYNAME);
			if (!SetEvent(ghOutputEvent)) {
				printf("%s: Error %d SetEvent()\n", MYNAME, GetLastError());
				bContinue = FALSE;
			}
			else {
				printf("%s: Command 'output' sent", MYNAME);
			}
			break;
		case WAIT_OBJECT_0 + 2: // // ExitEvent
			printf("%s: I got the event %s!\n\n", MYNAME, "ExitEvent");
			bContinue = FALSE;
			break;
		case WAIT_TIMEOUT:
			// no event has been signalled but timeout instead - this should not happen
			break;
		default:
			// invalid return value: Error
			printf("%s: Wait error: %d\n", MYNAME, GetLastError());
			ExitProcess(0);
		}

	}

printf("\n%s: Process stopped. Hit any key to close Window.\n", MYNAME);
(void)_getch();

for (int i = 0; i < nNumEvents; i++) {
	CloseHandle(ghEvents[i]);
	}
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

