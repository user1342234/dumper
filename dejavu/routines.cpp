#include "nt.h"
#include "communication.h"
#include "routines.h"

NTSTATUS kread(UINT64 address, ULONG size, void* user_buffer) {

	// Map Usermode buffer to Kernel for copying
	PVOID apc;
	KeStackAttachProcess((PRKPROCESS)global_info.process, &apc);
	PMDL mdl = IoAllocateMdl((PVOID)user_buffer, size, FALSE, FALSE, NULL);
	MmProbeAndLockPages(mdl, KernelMode, IoModifyAccess);
	user_buffer = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, HighPagePriority);
	KeUnstackDetachProcess(&apc);
	

	MM_COPY_ADDRESS AddrToRead = { 0 };
	AddrToRead.PhysicalAddress.QuadPart = address;
	SIZE_T out = 0;
	if (!NT_SUCCESS(MmCopyMemory(user_buffer, AddrToRead, size, MM_COPY_MEMORY_VIRTUAL, &out))) {
		MmUnmapLockedPages(user_buffer, mdl);
		MmUnlockPages(mdl);
		IoFreeMdl(mdl);
		return STATUS_UNSUCCESSFUL;
	}
	MmUnmapLockedPages(user_buffer, mdl);
	MmUnlockPages(mdl);
	IoFreeMdl(mdl);
	return STATUS_SUCCESS;
}
