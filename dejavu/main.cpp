#include "nt.h"
#include "communication.h"


NTSTATUS start() {


    KIRQL irql = KeGetCurrentIrql();
    if (irql > APC_LEVEL)
        KeLowerIrql(PASSIVE_LEVEL);

    // Communication

    UNICODE_STRING     uniName;
    OBJECT_ATTRIBUTES  objAttr;

    RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\map.dat");
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    HANDLE   handle;
    IO_STATUS_BLOCK    ioStatusBlock;
    LARGE_INTEGER      byteOffset;

   
    RtlZeroMemory(&global_info, sizeof(INFO));


    if (!NT_SUCCESS(ZwCreateFile(&handle,
        GENERIC_READ,
        &objAttr, &ioStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0))) 
    {
        return STATUS_UNSUCCESSFUL;
    }
       
    // Setting up our information buffer
    byteOffset.LowPart = byteOffset.HighPart = 0;
    if (!NT_SUCCESS(ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock, (PVOID)&global_info, sizeof(REQUEST), &byteOffset, NULL))) {
        return STATUS_UNSUCCESSFUL;
    }
    if (handle)
        ZwClose(handle);


    
    if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)global_info.process_id, &global_info.process)))
        return STATUS_UNSUCCESSFUL;

    // set up communication page
    PVOID apc;
    KeStackAttachProcess((PRKPROCESS)global_info.process, &apc);
    PMDL mdl = IoAllocateMdl((PVOID)global_info.shared_page, 0x1000, FALSE, FALSE, NULL);
    MmProbeAndLockPages(mdl, KernelMode, IoModifyAccess);
    global_info.shared_page = (UINT64)MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, HighPagePriority);
    KeUnstackDetachProcess(&apc);
    IoFreeMdl(mdl);

    communicate((PREQUEST)global_info.shared_page);
 
    
    return STATUS_SUCCESS;
    
    
}
    