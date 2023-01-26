
#ifndef _KERNEL_HPP
#define _KERNEL_HPP

#include "communication.hpp"
#include <TlHelp32.h>
#include <stdio.h>
#include <mutex>
#include "../math.hpp"
uint64_t GamePid = 6008;
uint64_t CurrentPid;
typedef __int64(*nt_user_init)(uintptr_t, PVOID);
nt_user_init ntusrinit = nullptr;
uint64_t base1;
#define  MAGICODE 0x47DFAED4774

typedef struct _MEMORY_STRUCT
{
	BYTE type;
	LONG usermode_pid;
	LONG target_pid;
	ULONG64 base_address;
	void* address;
	LONG size;
	void* output;
	ULONG magic;
} MEMORY_STRUCT, * PMEMORY_STRUCT;

class KernelInterface
{
public:

	void InitializeHook()
	{
		LoadLibraryA("user32.dll");
		ntusrinit = (nt_user_init)GetProcAddress(::LoadLibraryA("win32u.dll"), "NtCompositionInputThread");

		if (!ntusrinit)
		{
		}
	}

	DWORD_PTR GetClientAddress()
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 12;
		memory_struct.magic = 0x1337;
		memory_struct.target_pid = GamePid;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return memory_struct.base_address;
	}

	DWORD64 GetProcessId(const char* process_name)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (!Process32First(snapshot, &entry) == TRUE)
		{
			return 0;
		}

		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (strcmp(entry.szExeFile, process_name) == 0)
			{
				return (DWORD64)entry.th32ProcessID;
			}
		}

		return 0;
	}
	// reading non-typed info to buffer;
	bool ReadBuffer(ULONG64 ProcessId, void* buffer, ULONG64 ReadAddress, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 3;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(ReadAddress);
		memory_struct.output = buffer;
		memory_struct.size = Size;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}
	bool writeft(uint64_t address, float buffer, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 7;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = Size;
		memory_struct.output = &buffer;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}

	bool write(uint64_t address, UINT32 buffer, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 7;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = Size;
		memory_struct.output = &buffer;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}

	bool writebl(uint64_t address, bool buffer, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 7;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = Size;
		memory_struct.output = &buffer;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}
	bool write32(uint64_t address, int32_t buffer, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 7;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = Size;
		memory_struct.output = &buffer;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}
	bool writeVec(uint64_t address, Vector2 buffer, SIZE_T Size)
	{
		MEMORY_STRUCT memory_struct = { 0 };
		memory_struct.type = 7;
		memory_struct.magic = 0x1337;
		memory_struct.usermode_pid = CurrentPid;
		memory_struct.target_pid = GamePid;
		memory_struct.address = reinterpret_cast<void*>(address);
		memory_struct.size = Size;
		memory_struct.output = &buffer;

		ntusrinit(MAGICODE, (PVOID)&memory_struct);

		return true;
	}

	template <typename type>
	type ReadVirtualMemory(ULONG64 ProcessId, ULONG64 ReadAddress, SIZE_T Size)
	{
		type Buffer;
		ReadBuffer(0, &Buffer, ReadAddress, Size);
		return Buffer;
	}



	template <typename type>
	bool WriteVirtualMemory(ULONG64 ProcessId, ULONG64 WriteAddress, type WriteValue, SIZE_T Size)
	{
		write(WriteAddress, WriteValue, Size);
		return true;
	}

};
KernelInterface kernel;

#endif; // _KERNEL_HPP