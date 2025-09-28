#ifndef NT_H
#define NT_H
#include <wdm.h>

typedef UINT64 QWORD;
typedef UINT16 WORD;
typedef UINT32 DWORD;

#define MM_COPY_MEMORY_PHYSICAL             0x1
#define MM_COPY_MEMORY_VIRTUAL              0x2

typedef struct _MM_COPY_ADDRESS {
    union {
        PVOID            VirtualAddress;
        PHYSICAL_ADDRESS PhysicalAddress;
    };
} MM_COPY_ADDRESS, * PMMCOPY_ADDRESS;

EXTERN_C NTKERNELAPI NTSTATUS MmCopyMemory(
    IN PVOID           TargetAddress,
    IN MM_COPY_ADDRESS SourceAddress,
    IN SIZE_T          NumberOfBytes,
    IN ULONG           Flags,
    OUT PSIZE_T         NumberOfBytesTransferred
);

EXTERN_C 
NTSTATUS
PsLookupProcessByProcessId(
    _In_ HANDLE ProcessId,
    _Outptr_ PEPROCESS* Process
);
EXTERN_C 
NTKERNELAPI
VOID
KeStackAttachProcess(
    _Inout_ PRKPROCESS PROCESS,
    _Out_ PVOID ApcState
);

EXTERN_C 
NTKERNELAPI
VOID
KeUnstackDetachProcess(
    _In_ PVOID ApcState
);

#endif
