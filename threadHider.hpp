#pragma once
#include "include.hpp"

/*
* APC Queueable - https://www.unknowncheats.me/forum/anti-cheat-bypass/492141-thread-apc-queueable.html
* System thread bit flag
* stack base bit flag
* stack limit bit flag
* start address bit flag
* alertable bit flag
* NMI
* Callstack return spoofer
* Timing checks
* PspCidTable
*/

namespace ThreadHider {
	static _CACHE_THREAD_DATA g_CacheThreadData = { 0 };
	static __forceinline _WINVER_THREAD_OFFSET getRelativeOffset(ULONG build_version) {
		_WINVER_THREAD_OFFSET threadOffset { 0 };
		
		switch (build_version) {
		case WINDOWS10_22H2: {
			threadOffset.initialStack = 0x28;
			threadOffset.stackBase = 0x38;
			threadOffset.kernelStack = 0x58;
			threadOffset.miscFlags = 0x74;
			threadOffset.threadListEntry = 0x2f8;
			threadOffset.createTime = 0x430;
			threadOffset.startAddress = 0x450;
			threadOffset.win32StartAddress = 0x4d0;
			threadOffset.clientID = 0x478;
			threadOffset.exitStatus = 0x548;
			threadOffset.kernelStackReference = 0x55c;
			return threadOffset;
		}
		default: {
			DEBUG_PRINT("build version: %x", build_version);
			return {};
		}
		};
	}

	// Okay retard, you can't call function because you don't have a fucking stack pointer to return to!!!
	static __forceinline bool hideSystemThread(PETHREAD current_thread) {
		RTL_OSVERSIONINFOW ver{};
		RtlGetVersion(&ver);
		g_CacheThreadData.relativeOffsets = getRelativeOffset(ver.dwBuildNumber);

		DEBUG_PRINT("Current Thread PETHREAD: %p", current_thread);
		if (0x0 == g_CacheThreadData.relativeOffsets.initialStack) {
			DEBUG_PRINT("Invalid winver: failed to getRelativeOffset");
			return false;
		}

		auto spoofPointer = [](PVOID* address) ->void { // TODO | maybe? do we even care? :skull:
			*address = reinterpret_cast<PVOID>(0x1);
		};

		g_CacheThreadData.initialStack =
			*reinterpret_cast<void**>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.initialStack);
		
		g_CacheThreadData.kernelStack =
			*reinterpret_cast<void**>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.kernelStack);
		
		g_CacheThreadData.startAddress =
			*reinterpret_cast<void**>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.startAddress);
		
		g_CacheThreadData.win32StartAddress=
			*reinterpret_cast<void**>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.win32StartAddress);
		
		g_CacheThreadData.clientID =
			reinterpret_cast<_CLIENT_ID*>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.clientID);
		
		g_CacheThreadData.exitStatus =
			*reinterpret_cast<long*>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.exitStatus);
		
		g_CacheThreadData.kernelStackReference =
			*reinterpret_cast<long*>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.kernelStackReference);
		
		g_CacheThreadData.flags =
			reinterpret_cast<KTHREAD_MISC_FLAGS*>((ULONG64)current_thread + g_CacheThreadData.relativeOffsets.miscFlags);

		*reinterpret_cast<PVOID*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.initialStack) = (void*)0;
		*reinterpret_cast<PVOID*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.kernelStack) = (void*)0;
		*reinterpret_cast<PVOID*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.startAddress) = (void*)0;
		*reinterpret_cast<PVOID*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.win32StartAddress) = (void*)0;

		/*_LIST_ENTRY* tLE = *reinterpret_cast<_LIST_ENTRY**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.threadListEntry);
		tLE->Blink->Flink = tLE->Flink; 
		
		CLIENT_ID* tCID = *reinterpret_cast<CLIENT_ID**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.clientID);
		tCID->UniqueThread = 0x0;*/

		*reinterpret_cast<ULONG*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.exitStatus) = STATUS_THREAD_IS_TERMINATING;
		*reinterpret_cast<LONG*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.kernelStackReference) = 0;

		g_CacheThreadData.flags->Alertable = 0;
		g_CacheThreadData.flags->ApcQueueable = 0;
		g_CacheThreadData.flags->SystemThread = 0;

		return true;
	}
	
	static __forceinline void revealSystemThread(PKTHREAD current_thread) {
		DEBUG_PRINT("Revealing system thread");

		*reinterpret_cast<void**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.initialStack) = g_CacheThreadData.initialStack;
		*reinterpret_cast<void**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.kernelStack) = g_CacheThreadData.kernelStack;
		*reinterpret_cast<void**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.startAddress) = g_CacheThreadData.startAddress;
		*reinterpret_cast<void**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.win32StartAddress) = g_CacheThreadData.win32StartAddress;

		/*_LIST_ENTRY* tLE = *reinterpret_cast<_LIST_ENTRY**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.threadListEntry);
		tLE->Blink->Flink = tLE;
		
		CLIENT_ID* tCID = *reinterpret_cast<CLIENT_ID**>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.clientID);
		tCID->UniqueThread = g_CacheThreadData.clientID->UniqueThread;*/

		*reinterpret_cast<long*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.exitStatus) = 0;
		*reinterpret_cast<long*>((UINT64)current_thread + g_CacheThreadData.relativeOffsets.kernelStackReference) = 1;

		g_CacheThreadData.flags->Alertable = 1;
		g_CacheThreadData.flags->ApcQueueable = 1;
		g_CacheThreadData.flags->SystemThread = 1;
	}
}