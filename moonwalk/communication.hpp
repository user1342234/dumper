

#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <Windows.h>
#include "globals.hpp"
#include "stdio.h"
#include "utilities.hpp"
#define IN_PROGRESS 0


typedef struct _INFO {
    UINT64 process_id;
    UINT64 shared_page;
}INFO, * PINFO;


enum operation_t {
    read_op = 0xFB,
    write_op = 0xFC,
    module_base_op = 0xFD,
    kread_op = 0xFE
};

enum success_op_t {
    success_read = read_op >> 1,
    success_write = write_op >> 1,
    success_module = module_base_op >> 1,
    success_kread = kread_op >> 1
};

enum failure_op_t {
    fail_read = read_op << 1,
    fail_write = write_op << 1,
    fail_module = module_base_op << 1,
    fail_kread = kread_op << 1
};

typedef struct _REQUEST {
    operation_t id;
    PVOID address; // when requesting process base, this is a pointer to a const char*
    PVOID user_buffer;
    ULONG size;
    UINT64 value; // for write operation // also contains requested module base address (if requested)
    char module_name[64];
}REQUEST, * PREQUEST;
constexpr int x = sizeof(REQUEST);

typedef struct _PROCESS_INFO {
    UINT64 process_base;
    UINT64 module_base;
} PROCESS_INFO, *PPROCESS_INFO;


#define ALIGNMENT sizeof(REQUEST)


template <typename T>
T read(UINT64 address) {
    if (sizeof(T) > 8) { return (T)(0); } // if size is greater than 8 return 0.
   volatile PREQUEST req = (PREQUEST)globals::shared_page; // must be aligned
    req->id = (operation_t)IN_PROGRESS; // set to NULL first, so we can set other parameters without driver faulting on incorrect memcpy
    req->address = (PVOID)address;
    printf("address: %llx \n", address);
    req->size = (size_t)sizeof(T);

    req->id = (operation_t)read_op; // now set it
    while (req->id == read_op) {
     
    } 		// hang until we get a response, FIXME: add FAILURE response.
    if (req->id == fail_read) {
        return (T)-1;
    }
    T buffer;
    buffer = *(T*)((UINT64)globals::shared_page + ALIGNMENT);
    // clean buffer
    memset( (PVOID)((UINT64)globals::shared_page + ALIGNMENT), 0, sizeof(UINT64)); // just clear max instead of sizeof)(T), compiler optimizes to 1 move instruction anyways
    return buffer;
}

template <typename T>
T write(UINT64 address, T value) {
    if (sizeof(T) > 8) { return (T)(0); } // if size is greater than 8 return 0.
    volatile PREQUEST req = (PREQUEST)globals::shared_page; // must be aligned
    req->id = (operation_t)IN_PROGRESS; // set to NULL first, so we can set other parameters without driver faulting on incorrect memcpy
    req->address = (PVOID)address;
    printf("address: %llx \n", address);
    req->size = (size_t)sizeof(T);
    req->value = value;

    req->id = (operation_t)write_op; // now set it
    while (req->id == write_op) {
        
    } 		// hang until we get a response, FIXME: add FAILURE response.
    if (req->id == fail_write) {
        return (T)0;
    }
    T buffer;
    buffer = *(T*)((UINT64)globals::shared_page + ALIGNMENT);
    // clean buffer
    memset((PVOID)((UINT64)globals::shared_page + ALIGNMENT), 0, sizeof(UINT64)); // just clear max instead of sizeof)(T), compiler optimizes to 1 move instruction anyways
    return req->id;
}

NTSTATUS kread(UINT64 kernel_address, ULONG size, void* user_buffer);
void request_module_base(const char module_str[64], PPROCESS_INFO information);

#endif