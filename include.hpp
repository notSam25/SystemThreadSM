#pragma once
#include <ntifs.h>
#include <wdm.h>

#define DEBUG_PRINT(fmt, ...) DbgPrintEx(0, 0, "[SystemThread] " fmt "\n", __VA_ARGS__)

#define WINDOWS10_1803 17134
#define WINDOWS10_1809 17763
#define WINDOWS10_1903 18362
#define WINDOWS10_1909 18363
#define WINDOWS10_2004 19041
#define WINDOWS10_20H2 19042
#define WINDOWS10_21H1 19043
#define WINDOWS10_21H2 19044
#define WINDOWS10_22H2 19045
#define WINDOWS11_21H2 22000

typedef struct _KTHREAD_MISC_FLAGS {
	ULONG AutoBoostActive : 1;
	ULONG ReadyTransition : 1;
	ULONG WaitNext : 1;
	ULONG SystemAffinityActive : 1;
	ULONG Alertable : 1;
	ULONG UserStackWalkActive : 1;
	ULONG ApcInterruptRequest : 1;
	ULONG QuantumEndMigrate : 1;
	ULONG UmsDirectedSwitchEnable : 1;
	ULONG TimerActive : 1;
	ULONG SystemThread : 1;
	ULONG ProcessDetachActive : 1;
	ULONG CalloutActive : 1;
	ULONG ScbReadyQueue : 1;
	ULONG ApcQueueable : 1;
	ULONG ReservedStackInUse : 1;
	ULONG UmsPerformingSyscall : 1;
	ULONG TimerSuspended : 1;
	ULONG SuspendedWaitMode : 1;
	ULONG SuspendSchedulerApcWait : 1;
	ULONG CetUserShadowStack : 1;
	ULONG BypassProcessFreeze : 1;
	ULONG Reserved : 10;
} KTHREAD_MISC_FLAGS;

typedef struct _WINVER_THREAD_OFFSET {
	UINT64 initialStack{},
		stackBase{},
		kernelStack{},
		miscFlags{},
		threadListEntry{},
		createTime{},
		startAddress{},
		win32StartAddress{},
		clientID{}, // CID
		exitStatus{},
		kernelStackReference = {};
};

struct _CACHE_THREAD_DATA {
	void *initialStack = 0, 
		*kernelStack = 0,
		*startAddress = 0,
		*win32StartAddress = 0;
	_CLIENT_ID* clientID = nullptr;
	LONG exitStatus = 0;
	ULONG kernelStackReference = 0;
	KTHREAD_MISC_FLAGS* flags = nullptr;
	_WINVER_THREAD_OFFSET relativeOffsets = {};
};