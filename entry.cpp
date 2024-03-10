#include "include.hpp"
#include "threadHider.hpp"

NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath
);

__forceinline NTSTATUS delayExec(ULONG64 ms) {
	DEBUG_PRINT("Started waiting");
	LARGE_INTEGER waitTime;
	waitTime.QuadPart = -((ms) * 10000);
	return KeDelayExecutionThread(KernelMode, false, &waitTime);
}

void _DriverThread() {
	DEBUG_PRINT("entered driver thread: 0x%p | 0x%p", &DriverEntry, &ThreadHider::hideSystemThread);

	if (false == ThreadHider::hideSystemThread(KeGetCurrentThread())) {
		DEBUG_PRINT("System thread hider routine returned unsuccessfully");
		PsTerminateSystemThread(STATUS_UNSUCCESSFUL);
	}
	DEBUG_PRINT("System thread hider routine successfully executed");

	/*PETHREAD threadID;
	NTSTATUS result = PsLookupThreadByThreadId(PsGetCurrentThreadId(), &threadID);
	if (STATUS_SUCCESS != result) {
		DEBUG_PRINT("Failed to find thread by ID");
	}*/

	/*NTSTATUS result = delayExec(5000);
	if (STATUS_SUCCESS != result) {
		DEBUG_PRINT("Failed to wait, result: %x", result);
		ThreadHider::revealSystemThread(KeGetCurrentThread());
		PsTerminateSystemThread(STATUS_UNSUCCESSFUL);
	}
	DEBUG_PRINT("Done waiting");*/

	ThreadHider::revealSystemThread(KeGetCurrentThread());
	DEBUG_PRINT("Exiting system thread");
	PsTerminateSystemThread(STATUS_SUCCESS);
}

NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath
) {
	HANDLE threadHandle = {};

	DEBUG_PRINT("Hello from DriverEntry");
	NTSTATUS result = PsCreateSystemThread(&threadHandle,
						SYNCHRONIZE,
						NULL,
						NULL,
						NULL,
						reinterpret_cast<PKSTART_ROUTINE>(_DriverThread),
						NULL);
	if (STATUS_SUCCESS != result) {
		DEBUG_PRINT("Failed to create system thread: 0x%x", result);
		return result;
	}
	
	result = ZwClose(threadHandle);
	if (STATUS_SUCCESS != result) {
		DEBUG_PRINT("Failed to free handle to system thread: 0x%x", result);
		return result;
	}
	
	return STATUS_SUCCESS;
}